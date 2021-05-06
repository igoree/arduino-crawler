#ifndef _COROUTINESCHEDULER_h
#define _COROUTINESCHEDULER_h

#include "stdint.h"
#include "WString.h"

class CoroutineTaskContext;
struct CoroutineTaskResult;

typedef CoroutineTaskResult* (*AsyncFuncPointer)(const CoroutineTaskContext* context);

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

class CoroutineTaskContext
{
private:
	CoroutineTaskResult* _result;
public:
	CoroutineTaskContext(const CoroutineTaskState* taskState, CoroutineTaskResult* defaultResult);
	~CoroutineTaskContext();

	uint8_t const step;
	void* const data;

	CoroutineTaskResult* repeat() const;
	CoroutineTaskResult* delayThenRepeat(unsigned long delayMs) const;
	CoroutineTaskResult* executeThenRepeat(CoroutineTask task) const;

	CoroutineTaskResult* next() const;
	CoroutineTaskResult* delayThenNext(unsigned long delayMs) const;
	CoroutineTaskResult* executeThenNext(CoroutineTask task) const;

	CoroutineTaskResult* goTo(uint8_t step) const;
	CoroutineTaskResult* delayThenGoTo(unsigned long delayMs, uint8_t step) const;
	CoroutineTaskResult* executeThenGoTo(CoroutineTask task, uint8_t step) const;

	CoroutineTaskResult* complete() const;
	CoroutineTaskResult* completeThenExecute(CoroutineTask task) const;
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