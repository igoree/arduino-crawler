#ifndef _COROUTINESCHEDULER_h
#define _COROUTINESCHEDULER_h

#include "stdint.h"

class CoroutineExecutionContext;

typedef uint8_t CoroutineStep;

typedef CoroutineStep(*AsyncFuncPointer)(CoroutineExecutionContext* context);

struct CoroutineTask 
{
	AsyncFuncPointer func;
	void* data;
};

struct CoroutineFuncState
{
	CoroutineTask task;
	uint8_t step;
	unsigned long executeAfterMillis;
};

class Coroutine
{
private:
	CoroutineFuncState* _stack;
	uint8_t _stackSize;
	uint8_t _currentFuncIndex;
public:
	Coroutine(uint8_t stackSize);
	~Coroutine();

	void start(CoroutineTask task);
	void switchTo(CoroutineTask task);

	void continueExecution();
};

class CoroutineExecutionContext
{
private:
	Coroutine* _coroutine;
	CoroutineFuncState* _funcState;
public:
	CoroutineExecutionContext(Coroutine* coroutine, CoroutineFuncState* funcState);
	~CoroutineExecutionContext();

	CoroutineStep getCurrentStep();
	void* getData();

	CoroutineStep repeat();
	CoroutineStep delayThenRepeat(unsigned long delayMs);
	CoroutineStep executeThenRepeat(AsyncFuncPointer asyncFunc, void* data = NULL, unsigned long startDelayMs = 0);

	CoroutineStep next();
	CoroutineStep delayThenNext(unsigned long delayMs);
	CoroutineStep executeThenNext(AsyncFuncPointer asyncFunc, void* data = NULL, unsigned long startDelayMs = 0);

	CoroutineStep goTo(CoroutineStep step);
	CoroutineStep delayThenGoTo(CoroutineStep step, unsigned long delayMs);
	CoroutineStep executeThenGoTo(CoroutineStep step, AsyncFuncPointer asyncFunc, void* data = NULL, unsigned long startDelayMs = 0);

	CoroutineStep end();
};

class CoroutineScheduler
{
private:
	Coroutine* _coroutines;
	uint8_t _coroutineCount;
public:
	CoroutineScheduler(Coroutine* coroutines, uint8_t coroutineCount);
	~CoroutineScheduler();

	void continueCoroutines();
};

#endif