#include "Coroutine.h"
#include "Arduino.h"

#include "debugLevels.h"
#define DEBUG_LEVEL DEBUG_LEVEL_ERR
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
	CoroutineTaskResult()
		: CoroutineTaskResult(CoroutineTaskResultKind::Continue)
	{
	}

	CoroutineTaskResult(CoroutineTaskResultKind resultKind)
		: CoroutineTaskResult(0, resultKind)
	{
	}

	CoroutineTaskResult(uint8_t nextStep, CoroutineTaskResultKind resultKind)
		: nextStep(nextStep), resultKind(resultKind), delayMillis(0), taskToSwitch()
	{
	}

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
	: CoroutineTaskState(CoroutineTask())
{
}

CoroutineTaskState::CoroutineTaskState(CoroutineTask task)
	: task(task), step(0), executeAfterMillis(0)
{
}

// Coroutine

Coroutine::Coroutine(const char* name, uint8_t stackSize = 1)
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

	DEBUG_INFO("CR '%s' switch task to %u", _name, _currentTaskIndex);
}

void Coroutine::continueExecution()
{
	if (_currentTaskIndex == UNKNOWN_INDEX)
		return;

	if (_stack[_currentTaskIndex].executeAfterMillis > 0ul && _stack[_currentTaskIndex].executeAfterMillis > millis())
		return;

	CoroutineTaskResult initialResult;
	CoroutineTaskContext context(&_stack[_currentTaskIndex], &initialResult);

	auto result = _stack[_currentTaskIndex].task.func(&context);
	if (result != &initialResult) 
	{
		DEBUG_ERR("CR '%s' task %u invalid result", _name, context.step);
		
		_currentTaskIndex = UNKNOWN_INDEX;

		return;
	}

	if (result->resultKind == CoroutineTaskResultKind::Finish)
	{
		DEBUG_INFO("CR '%s' complete task %u", _name, _currentTaskIndex);

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
		if (result->nextStep != _stack[_currentTaskIndex].step) 
		{
			DEBUG_INFO("CR '%s' go to step %u", _name, result->nextStep);

			_stack[_currentTaskIndex].step = result->nextStep;
		}
		
		if (result->delayMillis > 0) 
		{
			DEBUG_INFO("CR '%s' delay %u", _name, result->delayMillis);

			_stack[_currentTaskIndex].executeAfterMillis = millis() + result->delayMillis;
		}
		else
		{
			_stack[_currentTaskIndex].executeAfterMillis = 0;
		}
	}

	if (result->taskToSwitch.func != nullptr) 
	{
		switchTo(result->taskToSwitch);
	}
}

// CoroutineTaskContext

CoroutineTaskContext::CoroutineTaskContext(const CoroutineTaskState* taskState, CoroutineTaskResult* defaultResult)
	: step(taskState->step), data(taskState->task.data), _result(defaultResult)
{
}

CoroutineTaskContext::~CoroutineTaskContext()
{
}

CoroutineTaskResult* CoroutineTaskContext::repeat() const
{
	(*_result) = CoroutineTaskResult(step, CoroutineTaskResultKind::Continue);

	return _result;
}

CoroutineTaskResult* CoroutineTaskContext::delayThenRepeat(unsigned long delayMillis) const
{
	(*_result) = CoroutineTaskResult(step, CoroutineTaskResultKind::Continue);
	_result->delayMillis = delayMillis;

	return _result;
}

CoroutineTaskResult* CoroutineTaskContext::executeThenRepeat(CoroutineTask task) const
{
	(*_result) = CoroutineTaskResult(step, CoroutineTaskResultKind::Continue);
	_result->taskToSwitch = task;

	return _result;
}

CoroutineTaskResult* CoroutineTaskContext::next() const
{
	(*_result) = CoroutineTaskResult(step + 1, CoroutineTaskResultKind::Continue);

	return _result;
}

CoroutineTaskResult* CoroutineTaskContext::delayThenNext(unsigned long delayMillis) const
{
	(*_result) = CoroutineTaskResult(step + 1, CoroutineTaskResultKind::Continue);
	_result->delayMillis = delayMillis;

	return _result;
}

CoroutineTaskResult* CoroutineTaskContext::executeThenNext(CoroutineTask task) const
{
	(*_result) = CoroutineTaskResult(step + 1, CoroutineTaskResultKind::Continue);
	_result->taskToSwitch = task;

	return _result;
}

CoroutineTaskResult* CoroutineTaskContext::goTo(uint8_t nextStep) const
{
	(*_result) = CoroutineTaskResult(nextStep, CoroutineTaskResultKind::Continue);

	return _result;
}

CoroutineTaskResult* CoroutineTaskContext::delayThenGoTo(unsigned long delayMillis, uint8_t nextStep) const
{
	(*_result) = CoroutineTaskResult(nextStep, CoroutineTaskResultKind::Continue);
	_result->delayMillis = delayMillis;

	return _result;
}

CoroutineTaskResult* CoroutineTaskContext::executeThenGoTo(CoroutineTask task, uint8_t nextStep) const
{
	(*_result) = CoroutineTaskResult(nextStep, CoroutineTaskResultKind::Continue);
	_result->taskToSwitch = task;

	return _result;
}

CoroutineTaskResult* CoroutineTaskContext::end() const
{
	(*_result) = CoroutineTaskResult(CoroutineTaskResultKind::Finish);

	return _result;
}

CoroutineTaskResult* CoroutineTaskContext::endThenExecute(CoroutineTask task) const
{
	(*_result) = CoroutineTaskResult(CoroutineTaskResultKind::Finish);
	_result->taskToSwitch = task;

	return _result;
}