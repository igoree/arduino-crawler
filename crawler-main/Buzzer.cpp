#include "Buzzer.h"
#include "Arduino.h"
#include <avr/wdt.h>

Buzzer::Buzzer(uint8_t pin)
	: _pin(pin)
{
	_pin = pin;
}

void Buzzer::tone(uint16_t frequency, uint32_t duration)
{
	int period = 1000000L / frequency;
	int pulse = period / 2;
	pinMode(_pin, OUTPUT);
	for (long i = 0; i < duration * 1000L; i += period)
	{
		digitalWrite(_pin, HIGH);
		delayMicroseconds(pulse);
		digitalWrite(_pin, LOW);
		delayMicroseconds(pulse);
		wdt_reset();
	}
}

void Buzzer::_tone(float noteFrequency, long noteDuration, int silentDuration)
{
	if (silentDuration == 0)
	{
		silentDuration = 1;
	}

	tone(noteFrequency, noteDuration);
	delay(noteDuration);
	noTone();
	delay(silentDuration);
}

void Buzzer::bendTones(float initFrequency, float finalFrequency, float prop, long noteDuration, int silentDuration) {
	//Examples:
	//  bendTones (880, 2093, 1.02, 18, 1);
	//  bendTones (note_A5, note_C7, 1.02, 18, 0);

	if (silentDuration == 0)
	{
		silentDuration = 1;
	}

	if (initFrequency < finalFrequency)
	{
		for (int i = initFrequency; i < finalFrequency; i = i * prop)
		{
			_tone(i, noteDuration, silentDuration);
		}
	}
	else 
	{
		for (int i = initFrequency; i > finalFrequency; i = i / prop)
		{
			_tone(i, noteDuration, silentDuration);
		}
	}
}

void Buzzer::noTone()
{
	pinMode(_pin, OUTPUT);
	digitalWrite(_pin, LOW);
}