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
		: rgbLed(rgbLed), leftLed(), rightLed(), duration(0), targetDuration(0)
	{
	}

	RGBLed* const rgbLed;
	AsyncFuncPointer repeatedLightFunc;
	LedState leftLed;
	LedState rightLed;
	uint32_t duration;
	uint32_t targetDuration;
};

LightController::LightController(RGBLed* rgbLed, Coroutine* coroutine)
	: _coroutine(coroutine), _state(new LightState(rgbLed))
{
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

void performColorTransition(RGBColor &color, RGBColor &targetColor, uint32_t duration, uint32_t totalDuration)
{
	color.red = performColorTransition(color.red, targetColor.red, duration, totalDuration);
	color.green = performColorTransition(color.green, targetColor.green, duration, totalDuration);
	color.blue = performColorTransition(color.blue, targetColor.blue, duration, totalDuration);
}

CoroutineTaskResult* showLightTransitionAsync(const CoroutineTaskContext* context)
{
	auto state = (LightState*)context->data;

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

	return context->delayThenRepeat(stepDuration);
}

void LightController::show(LightEffect effect)
{
}

void LightController::repeat(LightEffect effect)
{
}

void LightController::lightOff()
{
}