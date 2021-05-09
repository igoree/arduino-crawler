#ifndef Buzzer_H
#define Buzzer_H

#include "stdint.h"

class Buzzer
{
private:
	uint8_t _pin;
public:
	Buzzer(uint8_t pin);

	void tone(uint16_t frequency, uint32_t duration = 0);
	void _tone(float noteFrequency, long noteDuration, int silentDuration);
	void bendTones(float initFrequency, float finalFrequency, float prop, long noteDuration, int silentDuration);
	void noTone();
};
#endif
