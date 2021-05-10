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

void Buzzer::noTone() const
{
	pinMode(_pin, OUTPUT);
	digitalWrite(_pin, LOW);
}