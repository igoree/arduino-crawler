#include "SoundPlayer.h"
#include "Buzzer.h"
#include "Storage.h"
#include "avr/pgmspace.h"

#include "DebugLevels.h"
//#define DEBUG_LEVEL DEBUG_LEVEL_INFO
#include "DebugOutput.h"

// Reference:  This list was adapted from the table located here:
//    http://www.phy.mtu.edu/~suits/notefreqs.html
#define  NOTE_C0  16.35  //C0
#define  NOTE_Db0 17.32  //C#0/Db0
#define  NOTE_D0  18.35  //D0
#define  NOTE_Eb0 19.45 //D#0/Eb0
#define  NOTE_E0  20.6  //E0
#define  NOTE_F0  21.83  //F0
#define  NOTE_Gb0 23.12  //F#0/Gb0
#define  NOTE_G0  24.5  //G0
#define  NOTE_Ab0 25.96  //G#0/Ab0
#define  NOTE_A0  27.5  //A0
#define  NOTE_Bb0 29.14  //A#0/Bb0
#define  NOTE_B0  30.87  //B0
#define  NOTE_C1  32.7  //C1
#define  NOTE_Db1 34.65  //C#1/Db1
#define  NOTE_D1  36.71  //D1
#define  NOTE_Eb1 38.89  //D#1/Eb1
#define  NOTE_E1  41.2  //E1
#define  NOTE_F1  43.65  //F1
#define  NOTE_Gb1 46.25  //F#1/Gb1
#define  NOTE_G1  49 //G1
#define  NOTE_Ab1 51.91  //G#1/Ab1
#define  NOTE_A1  55  //A1
#define  NOTE_Bb1 58.27  //A#1/Bb1
#define  NOTE_B1  61.74  //B1
#define  NOTE_C2  65.41  //C2 (Middle C)
#define  NOTE_Db2 69.3  //C#2/Db2
#define  NOTE_D2  73.42  //D2
#define  NOTE_Eb2 77.78  //D#2/Eb2
#define  NOTE_E2  82.41  //E2
#define  NOTE_F2  87.31  //F2
#define  NOTE_Gb2 92.5  //F#2/Gb2
#define  NOTE_G2  98  //G2
#define  NOTE_Ab2 103.83  //G#2/Ab2
#define  NOTE_A2  110  //A2
#define  NOTE_Bb2 116.54  //A#2/Bb2
#define  NOTE_B2  123.47  //B2
#define  NOTE_C3  130.81  //C3
#define  NOTE_Db3 138.59  //C#3/Db3
#define  NOTE_D3  146.83  //D3
#define  NOTE_Eb3 155.56  //D#3/Eb3
#define  NOTE_E3  164.81  //E3
#define  NOTE_F3  174.61  //F3
#define  NOTE_Gb3 185  //F#3/Gb3
#define  NOTE_G3  196  //G3
#define  NOTE_Ab3 207.65  //G#3/Ab3
#define  NOTE_A3  220  //A3
#define  NOTE_Bb3 233.08  //A#3/Bb3
#define  NOTE_B3  246.94  //B3
#define  NOTE_C4  261.63  //C4
#define  NOTE_Db4 277.18  //C#4/Db4
#define  NOTE_D4  293.66  //D4
#define  NOTE_Eb4 311.13  //D#4/Eb4
#define  NOTE_E4  329.63  //E4
#define  NOTE_F4  349.23  //F4
#define  NOTE_Gb4 369.99  //F#4/Gb4
#define  NOTE_G4  392  //G4
#define  NOTE_Ab4 415.3  //G#4/Ab4
#define  NOTE_A4  440  //A4
#define  NOTE_Bb4 466.16  //A#4/Bb4
#define  NOTE_B4  493.88  //B4
#define  NOTE_C5  523.25  //C5
#define  NOTE_Db5 554.37  //C#5/Db5
#define  NOTE_D5  587.33  //D5
#define  NOTE_Eb5 622.25  //D#5/Eb5
#define  NOTE_E5  659.26  //E5
#define  NOTE_F5  698.46  //F5
#define  NOTE_Gb5 739.99  //F#5/Gb5
#define  NOTE_G5  783.99  //G5
#define  NOTE_Ab5 830.61  //G#5/Ab5
#define  NOTE_A5  880  //A5
#define  NOTE_Bb5 932.33  //A#5/Bb5
#define  NOTE_B5  987.77  //B5
#define  NOTE_C6  1046.5  //C6
#define  NOTE_Db6 1108.73  //C#6/Db6
#define  NOTE_D6  1174.66  //D6
#define  NOTE_Eb6 1244.51  //D#6/Eb6
#define  NOTE_E6  1318.51  //E6
#define  NOTE_F6  1396.91  //F6
#define  NOTE_Gb6 1479.98  //F#6/Gb6
#define  NOTE_G6  1567.98  //G6
#define  NOTE_Ab6 1661.22  //G#6/Ab6
#define  NOTE_A6  1760  //A6
#define  NOTE_Bb6 1864.66  //A#6/Bb6
#define  NOTE_B6  1975.53  //B6
#define  NOTE_C7  2093  //C7
#define  NOTE_Db7 2217.46  //C#7/Db7
#define  NOTE_D7  2349.32  //D7
#define  NOTE_Eb7 2489.02  //D#7/Eb7
#define  NOTE_E7  2637.02  //E7
#define  NOTE_F7  2793.83  //F7
#define  NOTE_Gb7 2959.96  //F#7/Gb7
#define  NOTE_G7  3135.96  //G7
#define  NOTE_Ab7 3322.44  //G#7/Ab7
#define  NOTE_A7  3520  //A7
#define  NOTE_Bb7 3729.31  //A#7/Bb7
#define  NOTE_B7  3951.07  //B7
#define  NOTE_C8  4186.01  //C8
#define  NOTE_Db8 4434.92  //C#8/Db8
#define  NOTE_D8  4698.64  //D8
#define  NOTE_Eb8 4978.03  //D#8/Eb8

