#include "Buzzer.h"
#include "Arduino.h"
#include <avr/wdt.h>

#include "debugLevels.h"
#define DEBUG_LEVEL DEBUG_LEVEL_INFO
#include "debug.h"

Buzzer::Buzzer(uint8_t pin)
	: _pin(pin)
{
	_pin = pin;
}

void Buzzer::tone(float frequency, uint32_t duration)
{
	uint32_t period = 1000000ul / frequency;
	uint32_t pulse = period / 2;

	DEBUG_INFO("tone: frequency=%lu,duration=%lu,period=%lu", (uint32_t) frequency, duration, period);

	pinMode(_pin, OUTPUT);

	for (uint32_t i = 0ul; i < duration * 1000ul; i += period)
	{
		digitalWrite(_pin, HIGH);
		delayMicroseconds(pulse);
		digitalWrite(_pin, LOW);
		delayMicroseconds(pulse);
		wdt_reset();
	}
}

void Buzzer::singleTone(float noteFrequency, uint32_t noteDuration, uint32_t silentDuration)
{
	tone(noteFrequency, noteDuration);

	if (silentDuration > 0)
	{
		delay(silentDuration);
	}
}

void Buzzer::toneTransition(float initFrequency, float finalFrequency, float changeRatio, uint32_t noteDuration, uint32_t silentDuration) {
	//Examples:
	//  bendTones (880, 2093, 1.02, 18, 1);
	//  bendTones (note_A5, note_C7, 1.02, 18, 0);

	if (initFrequency < finalFrequency)
	{
		for (int i = initFrequency; i < finalFrequency; i = i * changeRatio)
		{
			singleTone(i, noteDuration, silentDuration);
		}
	}
	else 
	{
		for (int i = initFrequency; i > finalFrequency; i = i / changeRatio)
		{
			singleTone(i, noteDuration, silentDuration);
		}
	}
}

void Buzzer::noTone()
{
	pinMode(_pin, OUTPUT);
	digitalWrite(_pin, LOW);
}