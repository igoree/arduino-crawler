#ifndef _SOUNDPLAYER_h
#define _SOUNDPLAYER_h

#include "stdint.h"
#include "Coroutine.h"
#include "Buzzer.h"

enum class Sound : uint8_t
{
	Up = 0,
	Down = 1,
	ButtonPushed = 2,
	Mode1 = 3,
	Mode2 = 4,
	TurnOn = 5,
	Surprise = 6,
	OhOoh = 7,
	OhOoh2 = 8,
	Cuddly = 9,
	Sleeping = 10,
	Happy = 11,
	SuperHappy = 12,
	HappyShort = 13,
	Sad = 14,
	Confused = 15,
	Fart1 = 16,
	Fart2 = 17,
	Fart3 = 18,
	Didi = 19,
	HappyBirthday = 20,
	Police = 21
};

struct SoundState;

class SoundPlayer 
{
private:
	Coroutine* const _soundCoroutine;
	SoundState* const _currentSoundState;
public:
	SoundPlayer(const Buzzer* buzzer, Coroutine* soundCoroutine);
	~SoundPlayer();

	void play(Sound sound);
	void repeat(Sound sound);
	void stop();
};

#endif
