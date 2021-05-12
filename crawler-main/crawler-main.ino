#include "Coroutine.h"
#include "ProtocolParser.h"
#include "Crawler.h"

#include "DebugLevels.h"
#define DEBUG_LEVEL DEBUG_LEVEL_INFO
#include "DebugOutput.h"

enum class CrawlerIRControlMode : uint8_t
{
	ContinuousPressing,
	SinglePress
};

Coroutine _freeMemoryCoroutine("freeMemory");
Coroutine _irRemoteCoroutine("IRRemote");
Coroutine _lightCoroutine("speedLight", 3);
Coroutine _soundCoroutine("sound", 4);

ProtocolParser _protocol(&Serial);
Crawler _crawler;

int _lastFreeMemory(0);
CrawlerIRControlMode _crawlerIRControlMode(CrawlerIRControlMode::ContinuousPressing);

void setup()
{
	INIT_DEBUG();
	_crawler.init();
	_crawler.initServo();
	_crawler.initLights(&_lightCoroutine);
	_crawler.initSoundPlayer(&_soundCoroutine);
	_crawler.initIRRemote();
	_crawler.setSpeed(50);
	_crawler.setServoBaseAngle(90);
	_crawler.setServoAngle(CrawlerServoKind::Ultrasonic, 90);
	_crawler.initUltrasonic();
	_crawler.initBehaviour();

	DEBUG_INFO("init ok");

	_crawler.playSound(Sound::TurnOn);
	_crawler.showLightEffect(LightEffect::TurnOn);

	_freeMemoryCoroutine.start(CoroutineTask(&monitorFreeMemoryAsync));
	_irRemoteCoroutine.start(CoroutineTask(&handleIRRemoteAsync));
}

CoroutineTaskResult* monitorFreeMemoryAsync(const CoroutineTaskContext* context)
{
#if DEBUG_LEVEL <= DEBUG_LEVEL_INFO
	auto freeMemory = debug_freeMemory();
	if (freeMemory != _lastFreeMemory)
	{
		DEBUG_INFO("free memory: %d", freeMemory);

		_lastFreeMemory = freeMemory;
	}

	return context->delayThenRepeat(500);
#else
	return context->end();
#endif
}

void crawlerHandleIRCommand(IRKeyCode irKeyCode)
{
	auto crawlerStatus = _crawler.getStatus();
	bool rotateTurnMode = _crawlerIRControlMode == CrawlerIRControlMode::ContinuousPressing ||
		crawlerStatus == CrawlerStatus::Stop ||
		crawlerStatus == CrawlerStatus::TurnLeftRotate ||
		crawlerStatus == CrawlerStatus::TurnRightRotate;
	bool backwardTurnMode = crawlerStatus == CrawlerStatus::RunBackward ||
		crawlerStatus == CrawlerStatus::TurnLeftBackward ||
		crawlerStatus == CrawlerStatus::TurnRightBackward;
	switch (irKeyCode) {
	case IRKeyCode::Star:
		_crawler.speedUp(10);
		break;

	case IRKeyCode::Pound:
		_crawler.speedDown(10);
		break;

	case IRKeyCode::Up:
		_crawler.goForward();
		break;

	case IRKeyCode::Down:
		_crawler.goBack();
		break;

	case IRKeyCode::Ok:
		_crawler.stop();
		break;

	case IRKeyCode::Left:
		if (rotateTurnMode)
		{
			_crawler.turnLeftRotate();
		}
		else if (backwardTurnMode)
		{
			_crawler.turnLeftBackward();
		}
		else
		{
			_crawler.turnLeft();
		}
		break;

	case IRKeyCode::Right:
		if (rotateTurnMode)
		{
			_crawler.turnRightRotate();
		}
		else if (backwardTurnMode)
		{
			_crawler.turnRightBackward();
		}
		else
		{
			_crawler.turnRight();
		}
		break;

	case IRKeyCode::Button1:
		DEBUG_INFO("IRControlMode=ContinuousPressing");
		_crawlerIRControlMode = CrawlerIRControlMode::ContinuousPressing;
		_crawler.playSound(Sound::HappyShort);
		break;

	case IRKeyCode::Button2:
		DEBUG_INFO("IRControlMode=SinglePress");
		_crawlerIRControlMode = CrawlerIRControlMode::SinglePress;
		_crawler.playSound(Sound::SuperHappy);
		break;

	case IRKeyCode::Button3:
		if (_crawler.isMuted()) 
		{
			_crawler.unmute();
		}
		else 
		{
			_crawler.mute();
		}
		break;

	case IRKeyCode::Button5:
		_crawler.playSound(Sound::HappyBirthday);
		break;
	}
}

CoroutineTaskResult* handleIRRemoteAsync(const CoroutineTaskContext* context)
{
	auto keyCode = _crawler.getPressedIRKey();
	if (keyCode != IRKeyCode::Unknown)
	{
		DEBUG_INFO("IRKeyCode = %u", (uint8_t)keyCode);

		crawlerHandleIRCommand(keyCode);

		return context->delayThenGoTo(75, 1u);
	}
	else if (context->step == 1u)
	{
		return context->delayThenNext(75);
	}
	else if (context->step == 2u)
	{
		if (_crawlerIRControlMode == CrawlerIRControlMode::ContinuousPressing && _crawler.getStatus() != CrawlerStatus::Stop)
		{
			_crawler.stop();
		}
	}

	return context->goTo(0u);
}

void loop()
{
	_freeMemoryCoroutine.continueExecution();
	_irRemoteCoroutine.continueExecution();
	_lightCoroutine.continueExecution();
	_soundCoroutine.continueExecution();

	/*_protocol.RecevData();
	if (recv_flag = _protocol.ParserPackage()) {
		if (_protocol->GetRobotControlFun() == E_CONTROL_MODE) {
		}
	}*/
}