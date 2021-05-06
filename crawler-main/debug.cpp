#include "Arduino.h"
#include "stdio.h"
#include "debug.h"

void debug_printf(const __FlashStringHelper* fmt, ...)
{
	va_list v_arg;
	char string[DEBUG_BUFFER_SIZE];
	va_start(v_arg, fmt);
	vsprintf_P(string, reinterpret_cast<PGM_P>(fmt), v_arg);
	Serial.println(string);
}