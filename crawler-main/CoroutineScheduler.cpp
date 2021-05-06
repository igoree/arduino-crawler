#include "CoroutineScheduler.h"

#define EMPTY_ARRAY_INDEX UINT8_C(255)

Coroutine::Coroutine(uint8_t stackSize)
	: _stackSize(stackSize)
{
	_stack = new CoroutineTaskState[_stackSize];
	_currentFuncIndex = EMPTY_ARRAY_INDEX;
}

Coroutine::~Coroutine()
{
	delete[] _stack;
}

CoroutineExecutionContext::CoroutineExecutionContext(Coroutine* coroutine, CoroutineTaskState* funcState)
{
	_coroutine = coroutine;
	_funcState = funcState;
}

CoroutineExecutionContext::~CoroutineExecutionContext()
{
}

CoroutineStep CoroutineExecutionContext::getCurrentStep()
{
	return _funcState->step;
}

void* CoroutineExecutionContext::getData()
{
	return _funcState->task.data;
}
