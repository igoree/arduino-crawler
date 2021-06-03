#pragma once

#include "stdint.h"

class Buzzer
{
private:
	const uint8_t _pin;
public:
	Buzzer(uint8_t pin);

	void tone(float frequency, uint32_t duration) const;
	void noTone() const;
};
