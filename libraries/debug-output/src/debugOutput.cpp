#include "Arduino.h"
#include "stdio.h"
#include "debugOutput.h"

extern unsigned int __bss_end;
extern unsigned int __heap_start;
extern void* __brkval;

int debug_freeMemory()
{
	int free_memory;

	if ((int)__brkval == 0)
		free_memory = ((int)&free_memory) - ((int)&__bss_end);
	else
		free_memory = ((int)&free_memory) - ((int)__brkval);

	return free_memory;
}

void debug_printf(const __FlashStringHelper* fmt, ...)
{
	va_list v_arg;
	char string[DEBUG_BUFFER_SIZE];
	va_start(v_arg, fmt);
	vsprintf_P(string, reinterpret_cast<PGM_P>(fmt), v_arg);
	Serial.println(string);
}