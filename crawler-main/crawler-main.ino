#include "ObstacleSensor.h"
#include "Coroutine.h"
#include "Crawler.h"

#include "DebugLevels.h"
#define DEBUG_LEVEL DEBUG_LEVEL_INFO
#include "DebugOutput.h"

#if DEBUG_LEVEL <= DEBUG_LEVEL_INFO
Coroutine _freeMemoryCoroutine("freeMemory");
#endif
Coroutine _irRemoteCoroutine("IRRemote");
Coroutine _lightCoroutine("speedLight", 3);
Coroutine _soundCoroutine("sound", 4);

Crawler _crawler;

int _lastFreeMemory(0);

void setup()
{
	INIT_DEBUG();
	_crawler.init();
	_crawler.setSpeed(50);

	_crawler.initObstacleSensor();
	_crawler.initLights(&_lightCoroutine);
	_crawler.initSoundPlayer(&_soundCoroutine);
	_crawler.initIRRemote(&_irRemoteCoroutine);
	_crawler.initBehaviour();

	DEBUG_INFO("init ok");

	_crawler.playSound(Sound::TurnOn);
	_crawler.showLightEffect(LightEffect::TurnOn);

#if DEBUG_LEVEL <= DEBUG_LEVEL_INFO
	_freeMemoryCoroutine.start(CoroutineTask(&monitorFreeMemoryAsync));
#endif
}

CoroutineTaskResult* monitorFreeMemoryAsync(const CoroutineTaskContext* context)
{
	auto freeMemory = debug_freeMemory();
	if (freeMemory != _lastFreeMemory)
	{
		DEBUG_INFO("free memory: %d", freeMemory);

		_lastFreeMemory = freeMemory;
	}

	DEBUG_INFO("distance: %u", _crawler.getObstacleDistance());

	return context->delayThenRepeat(500);
}

void loop()
{
	_irRemoteCoroutine.continueExecution();
	_lightCoroutine.continueExecution();
	_soundCoroutine.continueExecution();

#if DEBUG_LEVEL <= DEBUG_LEVEL_INFO
	_freeMemoryCoroutine.continueExecution();
#endif
}