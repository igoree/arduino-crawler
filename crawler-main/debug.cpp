#include "Arduino.h"
#include "stdio.h"
#include "debug.h"

void debug_printf(const String& fmt, ...)
{
	va_list v_arg;
	char string[DEBUG_BUFFER_SIZE];
	va_start(v_arg, fmt);
	vsprintf(string, fmt.c_str(), v_arg);
	Serial.println(string);
}