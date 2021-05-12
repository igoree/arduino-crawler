#include "LightController.h"
#include "RGBLed.h"

#include "DebugLevels.h"
#define DEBUG_LEVEL DEBUG_LEVEL_INFO
#include "DebugOutput.h"

#define LEFT_LED 2u
#define RIGHT_LED 1u

#define RGB_RED     RGBColor(0xFF, 0x00, 0x00)
#define RGB_GREEN   RGBColor(0x00, 0xFF, 0x00)
#define RGB_BLUE    RGBColor(0x00, 0x00, 0xFF)
#define RGB_YELLOW  RGBColor(0xFF, 0xFF, 0x00)
#define RGB_PURPLE  RGBColor(0xFF, 0x00, 0xFF)
#define RGB_ORANGE  RGBColor(0xFF, 0xA5, 0x00)
#define RGB_INDIGO  RGBColor(0x4B, 0x00, 0x82)
#define RGB_VIOLET  RGBColor(0x8A, 0x2B, 0xE2)
#define RGB_WHITE   RGBColor(0xFF, 0xFF, 0xFF)
#define RGB_BLACK   RGBColor(0x00, 0x00, 0x00)

#define LIGHT_TRANSITION_STEP_DURATION 10ul

struct LedState
{
	LedState()
		: color(RGB_BLACK), targetColor(RGB_BLACK)
	{
	}

	RGBColor color;
	RGBColor targetColor;
};

struct LightState
{
	LightState(RGBLed* rgbLed)
		: rgbLed(rgbLed), repeatedLightEffectFunc(nullptr), repeatedLightEffectBrightness(MAX_BRIGHTNESS), brightness(MAX_BRIGHTNESS), leftLed(), rightLed(), duration(0), targetDuration(0)
	{
	}

	RGBLed* const rgbLed;
	AsyncFuncPointer repeatedLightEffectFunc;
	uint8_t repeatedLightEffectBrightness;
	uint8_t brightness;
	LedState leftLed;
	LedState rightLed;
	uint32_t duration;
	uint32_t targetDuration;
};

LightController::LightController(RGBLed* rgbLed, Coroutine* lightCoroutine)
	: _coroutine(lightCoroutine), _state(new LightState(rgbLed))
{
}

LightController::~LightController()
{
	delete _state;
}

void showLighs(LightState* state)
{
	state->rgbLed->setColor(LEFT_LED, state->leftLed.color);
	state->rgbLed->setColor(RIGHT_LED, state->rightLed.color);
	state->rgbLed->show();
}

uint8_t performColorTransition(uint8_t color, uint8_t targetColor, uint32_t duration, uint32_t totalDuration)
{
	if (color == targetColor)
	{
		return targetColor;
	}
	else if (color > targetColor)
	{
		return color - (color - targetColor) * duration / totalDuration;
	}
	else
	{
		return color + (targetColor - color) * duration / totalDuration;
	}
}

void performColorTransition(RGBColor& color, RGBColor& targetColor, uint32_t duration, uint32_t totalDuration)
{
	color.red = performColorTransition(color.red, targetColor.red, duration, totalDuration);
	color.green = performColorTransition(color.green, targetColor.green, duration, totalDuration);
	color.blue = performColorTransition(color.blue, targetColor.blue, duration, totalDuration);
}

