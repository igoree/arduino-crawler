#include "OrientationSensor.h"
#include "Coroutine.h"

#include "DebugLevels.h"
#define DEBUG_LEVEL DEBUG_LEVEL_INFO
#include "DebugOutput.h"

Coroutine _freeMemoryCoroutine("freeMemory");
Coroutine _orientationCoroutine("orientation");
Coroutine _orientationDebugCoroutine("orientationDebug");

int _lastFreeMemory(0);
OrientationSensor orientationSensor(&_orientationCoroutine);

void setup() 
{
	INIT_DEBUG();

	orientationSensor.initialize();

	DEBUG_INFO("init ok");

#if DEBUG_LEVEL <= DEBUG_LEVEL_INFO
	_freeMemoryCoroutine.start(CoroutineTask(&monitorFreeMemoryAsync));
	_orientationDebugCoroutine.start(CoroutineTask(&printOrientationAsync));
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

	return context->delayThenRepeat(500);
}

CoroutineTaskResult* printOrientationAsync(const CoroutineTaskContext* context)
{
	auto orientation = orientationSensor.getOrientation();
	DEBUG_INFO("orientation: %d,%d,%d", orientation.yaw, orientation.pitch, orientation.roll);

	return context->delayThenRepeat(200);
}

void loop() 
{
	_freeMemoryCoroutine.continueExecution();
	_orientationCoroutine.continueExecution();
	_orientationDebugCoroutine.continueExecution();
}

