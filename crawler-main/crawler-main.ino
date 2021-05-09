#include "Coroutine.h"
#include "ProtocolParser.h"
#include "Crawler.h"
#include "IRKeyMap.h"

#include "debugLevels.h"
#define DEBUG_LEVEL DEBUG_LEVEL_INFO
#include "debug.h"

enum class CrawlerIRControlMode : uint8_t
{
	ContinuousPressing,
	SinglePress
};

Coroutine _freeMemoryCoroutine("freeMemory");
Coroutine _irRemoteCoroutine("IRRemote");
Coroutine _speedLightCoroutine("speedLight");
Coroutine _soundCoroutine("sound");

ProtocolParser _protocol(&Serial);
Crawler _crawler;

int _lastFreeMemory(0);
CrawlerIRControlMode _crawlerIRControlMode(CrawlerIRControlMode::ContinuousPressing);

bool _secondSoundGroup = false;

void setup()
{
	INIT_DEBUG();
	_crawler.init(M2, M1);
	_crawler.initServo();
	_crawler.initRgb();
	_crawler.initBuzzer();
	_crawler.initIR();
	_crawler.setSpeed(50);
	_crawler.setServoBaseAngle(90);
	_crawler.setServoAngle(CrawlerServoKind::Ultrasonic, 90);
	_crawler.initUltrasonic();

	DEBUG_INFO("init ok");

	_crawler.playSound(S_mode3);

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

void crawlerShowStatusLight()
{
	switch (_crawler.getStatus()) {
	case CrawlerStatus::RunForward:
		_crawler.setRgbColor(E_RGB_ALL, RGB_WHITE);
		break;

	case CrawlerStatus::TurnLeft:
	case CrawlerStatus::TurnLeftRotate:
	case CrawlerStatus::TurnLeftBackward:
		_crawler.setRgbColor(E_RGB_RIGHT, RGB_ORANGE);
		_crawler.setRgbColor(E_RGB_LEFT, RGB_BLACK);
		break;

	case CrawlerStatus::TurnRight:
	case CrawlerStatus::TurnRightRotate:
	case CrawlerStatus::TurnRightBackward:
		_crawler.setRgbColor(E_RGB_LEFT, RGB_ORANGE);
		_crawler.setRgbColor(E_RGB_RIGHT, RGB_BLACK);
		break;

	case CrawlerStatus::RunBackward:
		_crawler.setRgbColor(E_RGB_ALL, RGB_RED);
		break;

	case CrawlerStatus::Stop:
		_crawler.lightOff();
		break;

	default:
		break;
	}
}

CoroutineTaskResult* crawlerShowSpeedLightAsync(const CoroutineTaskContext* context)
{
	switch (context->step)
	{
	case 0:
		_crawler.setRgbColor(E_RGB_ALL, _crawler.getSpeed() * 2.5);

		return context->delayThenNext(100);

	default:
		crawlerShowStatusLight();

		return context->end();
	}
}

void crawlerShowSpeedLight() 
{
	_speedLightCoroutine.start(CoroutineTask(&crawlerShowSpeedLightAsync));
}

void crawlerSpeedUp(uint8_t delta)
{
	if (_crawler.speedUp(delta))
	{
		_crawler.playSound(S_connection);
	}
	crawlerShowSpeedLight();
}

void crawlerSpeedDown(uint8_t delta)
{
	if (_crawler.speedDown(delta))
	{
		_crawler.playSound(S_disconnection);
	}
	crawlerShowSpeedLight();
}

void playSound(uint8_t soundIndex) {
	if (_secondSoundGroup) {
		soundIndex = soundIndex + 9;
	}
	DEBUG_INFO("Play sound %u", soundIndex);
	_crawler.playSound(soundIndex);
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
		crawlerSpeedUp(10);
		break;

	case IRKeyCode::Pound:
		crawlerSpeedDown(10);
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
		playSound(2);
		//playSound(13);
		DEBUG_INFO("IRControlMode=ContinuousPressing");
		_crawlerIRControlMode = CrawlerIRControlMode::ContinuousPressing;
		break;

	case IRKeyCode::Button2:
		playSound(3);
		//playSound(12);
		DEBUG_INFO("IRControlMode=SinglePress");
		_crawlerIRControlMode = CrawlerIRControlMode::SinglePress;
		break;

	case IRKeyCode::Button3:
		playSound(4);
		break;

	case IRKeyCode::Button4:
		playSound(5);
		break;

	case IRKeyCode::Button5:
		playSound(6);
		break;

	case IRKeyCode::Button6:
		playSound(7);
		break;

	case IRKeyCode::Button7:
		playSound(8);
		break;

	case IRKeyCode::Button8:
		playSound(9);
		break;

	case IRKeyCode::Button9:
		playSound(10);
		break;

	case IRKeyCode::Button0:
		_secondSoundGroup = !_secondSoundGroup;
		break;

	default:
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

		crawlerShowStatusLight();

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

			crawlerShowStatusLight();
		}
	}

	return context->goTo(0u);
}

void loop()
{
	_freeMemoryCoroutine.continueExecution();
	_irRemoteCoroutine.continueExecution();
	_speedLightCoroutine.continueExecution();

	/*_protocol.RecevData();
	if (recv_flag = _protocol.ParserPackage()) {
		if (_protocol->GetRobotControlFun() == E_CONTROL_MODE) {
		}
	}*/
}