struct SoundNoteState
{
	SoundNoteState()
		: frequency(0), targetFrequency(0), frequencyChangeRatio(0), duration(0), silenceDuration(0)
	{
	}

	float frequency;
	float targetFrequency;
	float frequencyChangeRatio;
	uint32_t duration;
	uint32_t silenceDuration;
};

struct SoundState
{
	SoundState(const Buzzer* buzzer)
		: buzzer(buzzer), repeatedSoundFunc(), currentNote()
	{
	}

	const Buzzer* const buzzer;
	AsyncFuncPointer repeatedSoundFunc;
	SoundNoteState currentNote;
};

SoundPlayer::SoundPlayer(const Buzzer* buzzer, Coroutine* soundCoroutine)
	: _soundCoroutine(soundCoroutine), _currentSoundState(new SoundState(buzzer))
{
}

SoundPlayer::~SoundPlayer()
{
	delete _currentSoundState;
}

CoroutineTaskResult* playSeparatedNoteAsync(const CoroutineTaskContext* context)
{
	auto state = (SoundState*)context->data;

	switch (context->step)
	{
	case 0:
		if (storage.getSoundEnabled()) 
		{
			state->buzzer->tone(state->currentNote.frequency, state->currentNote.duration);
		}
		return context->delayThenNext(state->currentNote.duration);

	case 1:
		state->buzzer->noTone();
		if (state->currentNote.silenceDuration == 0)
		{
			return context->next();
		}
		return context->delayThenNext(state->currentNote.silenceDuration);

	default:
		return context->end();
	}
}

CoroutineTaskResult* playNoteTransitionAsync(const CoroutineTaskContext* context)
{
	auto state = (SoundState*)context->data;

	switch (context->step)
	{
	case 0:
		return context->executeThenNext(CoroutineTask(&playSeparatedNoteAsync, state));

	case 1:
		if (state->currentNote.frequency < state->currentNote.targetFrequency)
		{
			state->currentNote.frequency *= state->currentNote.frequencyChangeRatio;
			if (state->currentNote.frequency > state->currentNote.targetFrequency)
			{
				return context->end();
			}
		}
		else if (state->currentNote.frequency > state->currentNote.targetFrequency)
		{
			state->currentNote.frequency /= state->currentNote.frequencyChangeRatio;
			if (state->currentNote.frequency < state->currentNote.targetFrequency)
			{
				return context->end();
			}
		}
		else
		{
			return context->end();
		}

		return context->executeThenRepeat(CoroutineTask(&playSeparatedNoteAsync, state));

	default:
		return context->end();
	}
}

