#include "debug.h"
#include "CoroutineScheduler.h"
#include "ProtocolParser.h"
#include "Crawler.h"
#include "IRKeyMap.h"

ProtocolParser _protocol = ProtocolParser(&Serial);
Crawler _crawler = Crawler();
byte _count = 0;
bool _secondSoundGroup = false;

void setup()
{
	INIT_DEBUG();
	_crawler.init(M2, M1);
	_crawler.initServo();
	_crawler.initRgb();
	_crawler.initBuzzer();
	_crawler.initIr();
	_crawler.setSpeed(50);
	_crawler.setServoBaseAngle(90);
	_crawler.setServoAngle(CrawlerServoKind::Ultrasonic, 90);
	_crawler.initUltrasonic();
	DEBUG_INFO("init ok");
	_crawler.playSound(S_connection);
}

void playSound(uint8_t soundIndex) {
	if (_secondSoundGroup) {
		soundIndex = soundIndex + 9;
	}
	_crawler.playSound(soundIndex);
}

void handleInfaredRemote(IRKeyCode irKeyCode)
{
	switch (irKeyCode) {
	case IRKeyCode::Star:
		_crawler.playSound(S_connection);
		_crawler.setRgbColor(E_RGB_ALL, _crawler.getSpeed() * 2.5);
		_crawler.speedUp(10);
		DEBUG_INFO("Speed = %d \n", _crawler.getSpeed());
		break;
	case IRKeyCode::Pound:
		_crawler.playSound(S_disconnection);
		_crawler.setRgbColor(E_RGB_ALL, _crawler.getSpeed() * 2.5);
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

void handleUltrasonicAvoidance(void)
{
	uint16_t UlFrontDistance, UlLeftDistance, UlRightDistance;
	UlFrontDistance = _crawler.getUltrasonicValue(CrawlerUltrasonicServoDirection::Front);
	if (_count++ > 50) {
		//_crawler.SendUltrasonicData();
		_count = 0;
	}
	DEBUG_INFO("UlFrontDistance = %d \n", UlFrontDistance);
	if (UlFrontDistance < UL_LIMIT_MIN)
	{
		_crawler.setSpeed(80);
		_crawler.goBack();
		delay(200);
	}
	if (UlFrontDistance < UL_LIMIT_MID)
	{
		_crawler.stop();
		delay(100);
		UlRightDistance = _crawler.getUltrasonicValue(CrawlerUltrasonicServoDirection::Right);
		delay(50);
		UlLeftDistance = _crawler.getUltrasonicValue(CrawlerUltrasonicServoDirection::Left);
		if ((UlRightDistance > UL_LIMIT_MIN) && (UlRightDistance < UL_LIMIT_MAX)) {
			_crawler.setSpeed(100);
			_crawler.turnRight();
			delay(400);
		}
		else if ((UlLeftDistance > UL_LIMIT_MIN) && (UlLeftDistance < UL_LIMIT_MAX)) {
			_crawler.setSpeed(100);
			_crawler.turnLeft();
			delay(400);
		}
		else if ((UlRightDistance < UL_LIMIT_MIN) && (UlLeftDistance < UL_LIMIT_MIN)) {
			_crawler.setSpeed(400);
			_crawler.turnLeft();
			delay(800);
		}
	}
	else {
		_crawler.setSpeed(80);
		_crawler.goForward();
	}
}

void ultrasonicFollow()
{
	_crawler.setSpeed(40);
	uint16_t UlFrontDistance = _crawler.getUltrasonicValue(CrawlerUltrasonicServoDirection::Front);
	delay(10);
	if (UlFrontDistance < 10) {
		_crawler.goBack();
	}
	else if (UlFrontDistance > 14) {
		_crawler.goForward();
	}
	else if (10 <= UlFrontDistance <= 14) {
		_crawler.stop();
	}
}

void loop()
{
	static bool recv_flag;
	_protocol.RecevData();
	if (recv_flag = _protocol.ParserPackage()) {
		/*if (_protocol->GetRobotControlFun() == E_CONTROL_MODE) {
		}*/
	}

	auto irKeyCode = _crawler.getPressedIRKey();
	if (irKeyCode != IRKeyCode::Unknown)
	{
		DEBUG_INFO("irKeyCode = %x", (uint8_t)irKeyCode);
		handleInfaredRemote(irKeyCode);
		delay(110);
	}
	else
	{
		if (_crawler.getStatus() != CrawlerStatus::Stop)
		{
			_crawler.stop();
		}
	}

	switch (_crawler.getStatus()) {
	case CrawlerStatus::RunForward:
		_crawler.setRgbColor(E_RGB_ALL, RGB_WHITE);
		break;

	case CrawlerStatus::TurnLeftRotate:
		_crawler.setRgbColor(E_RGB_LEFT, RGB_WHITE);
		break;

	case CrawlerStatus::TurnRightRotate:
		_crawler.setRgbColor(E_RGB_RIGHT, RGB_WHITE);
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