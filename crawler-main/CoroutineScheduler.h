#ifndef _COROUTINESCHEDULER_h
#define _COROUTINESCHEDULER_h

#include "stdint.h"
#include "WString.h"

class CoroutineExecutionContext;

typedef uint8_t CoroutineStep;

typedef CoroutineStep(*AsyncFuncPointer)(const CoroutineExecutionContext* context);

struct CoroutineTask
{
	CoroutineTask();
	CoroutineTask(AsyncFuncPointer func, void* data = nullptr);

	AsyncFuncPointer func;
	void* data;
};

struct CoroutineTaskState;

class Coroutine
{
private:
	const char* const _name;
	const uint8_t _stackSize;
	uint8_t _currentTaskIndex;
	CoroutineTaskState* _stack;
public:
	Coroutine(const char* name, uint8_t stackSize);
	~Coroutine();

	void start(CoroutineTask task);
	void switchTo(CoroutineTask task);

	void continueExecution();
};

class CoroutineExecutionContext
{
private:
	Coroutine* const _coroutine;
	CoroutineTaskState* const _funcState;
public:
	CoroutineExecutionContext(Coroutine* coroutine, CoroutineTaskState* funcState);
	~CoroutineExecutionContext();

	CoroutineStep getCurrentStep() const;
	void* getData() const;

	CoroutineStep repeat() const;
	CoroutineStep delayThenRepeat(unsigned long delayMs) const;
	CoroutineStep executeThenRepeat(CoroutineTask task) const;

	CoroutineStep next() const;
	CoroutineStep delayThenNext(unsigned long delayMs) const;
	CoroutineStep executeThenNext(CoroutineTask task) const;

	CoroutineStep goTo(CoroutineStep step) const;
	CoroutineStep delayThenGoTo(unsigned long delayMs, CoroutineStep step) const;
	CoroutineStep executeThenGoTo(CoroutineTask task, CoroutineStep step) const;

	CoroutineStep complete() const;
	CoroutineStep completeThenExecute(CoroutineTask task) const;
};

template<uint8_t MaxCoroutines> class CoroutineScheduler
{
private:
	Coroutine* const _coroutines[MaxCoroutines];
public:
	CoroutineScheduler(Coroutine* const coroutines[MaxCoroutines]);
	~CoroutineScheduler();

	void continueCoroutines() const;
};

#endif