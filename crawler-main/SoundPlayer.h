#ifndef _SOUNDPLAYER_h
#define _SOUNDPLAYER_h

#include "stdint.h"
#include "Coroutine.h"

enum class Sound : uint8_t
{
	Up,
	Down,
	ButtonPushed,
	Mode1,
	Mode2,
	TurnOn,
	Surprise,
	OhOoh,
	OhOoh2,
	Cuddly,
	Sleeping,
	Happy,
	SuperHappy,
	HappyShort,
	Sad,
	Confused,
	Fart1,
	Fart2,
	Fart3,
	Command,
	HappyBirthday,
	Police
};

class Buzzer;
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

	void mute();
	void unmute();
	bool isMuted();
};

#endif
