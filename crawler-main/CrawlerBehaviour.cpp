#include "CrawlerBehaviour.h"
#include "Crawler.h"
#include "Storage.h"

#include "DebugLevels.h"
//#define DEBUG_LEVEL DEBUG_LEVEL_INFO
#include "DebugOutput.h"

void CrawlerBehaviourStrategy::init(SoundPlayer* soundPlayer, LightController* lightController, CrawlerStatus status)
{
	onStatusChanged(soundPlayer, lightController, status);
	soundPlayer->stop();
}

void CrawlerBehaviourStrategy::onStatusChanged(SoundPlayer* soundPlayer, LightController* lightController, CrawlerStatus status)
{
	switch (status) {
	case CrawlerStatus::RunForward:
		lightController->repeat(LightEffect::FrontLights);
		break;

	case CrawlerStatus::TurnLeft:
	case CrawlerStatus::TurnLeftRotate:
	case CrawlerStatus::TurnLeftBackward:
		lightController->repeat(LightEffect::LeftTurnSignal);
		break;

	case CrawlerStatus::TurnRight:
	case CrawlerStatus::TurnRightRotate:
	case CrawlerStatus::TurnRightBackward:
		lightController->repeat(LightEffect::RightTurnSignal);
		break;

	case CrawlerStatus::RunBackward:
		lightController->repeat(LightEffect::RearLights);
		break;

	case CrawlerStatus::Stop:
		lightController->lightOff();
		break;

	default:
		break;
	}
}

void CrawlerBehaviourStrategy::onSpeedChanged(SoundPlayer* soundPlayer, LightController* lightController, uint8_t newSpeed, uint8_t oldSpeed)
{
	if (newSpeed == oldSpeed)
		return;

	if (newSpeed > oldSpeed)
	{
		soundPlayer->play(Sound::Up);
	}
	else 
	{
		soundPlayer->play(Sound::Down);
	}

	lightController->show(LightEffect::SpeedChange, newSpeed);
}

void PoliceCrawlerBehaviourStrategy::init(SoundPlayer* soundPlayer, LightController* lightController, CrawlerStatus status)
{
	DEBUG_INFO("police init");

	soundPlayer->repeat(Sound::Police);
	lightController->repeat(LightEffect::Police);
}

void PoliceCrawlerBehaviourStrategy::onStatusChanged(SoundPlayer* soundPlayer, LightController* lightController, CrawlerStatus status)
{
}

CrawlerBehaviour::CrawlerBehaviour(SoundPlayer* soundPlayer, LightController* lightController)
	: _soundPlayer(soundPlayer), _lightController(lightController), _defaultStrategy(), _policeStrategy()
{
	getCurrentStrategy()->init(_soundPlayer, _lightController, CrawlerStatus::Stop);
}

CrawlerBehaviour::~CrawlerBehaviour()
{
}

CrawlerBehaviourStrategy* CrawlerBehaviour::getCurrentStrategy()
{
	switch ((CrawlerBehaviourKind)storage.getCrawlerBehaviourKind())
	{
	case CrawlerBehaviourKind::Police:
		return (CrawlerBehaviourStrategy*) &_policeStrategy;

	default:
		return (CrawlerBehaviourStrategy*) &_defaultStrategy;
	}
}

void CrawlerBehaviour::switchTo(CrawlerBehaviourKind behaviourKind, CrawlerStatus status)
{
	DEBUG_INFO("behaviour=%u", (uint8_t)behaviourKind);

	storage.setCrawlerBehaviourKind((uint8_t)behaviourKind);

	getCurrentStrategy()->init(_soundPlayer, _lightController, status);
}

void CrawlerBehaviour::onStatusChanged(CrawlerStatus status)
{
	getCurrentStrategy()->onStatusChanged(_soundPlayer, _lightController, status);
}

void CrawlerBehaviour::onSpeedChanged(uint8_t newSpeed, uint8_t oldSpeed)
{
	getCurrentStrategy()->onSpeedChanged(_soundPlayer, _lightController, newSpeed, oldSpeed);
}
