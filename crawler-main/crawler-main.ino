#include "Coroutine.h"
#include "ProtocolParser.h"
#include "Crawler.h"
#include "IRKeyMap.h"

#include "debugLevels.h"
#define DEBUG_LEVEL DEBUG_LEVEL_INFO
#include "debug.h"

Coroutine _freeMemoryCoroutine("freeMemory", 1);
Coroutine _irRemoteCoroutine("IRRemote", 1);
Coroutine _soundCoroutine("sound", 1);

ProtocolParser _protocol(&Serial);
Crawler _crawler;

int _lastFreeMemory(0);

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

	_crawler.playSound(S_connection);

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

void crawlerColorState()
{
	switch (_crawler.getStatus()) {
	case CrawlerStatus::RunForward:
		_crawler.setRgbColor(E_RGB_ALL, RGB_WHITE);
		break;

	case CrawlerStatus::TurnLeft:
	case CrawlerStatus::TurnLeftRotate:
		_crawler.setRgbColor(E_RGB_RIGHT, RGB_ORANGE);
		break;

	case CrawlerStatus::TurnRight:
	case CrawlerStatus::TurnRightRotate:
		_crawler.setRgbColor(E_RGB_LEFT, RGB_ORANGE);
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

void crawlerSpeedUp(uint8_t delta)
{
	_crawler.playSound(S_connection);
	_crawler.setRgbColor(E_RGB_ALL, _crawler.getSpeed() * 2.5);
	_crawler.speedUp(delta);
}

void crawlerSpeedDown(uint8_t delta)
{
	_crawler.playSound(S_disconnection);
	_crawler.setRgbColor(E_RGB_ALL, _crawler.getSpeed() * 2.5);
	_crawler.speedDown(delta);
}

void playSound(uint8_t soundIndex) {
	if (_secondSoundGroup) {
		soundIndex = soundIndex + 9;
	}
	_crawler.playSound(soundIndex);
}

void crawlerHandleIRCommand(IRKeyCode irKeyCode)
{
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
		_crawler.turnLeft();
		break;

	case IRKeyCode::Right:
		_crawler.turnRight();
		break;

	case IRKeyCode::Button1:
		playSound(2);
		break;

	case IRKeyCode::Button2:
		playSound(3);
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

		crawlerColorState();

		return context->delayThenGoTo(75, 1u);
	}
	else if (context->step == 1u) 
	{
		return context->delayThenGoTo(75, 2u);
	}
	else if (context->step == 2u)
	{
		if (_crawler.getStatus() != CrawlerStatus::Stop)
		{
			_crawler.stop();

			crawlerColorState();
		}
	}

	return context->goTo(0u);
}

void loop()
{
	_freeMemoryCoroutine.continueExecution();
	_irRemoteCoroutine.continueExecution();

	/*_protocol.RecevData();
	if (recv_flag = _protocol.ParserPackage()) {
		if (_protocol->GetRobotControlFun() == E_CONTROL_MODE) {
		}
	}*/
}