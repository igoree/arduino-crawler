#ifndef Buzzer_H
#define Buzzer_H

#include "stdint.h"

class Buzzer
{
private:
	uint8_t _pin;
public:
	Buzzer(uint8_t pin);

	void tone(float frequency, uint32_t duration);
	void singleTone(float noteFrequency, uint32_t noteDuration, uint32_t silentDuration);
	void toneTransition(float initFrequency, float finalFrequency, float changeRatio, uint32_t noteDuration, uint32_t silentDuration);
	void noTone();
};
#endif
