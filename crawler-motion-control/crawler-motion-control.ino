#include "Coroutine.h"

#include "DebugLevels.h"
#define DEBUG_LEVEL DEBUG_LEVEL_INFO
#include "DebugOutput.h"

Coroutine _freeMemoryCoroutine("freeMemory");

int _lastFreeMemory(0);

void setup() 
{
	INIT_DEBUG();
	DEBUG_INFO("init ok");

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

	return context->delayThenRepeat(500);
}


void loop() 
{
	_freeMemoryCoroutine.continueExecution();
}

