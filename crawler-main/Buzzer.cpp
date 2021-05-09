#include "Buzzer.h"
#include "Arduino.h"
#include <avr/wdt.h>

Buzzer::Buzzer(uint8_t pin)
	: _pin(pin)
{
}

void Buzzer::tone(float frequency, uint32_t duration) const
{
	uint32_t period = 1000000ul / frequency;
	uint32_t pulse = period / 2;

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

void Buzzer::singleTone(float noteFrequency, uint32_t noteDuration, uint32_t silenceDuration) const
{
	tone(noteFrequency, noteDuration);

	if (silenceDuration > 0)
	{
		delay(silenceDuration);
	}
}

void Buzzer::toneTransition(float initFrequency, float finalFrequency, float changeRatio, uint32_t noteDuration, uint32_t silenceDuration) const
{
	//Examples:
	//  bendTones (880, 2093, 1.02, 18, 1);
	//  bendTones (NOTE_A5, NOTE_C7, 1.02, 18, 0);

	if (initFrequency < finalFrequency)
	{
		for (int i = initFrequency; i < finalFrequency; i = i * changeRatio)
		{
			singleTone(i, noteDuration, silenceDuration);
		}
	}
	else 
	{
		for (int i = initFrequency; i > finalFrequency; i = i / changeRatio)
		{
			singleTone(i, noteDuration, silenceDuration);
		}
	}
}

void Buzzer::noTone() const
{
	pinMode(_pin, OUTPUT);
	digitalWrite(_pin, LOW);
}