CoroutineTaskResult* showLightColorTransitionAsync(const CoroutineTaskContext* context)
{
	auto state = (LightState*)context->data;

	if (context->step == 0u)
	{
		if (state->leftLed.color == state->leftLed.targetColor && state->rightLed.color == state->rightLed.targetColor)
		{
			state->duration = state->targetDuration;

			return context->delayThenNext(state->targetDuration);
		}
	}
	else if (context->step == 1u)
	{
		return context->end();
	}

	showLighs(state);

	if (state->duration >= state->targetDuration)
	{
		return context->end();
	}

	auto stepDuration = LIGHT_TRANSITION_STEP_DURATION;
	if (state->duration + stepDuration > state->targetDuration)
	{
		stepDuration = state->targetDuration - state->duration;

		state->leftLed.color = state->leftLed.targetColor;
		state->rightLed.color = state->rightLed.targetColor;
	}
	else
	{
		auto remainedDuration = state->targetDuration - state->duration;

		performColorTransition(state->leftLed.color, state->leftLed.targetColor, stepDuration, remainedDuration);
		performColorTransition(state->rightLed.color, state->rightLed.targetColor, stepDuration, remainedDuration);
	}

	state->duration += stepDuration;

	return context->delayThenGoTo(stepDuration, 2u);
}

uint8_t applyBrightness(uint8_t color, uint8_t brightness)
{
	if (brightness >= MAX_BRIGHTNESS)
	{
		return color;
	}
	else if (brightness == 0u)
	{
		return 0u;
	}
	else 
	{
		return color * (uint16_t) brightness / MAX_BRIGHTNESS;
	}
}

RGBColor applyBrightness(RGBColor& color, uint8_t brightness)
{
	return RGBColor(applyBrightness(color.red, brightness), applyBrightness(color.green, brightness), applyBrightness(color.blue, brightness));
}

void prepareColorTransition(LightState* state, RGBColor leftColor, RGBColor rightColor, uint32_t duration)
{
	state->leftLed.color = state->rgbLed->getColor(LEFT_LED);
	state->leftLed.targetColor = applyBrightness(leftColor, state->brightness);

	state->rightLed.color = state->rgbLed->getColor(RIGHT_LED);
	state->rightLed.targetColor = applyBrightness(rightColor, state->brightness);

	state->duration = 0ul;
	state->targetDuration = duration;
}

CoroutineTaskResult* showEffectAsync(const CoroutineTaskContext* context)
{
	auto state = (LightState*)context->data;

	switch (context->step)
	{
	default:
		return context->end();
	}
}

CoroutineTaskResult* repeatLightEffectAsync(const CoroutineTaskContext* context)
{
	auto state = (LightState*)context->data;
	if (state->repeatedLightEffectFunc == nullptr)
		return context->end();

	state->brightness = state->repeatedLightEffectBrightness;

	return context->executeThenRepeat(CoroutineTask(state->repeatedLightEffectFunc, state));
}

AsyncFuncPointer getLightEffectFunc(LightEffect effect)
{
	switch (effect)
	{
	default:
		DEBUG_ERR("unsupported light effect %u", (uint8_t)effect);
		return nullptr;
	}
}

void LightController::show(LightEffect effect, uint8_t brightness = MAX_BRIGHTNESS)
{
	auto lightEffectFunc = getLightEffectFunc(effect);
	if (lightEffectFunc == nullptr)
		return;

	_state->brightness = brightness;

	if (_state->repeatedLightEffectFunc != nullptr)
	{
		_coroutine->start(CoroutineTask(&repeatLightEffectAsync, _state));
		_coroutine->switchTo(CoroutineTask(lightEffectFunc, _state));
	}
	else
	{
		_coroutine->start(CoroutineTask(lightEffectFunc, _state));
	}
}

void LightController::repeat(LightEffect effect, uint8_t brightness = MAX_BRIGHTNESS)
{
	auto lightEffectFunc = getLightEffectFunc(effect);
	if (lightEffectFunc == nullptr)
		return;

	_state->repeatedLightEffectBrightness = brightness;
	_state->repeatedLightEffectFunc = lightEffectFunc;
	_coroutine->start(CoroutineTask(&repeatLightEffectAsync, _state));
}

void LightController::lightOff()
{
	_state->repeatedLightEffectFunc = nullptr;
	_state->brightness = MAX_BRIGHTNESS;

	prepareColorTransition(_state, RGB_BLACK, RGB_BLACK, 50);

	_coroutine->start(CoroutineTask(&showLightColorTransitionAsync, _state));
}