#ifndef _COROUTINESCHEDULER_h
#define _COROUTINESCHEDULER_h

class CoroutineExecutionContext;

typedef void (*AsyncFuncPointer)(CoroutineExecutionContext* context);

class CoroutineExecutionContext {
public:
	uint8_t getStep();
	void nextStep(uint8_t step);
	void nextStep();
	void delay(unsigned long ms);
};

#endif

