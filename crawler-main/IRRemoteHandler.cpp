#include "IRRemoteHandler.h"
#include "IRRemote.h"
#include "Crawler.h"
#include "Storage.h"

#include "DebugLevels.h"
//#define DEBUG_LEVEL DEBUG_LEVEL_INFO
#include "DebugOutput.h"

#define MIN_DELAY_BETWEEN_COMMAND_REPEAT 500ul

struct IRRemoteHandlerState
{
	IRRemoteHandlerState(IRRemote* irRemote, Crawler* crawler)
		: irRemote(irRemote), crawler(crawler), lastHandledKeyMillis(0), lastHandledKey(IRKeyCode::Unknown)
	{
	}

	IRRemote* const irRemote;
	Crawler* const crawler;
	unsigned long lastHandledKeyMillis;
	IRKeyCode lastHandledKey;
};

enum class CrawlerIRControlMode : uint8_t
{
	ContinuousPressing,
	SinglePress
};

CrawlerIRControlMode getControlMode()
{
	return (CrawlerIRControlMode) storage.getIRRemoteControlMode();
}

void handleIRCommand(IRKeyCode irKeyCode, CrawlerIRControlMode controlMode, IRRemoteHandlerState* state)
{
	auto crawlerStatus = state->crawler->getStatus();
	bool rotateTurnMode = controlMode == CrawlerIRControlMode::ContinuousPressing ||
		crawlerStatus == CrawlerStatus::Stop ||
		crawlerStatus == CrawlerStatus::TurnLeftRotate ||
		crawlerStatus == CrawlerStatus::TurnRightRotate;
	bool backwardTurnMode = crawlerStatus == CrawlerStatus::RunBackward ||
		crawlerStatus == CrawlerStatus::TurnLeftBackward ||
		crawlerStatus == CrawlerStatus::TurnRightBackward;
	switch (irKeyCode) {
	case IRKeyCode::Star:
		state->crawler->speedUp(10);
		break;

	case IRKeyCode::Pound:
		state->crawler->speedDown(10);
		break;

	case IRKeyCode::Up:
		state->crawler->goForward();
		break;

	case IRKeyCode::Down:
		state->crawler->goBack();
		break;

	case IRKeyCode::Ok:
		state->crawler->stop();
		break;

	case IRKeyCode::Left:
		if (rotateTurnMode)
		{
			state->crawler->turnLeftRotate();
		}
		else if (backwardTurnMode)
		{
			state->crawler->turnLeftBackward();
		}
		else
		{
			state->crawler->turnLeft();
		}
		break;

	case IRKeyCode::Right:
		if (rotateTurnMode)
		{
			state->crawler->turnRightRotate();
		}
		else if (backwardTurnMode)
		{
			state->crawler->turnRightBackward();
		}
		else
		{
			state->crawler->turnRight();
		}
		break;

	case IRKeyCode::Button1:
		DEBUG_INFO("IRControlMode=ContinuousPressing");
		storage.setIRRemoteControlMode((uint8_t) CrawlerIRControlMode::ContinuousPressing);
		state->crawler->playSound(Sound::HappyShort);
		state->crawler->showLightEffect(LightEffect::Command);
		break;

	case IRKeyCode::Button2:
		DEBUG_INFO("IRControlMode=SinglePress");
		storage.setIRRemoteControlMode((uint8_t) CrawlerIRControlMode::SinglePress);
		state->crawler->playSound(Sound::SuperHappy);
		state->crawler->showLightEffect(LightEffect::Command);
		break;

	case IRKeyCode::Button3:
		if (state->crawler->isMuted())
		{
			state->crawler->unmute();
		}
		else
		{
			state->crawler->mute();
		}
		state->crawler->playSound(Sound::Command);
		state->crawler->showLightEffect(LightEffect::Command);
		break;

	case IRKeyCode::Button4:
		state->crawler->useBehaviour(CrawlerBehaviourKind::Default);
		state->crawler->playSound(Sound::Command);
		state->crawler->showLightEffect(LightEffect::Command);
		break;

	case IRKeyCode::Button5:
		state->crawler->useBehaviour(CrawlerBehaviourKind::Police);
		state->crawler->playSound(Sound::Command);
		state->crawler->showLightEffect(LightEffect::Command);
		break;

	case IRKeyCode::Button0:
		state->crawler->playSound(Sound::HappyBirthday);
		state->crawler->showLightEffect(LightEffect::Command);
		break;
	}
}

CoroutineTaskResult* handleIRRemoteAsync(const CoroutineTaskContext* context)
{
	auto state = (IRRemoteHandlerState*)context->data;
	auto keyCode = (IRKeyCode) state->irRemote->getCode();
	if (keyCode != IRKeyCode::Unknown)
	{
		if (state->lastHandledKey != keyCode || state->lastHandledKeyMillis + MIN_DELAY_BETWEEN_COMMAND_REPEAT < millis())
		{
			DEBUG_INFO("IRKeyCode=%u", (uint8_t)keyCode);

			handleIRCommand(keyCode, getControlMode(), state);

			state->lastHandledKey = keyCode;
			state->lastHandledKeyMillis = millis();
		}

		return context->delayThenGoTo(75, 1u);
	}
	else if (context->step == 1u)
	{
		return context->delayThenNext(75);
	}
	else if (context->step == 2u)
	{
		state->lastHandledKey = IRKeyCode::Unknown;
		state->lastHandledKeyMillis = 0ul;

		if (getControlMode() == CrawlerIRControlMode::ContinuousPressing && state->crawler->getStatus() != CrawlerStatus::Stop)
		{
			state->crawler->stop();
		}
	}

	return context->goTo(0u);
}

IRRemoteHandler::IRRemoteHandler(IRRemote* irRemote, Crawler* crawler, Coroutine* irRemoteCoroutine)
	: _coroutine(irRemoteCoroutine), _state(new IRRemoteHandlerState(irRemote, crawler))
{
	_coroutine->start(CoroutineTask(&handleIRRemoteAsync, _state));
}

IRRemoteHandler::~IRRemoteHandler()
{
	_coroutine->stop();
	delete _state;
}