void prepareSeparatedNote(SoundState* state, float frequency, uint32_t duration, uint32_t silenceDuration)
{
	state->currentNote.frequency = frequency;
	state->currentNote.duration = duration;
	state->currentNote.silenceDuration = silenceDuration;
}

void prepareNoteTransition(SoundState* state, float initialFrequency, float targetFrequency, float frequencyChangeRatio, uint32_t noteDuration, uint32_t silenceDuration)
{
	state->currentNote.frequency = initialFrequency;
	state->currentNote.targetFrequency = targetFrequency;
	state->currentNote.frequencyChangeRatio = frequencyChangeRatio;
	state->currentNote.duration = noteDuration;
	state->currentNote.silenceDuration = silenceDuration;
}

CoroutineTaskResult* playSoundUpAsync(const CoroutineTaskContext* context)
{
	auto state = (SoundState*)context->data;

	switch (context->step)
	{
	case 0:
		prepareSeparatedNote(state, NOTE_E5, 50, 60);
		break;

	case 1:
		prepareSeparatedNote(state, NOTE_E6, 55, 60);
		break;

	case 2:
		prepareSeparatedNote(state, NOTE_A6, 60, 50);
		break;

	default:
		return context->end();
	}

	return context->executeThenNext(CoroutineTask(&playSeparatedNoteAsync, state));
}

CoroutineTaskResult* playSoundDownAsync(const CoroutineTaskContext* context)
{
	auto state = (SoundState*)context->data;

	switch (context->step)
	{
	case 0:
		prepareSeparatedNote(state, NOTE_E5, 50, 60);
		break;

	case 1:
		prepareSeparatedNote(state, NOTE_A6, 55, 60);
		break;

	case 2:
		prepareSeparatedNote(state, NOTE_E6, 50, 50);
		break;

	default:
		return context->end();
	}

	return context->executeThenNext(CoroutineTask(&playSeparatedNoteAsync, state));
}

CoroutineTaskResult* playSoundButtonPushedAsync(const CoroutineTaskContext* context)
{
	auto state = (SoundState*)context->data;

	switch (context->step)
	{
	case 0:
		prepareNoteTransition(state, NOTE_E6, NOTE_G6, 1.03, 20, 20);
		return context->executeThenNext(CoroutineTask(&playNoteTransitionAsync, state));

	case 1:
		return context->delayThenNext(30);

	case 2:
		prepareNoteTransition(state, NOTE_E6, NOTE_D7, 1.04, 10, 10);
		return context->executeThenNext(CoroutineTask(&playNoteTransitionAsync, state));

	default:
		return context->end();
	}
}

CoroutineTaskResult* playSoundMode1Async(const CoroutineTaskContext* context)
{
	auto state = (SoundState*)context->data;

	switch (context->step)
	{
	case 0:
		prepareNoteTransition(state, NOTE_E6, NOTE_A6, 1.02, 30, 20);  //1318.51 to 1760
		return context->executeThenNext(CoroutineTask(&playNoteTransitionAsync, state));

	default:
		return context->end();
	}
}

CoroutineTaskResult* playSoundMode2Async(const CoroutineTaskContext* context)
{
	auto state = (SoundState*)context->data;

	switch (context->step)
	{
	case 0:
		prepareNoteTransition(state, NOTE_G6, NOTE_D7, 1.03, 30, 20);  //1318.51 to 1760
		return context->executeThenNext(CoroutineTask(&playNoteTransitionAsync, state));

	default:
		return context->end();
	}
}

CoroutineTaskResult* playSoundTurnOnAsync(const CoroutineTaskContext* context)
{
	auto state = (SoundState*)context->data;

	switch (context->step)
	{
	case 0:
		prepareSeparatedNote(state, NOTE_E6, 50, 150); //D6
		break;

	case 1:
		prepareSeparatedNote(state, NOTE_G6, 50, 130); //E6
		break;

	case 2:
		prepareSeparatedNote(state, NOTE_D7, 300, 0); //G6
		break;

	default:
		return context->end();
	}

	return context->executeThenNext(CoroutineTask(&playSeparatedNoteAsync, state));
}

