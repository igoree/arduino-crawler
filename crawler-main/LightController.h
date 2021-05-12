// LightController.h

#ifndef _LIGHTCONTROLLER_h
#define _LIGHTCONTROLLER_h

#include "stdint.h"
#include "Coroutine.h"

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
	LightController(RGBLed* rgbLed, Coroutine* coroutine);
	~LightController();

	void show(LightEffect effect);
	void repeat(LightEffect effect);
	void lightOff();
};

#endif
