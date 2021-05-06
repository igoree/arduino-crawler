#include "CoroutineScheduler.h"
#include "Arduino.h"

#include "debug.h"

#define UNKNOWN_INDEX UINT8_C(255)

struct CoroutineTaskState
{
	CoroutineTaskState();
	CoroutineTaskState(CoroutineTask task);

	CoroutineTask task;
	uint8_t step;
	unsigned long executeAfterMillis;
};

enum class CoroutineTaskResultKind : uint8_t
{
	Continue,
	Finish
};

struct CoroutineTaskResult
{
	uint8_t nextStep;
	unsigned long delayMillis;
	CoroutineTask taskToSwitch;
	CoroutineTaskResultKind resultKind;
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
		DEBUG_ERR("CR '%s' null func", _name);
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
		DEBUG_ERR("CR '%s' null func", _name);
		return;
	}

	if (_currentTaskIndex == _stackSize - 1)
	{
		DEBUG_ERR("CR '%s' stack overflow", _name);
		return;
	}

	_currentTaskIndex++;
	_stack[_currentTaskIndex] = CoroutineTaskState(task);

	DEBUG_INFO("CR '%s' switch task to %d", _name, _currentTaskIndex);
}

void Coroutine::continueExecution()
{
	if (_currentTaskIndex == UNKNOWN_INDEX)
		return;

	if (_stack[_currentTaskIndex].executeAfterMillis > 0 && _stack[_currentTaskIndex].executeAfterMillis < millis())
		return;

	CoroutineTaskResult initialResult;
	CoroutineTaskContext context(&_stack[_currentTaskIndex], &initialResult);

	DEBUG_INFO("CR '$s' execute step %d", _name, context.step);

	auto result = _stack[_currentTaskIndex].task.func(&context);

	if (result->resultKind == CoroutineTaskResultKind::Finish)
	{
		DEBUG_INFO("CR '%s' complete task %d", _name, _currentTaskIndex);

		if (_currentTaskIndex == 0)
		{
			_currentTaskIndex = UNKNOWN_INDEX;

			DEBUG_INFO("CR '%s' finish", _name);
		}
		else
		{
			_currentTaskIndex--;
		}
	}
	else 
	{
		_stack[_currentTaskIndex].step = result->nextStep;
		
		if (result->delayMillis > 0) 
		{
			_stack[_currentTaskIndex].executeAfterMillis = millis() + result->delayMillis;
		}
	}

	if (result->taskToSwitch.func != nullptr) 
	{
		switchTo(result->taskToSwitch);
	}
}