CoroutineTaskResult* playSoundSurpriseAsync(const CoroutineTaskContext* context)
{
	auto state = (SoundState*)context->data;

	switch (context->step)
	{
	case 0:
		prepareNoteTransition(state, 800, 2150, 1.02, 10, 10);
		break;

	case 1:
		prepareNoteTransition(state, 2149, 800, 1.03, 7, 8);
		break;

	default:
		return context->end();
	}

	return context->executeThenNext(CoroutineTask(&playNoteTransitionAsync, state));
}

CoroutineTaskResult* playSoundOhOohAsync(const CoroutineTaskContext* context)
{
	auto state = (SoundState*)context->data;

	switch (context->step)
	{
	case 0:
		prepareNoteTransition(state, 880, 2000, 1.04, 8, 10); //A5 = 880
		return context->executeThenNext(CoroutineTask(&playNoteTransitionAsync, state));

	case 1:
		return context->delayThenNext(200);

	default:
		if (context->step <= 23)
		{
			prepareSeparatedNote(state, NOTE_B5, 5, 15);
			return context->executeThenNext(CoroutineTask(&playSeparatedNoteAsync, state));
		}
		break;
	}

	return context->end();
}

CoroutineTaskResult* playSoundOhOoh2Async(const CoroutineTaskContext* context)
{
	auto state = (SoundState*)context->data;

	switch (context->step)
	{
	case 0:
		prepareNoteTransition(state, 1880, 3000, 1.03, 8, 10); //A5 = 880
		return context->executeThenNext(CoroutineTask(&playNoteTransitionAsync, state));

	case 1:
		return context->delayThenNext(200);

	default:
		if (context->step <= 18)
		{
			prepareSeparatedNote(state, NOTE_C6, 10, 20);
			return context->executeThenNext(CoroutineTask(&playSeparatedNoteAsync, state));
		}
		break;
	}

	return context->end();
}

CoroutineTaskResult* playSoundCuddlyAsync(const CoroutineTaskContext* context)
{
	auto state = (SoundState*)context->data;

	switch (context->step)
	{
	case 0:
		prepareNoteTransition(state, 700, 900, 1.03, 16, 20);
		break;

	case 1:
		prepareNoteTransition(state, 899, 650, 1.01, 18, 25);
		break;

	default:
		return context->end();
	}

	return context->executeThenNext(CoroutineTask(&playNoteTransitionAsync, state));
}

CoroutineTaskResult* playSoundSleepingAsync(const CoroutineTaskContext* context)
{
	auto state = (SoundState*)context->data;

	switch (context->step)
	{
	case 0:
		prepareNoteTransition(state, 100, 500, 1.04, 10, 20);
		return context->executeThenNext(CoroutineTask(&playNoteTransitionAsync, state));

	case 1:
		return context->delayThenNext(500);

	case 2:
		prepareNoteTransition(state, 400, 100, 1.04, 10, 10);
		return context->executeThenNext(CoroutineTask(&playNoteTransitionAsync, state));

	default:
		return context->end();
	}
}

CoroutineTaskResult* playSoundHappyAsync(const CoroutineTaskContext* context)
{
	auto state = (SoundState*)context->data;

	switch (context->step)
	{
	case 0:
		prepareNoteTransition(state, 1500, 2500, 1.05, 20, 10);
		break;

	case 1:
		prepareNoteTransition(state, 2499, 1500, 1.05, 25, 10);
		break;

	default:
		return context->end();
	}

	return context->executeThenNext(CoroutineTask(&playNoteTransitionAsync, state));
}

CoroutineTaskResult* playSoundSuperHappyAsync(const CoroutineTaskContext* context)
{
	auto state = (SoundState*)context->data;

	switch (context->step)
	{
	case 0:
		prepareNoteTransition(state, 2000, 6000, 1.05, 8, 10);
		return context->executeThenNext(CoroutineTask(&playNoteTransitionAsync, state));

	case 1:
		return context->delayThenNext(50);

	case 2:
		prepareNoteTransition(state, 5999, 2000, 1.05, 13, 15);
		return context->executeThenNext(CoroutineTask(&playNoteTransitionAsync, state));

	default:
		return context->end();
	}
}

CoroutineTaskResult* playSoundHappyShortAsync(const CoroutineTaskContext* context)
{
	auto state = (SoundState*)context->data;

	switch (context->step)
	{
	case 0:
		prepareNoteTransition(state, 1500, 2000, 1.05, 15, 23);
		return context->executeThenNext(CoroutineTask(&playNoteTransitionAsync, state));

	case 1:
		return context->delayThenNext(100);

	case 2:
		prepareNoteTransition(state, 1900, 2500, 1.05, 10, 18);
		return context->executeThenNext(CoroutineTask(&playNoteTransitionAsync, state));
	default:
		return context->end();
	}
}

