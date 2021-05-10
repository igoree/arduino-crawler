#include "Buzzer.h"
#include "NewTone.h"

Buzzer::Buzzer(uint8_t pin)
	: _pin(pin)
{
}

void Buzzer::tone(float frequency, uint32_t duration) const
{
	NewTone(_pin, frequency, duration);
}

void Buzzer::noTone() const
{
	noNewTone(_pin);
}