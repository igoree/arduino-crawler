#ifndef _COROUTINESCHEDULER_h
#define _COROUTINESCHEDULER_h

#include "stdint.h"

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
	Coroutine(const char* name, uint8_t stackSize = 1);
	~Coroutine();

	void start(CoroutineTask task);
	void switchTo(CoroutineTask task);
	void stop();

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
	CoroutineTaskResult* delayThenRepeat(unsigned long delayMillis) const;
	CoroutineTaskResult* executeThenRepeat(CoroutineTask task) const;

	CoroutineTaskResult* next() const;
	CoroutineTaskResult* delayThenNext(unsigned long delayMillis) const;
	CoroutineTaskResult* executeThenNext(CoroutineTask task) const;

	CoroutineTaskResult* goTo(uint8_t nextStep) const;
	CoroutineTaskResult* delayThenGoTo(unsigned long delayMillis, uint8_t nextStep) const;
	CoroutineTaskResult* executeThenGoTo(CoroutineTask task, uint8_t nextStep) const;

	CoroutineTaskResult* end() const;
	CoroutineTaskResult* endThenExecute(CoroutineTask task) const;
};

#endif