CoroutineTaskResult* playSoundSadAsync(const CoroutineTaskContext* context)
{
	auto state = (SoundState*)context->data;

	switch (context->step)
	{
	case 0:
		prepareNoteTransition(state, 880, 669, 1.02, 20, 150);
		return context->executeThenNext(CoroutineTask(&playNoteTransitionAsync, state));

	default:
		return context->end();
	}
}

CoroutineTaskResult* playSoundConfusedAsync(const CoroutineTaskContext* context)
{
	auto state = (SoundState*)context->data;

	switch (context->step)
	{
	case 0:
		prepareNoteTransition(state, 1000, 1700, 1.03, 8, 2);
		break;

	case 1:
		prepareNoteTransition(state, 1699, 500, 1.04, 8, 3);
		break;

	case 2:
		prepareNoteTransition(state, 1000, 1700, 1.05, 9, 10);
		break;

	default:
		return context->end();
	}

	return context->executeThenNext(CoroutineTask(&playNoteTransitionAsync, state));
}

CoroutineTaskResult* playSoundFart1Async(const CoroutineTaskContext* context)
{
	auto state = (SoundState*)context->data;

	switch (context->step)
	{
	case 0:
		prepareNoteTransition(state, 1600, 3000, 1.02, 2, 15);
		return context->executeThenNext(CoroutineTask(&playNoteTransitionAsync, state));
	default:
		return context->end();
	}
}

CoroutineTaskResult* playSoundFart2Async(const CoroutineTaskContext* context)
{
	auto state = (SoundState*)context->data;

	switch (context->step)
	{
	case 0:
		prepareNoteTransition(state, 2000, 6000, 1.02, 2, 20);
		return context->executeThenNext(CoroutineTask(&playNoteTransitionAsync, state));
	default:
		return context->end();
	}
}

CoroutineTaskResult* playSoundFart3Async(const CoroutineTaskContext* context)
{
	auto state = (SoundState*)context->data;

	switch (context->step)
	{
	case 0:
		prepareNoteTransition(state, 1600, 4000, 1.02, 2, 20);
		break;

	case 1:
		prepareNoteTransition(state, 4000, 3000, 1.02, 2, 20);
		break;

	default:
		return context->end();
	}

	return context->executeThenNext(CoroutineTask(&playNoteTransitionAsync, state));
}

CoroutineTaskResult* playSoundCommandAsync(const CoroutineTaskContext* context)
{
	auto state = (SoundState*)context->data;

	switch (context->step)
	{
	case 0:
		prepareSeparatedNote(state, NOTE_C7, 50, 100);
		return context->executeThenNext(CoroutineTask(&playSeparatedNoteAsync, state));

	case 1:
		return context->delayThenNext(110);

	case 2:
		prepareSeparatedNote(state, NOTE_C6, 50, 100);
		return context->executeThenNext(CoroutineTask(&playSeparatedNoteAsync, state));

	default:
		return context->end();
	}
}

CoroutineTaskResult* playSoundHappyBirthdayAsync(const CoroutineTaskContext* context)
{
	const static float PROGMEM notes[] =
	{
		NOTE_G4, NOTE_G4, NOTE_A4, NOTE_G4, NOTE_C5, NOTE_B4, NOTE_G4, NOTE_G4,
		NOTE_A4, NOTE_G4, NOTE_D5, NOTE_C5, NOTE_G4, NOTE_G4, NOTE_G5, NOTE_E5,
		NOTE_C5, NOTE_B4, NOTE_A4, NOTE_F5, NOTE_F5, NOTE_E5, NOTE_C5, NOTE_D5, NOTE_C5
	};

	const static uint8_t PROGMEM timings[] =
	{
	  1, 1, 2, 2, 2, 4, 1, 1,
	  2, 2, 2, 4, 1, 1, 2, 2,
	  2, 2, 2, 1, 1, 2, 2, 2, 4
	};

	auto state = (SoundState*)context->data;
	auto noteCount = sizeof(notes) / sizeof(notes[0]);
	if (context->step >= noteCount)
	{
		return context->end();
	}

	prepareSeparatedNote(state, pgm_read_float(&notes[context->step]), pgm_read_byte(&timings[context->step]) * 374u, 0);
	return context->executeThenNext(CoroutineTask(&playSeparatedNoteAsync, state));
}

