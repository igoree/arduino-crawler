// LightController.h

#ifndef _LIGHTCONTROLLER_h
#define _LIGHTCONTROLLER_h

#include "stdint.h"
#include "Coroutine.h"

#define MAX_BRIGHTNESS 100u

enum class LightEffect : uint8_t
{
	Command,
	FrontLights,
	RearLights,
	LeftTurnSignal,
	RightTurnSignal,
	SpeedChange,
	TurnOn,
	Police
};

class RGBLed;
struct LightState;

class LightController
{
private:
	Coroutine* const _coroutine;
	LightState* const _state;
public:
	LightController(RGBLed* rgbLed, Coroutine* lightCoroutine);
	~LightController();

	void show(LightEffect effect, uint8_t brightness = MAX_BRIGHTNESS);
	void repeat(LightEffect effect, uint8_t brightness = MAX_BRIGHTNESS);
	void lightOff();
};

#endif
