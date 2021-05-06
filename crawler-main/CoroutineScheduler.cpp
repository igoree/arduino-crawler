#include "CoroutineScheduler.h"
#include "Arduino.h"

#include "debug.h"

#define UNKNOWN_INDEX UINT8_C(255)

struct CoroutineTaskState
{
	CoroutineTaskState();
	CoroutineTaskState(CoroutineTask task);

	CoroutineTask task;
	CoroutineStep step;
	unsigned long executeAfterMillis;
};

// CoroutineTask

CoroutineTask::CoroutineTask()
	: CoroutineTask(nullptr)
{
}

CoroutineTask::CoroutineTask(AsyncFuncPointer func, void* data = nullptr)
	: func(func), data(data)
{
}

// CoroutineTaskState

CoroutineTaskState::CoroutineTaskState()
	: task()
{
}

CoroutineTaskState::CoroutineTaskState(CoroutineTask task)
	: task(task)
{
}

// Coroutine

Coroutine::Coroutine(const char* name, uint8_t stackSize)
	: _name(name), _stackSize(stackSize), _currentTaskIndex(UNKNOWN_INDEX), _stack(new CoroutineTaskState[stackSize])
{
}

Coroutine::~Coroutine()
{
	delete[] _stack;
}

void Coroutine::start(CoroutineTask task)
{
	if (_stackSize == 0)
	{
		DEBUG_ERR("CR '%s' zero stack", _name);
		return;
	}

	if (task.func == nullptr)
	{
		DEBUG_ERR("CR '%s' null task", _name);
		return;
	}

	_currentTaskIndex = 0;
	_stack[_currentTaskIndex] = CoroutineTaskState(task);

	DEBUG_INFO("CR '%s' start task", _name);
}

void Coroutine::switchTo(CoroutineTask task)
{
	if (_currentTaskIndex == UNKNOWN_INDEX)
	{
		start(task);
		return;
	}

	if (task.func == nullptr)
	{
		DEBUG_ERR("CR '%s' null task", _name);
		return;
	}

	if (_currentTaskIndex == _stackSize - 1)
	{
		DEBUG_ERR("CR '%s' stack overflow", _name);
		return;
	}

	if (_stack[_currentTaskIndex].step != UNKNOWN_INDEX)
	{
		_currentTaskIndex++;
	}
	else
	{
		DEBUG_INFO("CR '%s' complete task %d", _name, _currentTaskIndex);
	}

	_stack[_currentTaskIndex] = CoroutineTaskState(task);

	DEBUG_INFO("CR '%s' switch task to %d", _name, _currentTaskIndex);
}

void Coroutine::continueExecution()
{
	if (_currentTaskIndex == UNKNOWN_INDEX)
		return;

	while (_stack[_currentTaskIndex].step == UNKNOWN_INDEX)
	{
		DEBUG_INFO("CR '%s' complete task %d", _name, _currentTaskIndex);

		if (_currentTaskIndex == 0)
		{
			_currentTaskIndex = UNKNOWN_INDEX;

			DEBUG_INFO("CR '%s' finish", _name);

			return;
		}

		_currentTaskIndex--;
	}

	if (_stack[_currentTaskIndex].executeAfterMillis > millis())
	{
		CoroutineExecutionContext context(this, &_stack[_currentTaskIndex]);

		DEBUG_INFO("CR '$s' execute step %d", _name, context.getCurrentStep());

		auto nextStep = _stack[_currentTaskIndex].task.func(&context);
		if (nextStep == UNKNOWN_INDEX) 
		{
			// todo: handle completed task paying attention to task completion in switchTo method
		}
	}
}