CoroutineTaskResult* playSoundPoliceAsync(const CoroutineTaskContext* context)
{
	auto state = (SoundState*)context->data;

	switch (context->step)
	{
	case 0:
		prepareNoteTransition(state, 400, 1350, 1.04, 10, 0);
		return context->executeThenNext(CoroutineTask(&playNoteTransitionAsync, state));

	case 1:
		prepareNoteTransition(state, 1350, 400, 1.04, 10, 0);
		return context->executeThenNext(CoroutineTask(&playNoteTransitionAsync, state));

	default:
		return context->end();
	}
}

CoroutineTaskResult* repeatSoundAsync(const CoroutineTaskContext* context)
{
	auto state = (SoundState*)context->data;
	if (state->repeatedSoundFunc == nullptr)
		return context->end();

	return context->executeThenRepeat(CoroutineTask(state->repeatedSoundFunc, state));
}

AsyncFuncPointer getSoundFunc(Sound sound) 
{
	switch (sound)
	{
	case Sound::Up:
		return &playSoundUpAsync;

	case Sound::Down:
		return &playSoundDownAsync;

	case Sound::ButtonPushed:
		return &playSoundButtonPushedAsync;

	case Sound::Mode1:
		return &playSoundMode1Async;

	case Sound::Mode2:
		return &playSoundMode2Async;

	case Sound::TurnOn:
		return &playSoundTurnOnAsync;

	case Sound::Surprise:
		return &playSoundSurpriseAsync;

	case Sound::OhOoh:
		return &playSoundOhOohAsync;

	case Sound::OhOoh2:
		return &playSoundOhOoh2Async;

	case Sound::Cuddly:
		return &playSoundCuddlyAsync;

	case Sound::Sleeping:
		return &playSoundSleepingAsync;

	case Sound::Happy:
		return &playSoundHappyAsync;

	case Sound::SuperHappy:
		return &playSoundSuperHappyAsync;

	case Sound::HappyShort:
		return &playSoundHappyShortAsync;

	case Sound::Sad:
		return &playSoundSadAsync;

	case Sound::Confused:
		return &playSoundConfusedAsync;

	case Sound::Fart1:
		return &playSoundFart1Async;

	case Sound::Fart2:
		return &playSoundFart2Async;

	case Sound::Fart3:
		return &playSoundFart3Async;

	case Sound::Command:
		return &playSoundCommandAsync;

	case Sound::HappyBirthday:
		return &playSoundHappyBirthdayAsync;

	case Sound::Police:
		return &playSoundPoliceAsync;

	default:
		DEBUG_ERR("unsupported sound %u", (uint8_t)sound);
		return nullptr;
	}
}

void SoundPlayer::play(Sound sound)
{
	auto soundFunc = getSoundFunc(sound);
	if (soundFunc == nullptr)
		return;

	if (_currentSoundState->repeatedSoundFunc != nullptr) 
	{
		_soundCoroutine->start(CoroutineTask(&repeatSoundAsync, _currentSoundState));
		_soundCoroutine->switchTo(CoroutineTask(soundFunc, _currentSoundState));
	}
	else 
	{
		_soundCoroutine->start(CoroutineTask(soundFunc, _currentSoundState));
	}
}

void SoundPlayer::repeat(Sound sound)
{
	auto soundFunc = getSoundFunc(sound);
	if (soundFunc == nullptr)
		return;

	_currentSoundState->repeatedSoundFunc = soundFunc;
	_soundCoroutine->start(CoroutineTask(&repeatSoundAsync, _currentSoundState));
}

void SoundPlayer::stop()
{
	_currentSoundState->repeatedSoundFunc = nullptr;
}

void SoundPlayer::mute()
{
	storage.setSoundEnabled(false);
	_currentSoundState->buzzer->noTone();

	DEBUG_INFO("sound muted");
}

void SoundPlayer::unmute()
{
	storage.setSoundEnabled(true);

	DEBUG_INFO("sound unmuted");
}

bool SoundPlayer::isMuted()
{
	return !storage.getSoundEnabled();
}
