#include "Crawler.h"
#include "Emakefun_MotorDriver.h"

#include "DebugLevels.h"
//#define DEBUG_LEVEL DEBUG_LEVEL_INFO
#include "DebugOutput.h"

#define MAX_OPPOSITE_DRIVE_ROTATION_SPEED 10

//MotorDriverBoard V4.0
Crawler::Crawler()
	: _status(CrawlerStatus::Stop), _batteryValue(0), _servoBaseAngle(90), _speed(0), _motorDriver(nullptr), _leftDrive(nullptr), _rightDrive(nullptr), 
	  _sensors(nullptr), _irRemoteHandler(nullptr), _soundPlayer(nullptr), _lightController(nullptr), _behaviour(nullptr)
{
	for (uint8_t i = 0; i < CRAWLER_SERVO_COUNT; i++)
	{
		_servos[i] = NULL;
	}
}

Crawler::~Crawler()
{
	delete _motorDriver;
	delete _leftDrive;
	delete _rightDrive;
	delete _irRemoteHandler;
	delete _soundPlayer;
	delete _lightController;
	delete _behaviour;

	for (uint8_t i = 0; i < CRAWLER_SERVO_COUNT; i++)
	{
		delete _servos[i];
	}
}

void Crawler::init()
{
	_motorDriver = new Emakefun_MotorDriver(0x60, MOTOR_DRIVER_BOARD_V5);
	_sensors = (Emakefun_Sensor*)_motorDriver->getSensor(E_SENSOR_MAX);
	_leftDrive = _motorDriver->getMotor(M2);
	_rightDrive = _motorDriver->getMotor(M1);
	_motorDriver->begin(50);
}

void Crawler::stopDrive(Emakefun_DCMotor* drive)
{
	drive->run(BRAKE);
}

void Crawler::runDrive(Emakefun_DCMotor* drive, uint8_t speed, uint8_t direction)
{
	if (speed >= 0 && speed <= 100) {
		drive->setSpeed((speed / 10) * 25.5);
		drive->run(direction);
		return;
	}

	DEBUG_ERR("Invalid speed: %d", speed);
}

void Crawler::goForward(void)
{
	setStatus(CrawlerStatus::RunForward);
	runDrive(_leftDrive, _speed, FORWARD);
	runDrive(_rightDrive, _speed, FORWARD);
}

void Crawler::goBack(void)
{
	setStatus(CrawlerStatus::RunBackward);
	runDrive(_leftDrive, _speed, BACKWARD);
	runDrive(_rightDrive, _speed, BACKWARD);
}

void Crawler::stop(void)
{
	setStatus(CrawlerStatus::Stop);
	stopDrive(_leftDrive);
	stopDrive(_rightDrive);
}

void Crawler::turnLeft(void)
{
	setStatus(CrawlerStatus::TurnLeft);

	runDrive(_leftDrive, getOppositeDriveRotationSpeed(), FORWARD);
	runDrive(_rightDrive, _speed, FORWARD);
}

void Crawler::turnRight(void)
{
	setStatus(CrawlerStatus::TurnRight);

	runDrive(_leftDrive, _speed, FORWARD);
	runDrive(_rightDrive, getOppositeDriveRotationSpeed(), FORWARD);
}

void Crawler::turnLeftRotate(void)
{
	setStatus(CrawlerStatus::TurnLeftRotate);
	runDrive(_leftDrive, _speed, BACKWARD);
	runDrive(_rightDrive, _speed, FORWARD);
}

void Crawler::turnRightRotate(void)
{
	setStatus(CrawlerStatus::TurnRightRotate);
	runDrive(_leftDrive, _speed, FORWARD);
	runDrive(_rightDrive, _speed, BACKWARD);
}

void Crawler::turnLeftBackward()
{
	setStatus(CrawlerStatus::TurnLeftBackward);

	runDrive(_leftDrive, getOppositeDriveRotationSpeed(), BACKWARD);
	runDrive(_rightDrive, _speed, BACKWARD);
}

void Crawler::turnRightBackward()
{
	setStatus(CrawlerStatus::TurnRightBackward);

	runDrive(_leftDrive, _speed, BACKWARD);
	runDrive(_rightDrive, getOppositeDriveRotationSpeed(), BACKWARD);
}

void Crawler::setSpeed(int8_t speed)
{
	auto oldSpeed = _speed;
	if (speed > 100)
	{
		_speed = 100;
	}
	else if (speed < 0)
	{
		_speed = 0;
	}
	else
	{
		_speed = speed;
	}

	if (oldSpeed == _speed)
		return;

	validateState();

	if (_behaviour != nullptr)
	{
		_behaviour->onSpeedChanged(_speed, oldSpeed);
	}
}

uint8_t Crawler::getSpeed()
{
	return _speed;
}

bool Crawler::speedUp(int8_t delta = 5)
{
	auto oldSpeed = _speed;
	setSpeed(_speed + delta);

	return oldSpeed != _speed;
}

bool Crawler::speedDown(int8_t delta = 5)
{
	auto oldSpeed = _speed;
	setSpeed(_speed - delta);

	return oldSpeed != _speed;
}

void Crawler::setStatus(CrawlerStatus status)
{
	if (_status == status)
		return;

	_status = status;

	if (_behaviour != nullptr)
	{
		_behaviour->onStatusChanged(status);
	}
}

void Crawler::validateState()
{
	switch (_status)
	{
	case CrawlerStatus::RunForward:
		goForward();
		break;

	case CrawlerStatus::RunBackward:
		goBack();
		break;

	case CrawlerStatus::TurnLeft:
		turnLeft();
		break;

	case CrawlerStatus::TurnRight:
		turnRight();
		break;

	case CrawlerStatus::TurnLeftRotate:
		turnLeftRotate();
		break;

	case CrawlerStatus::TurnRightRotate:
		turnRightRotate();
		break;

	case CrawlerStatus::TurnLeftBackward:
		turnLeftBackward();
		break;

	case CrawlerStatus::TurnRightBackward:
		turnRightBackward();
		break;

	default:
		break;
	}
}

uint8_t Crawler::getOppositeDriveRotationSpeed()
{
	if (_speed > MAX_OPPOSITE_DRIVE_ROTATION_SPEED * 3)
	{
		return MAX_OPPOSITE_DRIVE_ROTATION_SPEED;
	}

	return _speed / 3;
}

CrawlerStatus Crawler::getStatus()
{
	return _status;
}

uint8_t Crawler::getBattery()
{
	return _batteryValue;
}

void Crawler::initIRRemote(Coroutine* irRemoteCoroutine)
{
	_irRemoteHandler = new IRRemoteHandler((IRRemote*)_motorDriver->getSensor(E_IR), this, irRemoteCoroutine);
}

void Crawler::initSoundPlayer(Coroutine* soundCoroutine)
{
	_soundPlayer = new SoundPlayer((::Buzzer*)_motorDriver->getSensor(E_BUZZER), soundCoroutine);
}

void Crawler::playSound(Sound sound)
{
	_soundPlayer->play(sound);
}

void Crawler::mute()
{
	_soundPlayer->mute();
}

void Crawler::unmute()
{
	_soundPlayer->unmute();
}

bool Crawler::isMuted()
{
	return _soundPlayer->isMuted();
}

void Crawler::initLights(Coroutine* lightCoroutine)
{
	_lightController = new LightController((RGBLed*)_motorDriver->getSensor(E_RGB), lightCoroutine);
}

void Crawler::showLightEffect(LightEffect effect)
{
	_lightController->show(effect);
}

void Crawler::initUltrasonic()
{
	_motorDriver->getSensor(E_ULTRASONIC);
}

void Crawler::initServo()
{
	for (uint8_t i = 0; i < CRAWLER_SERVO_COUNT; i++)
	{
		_servos[i] = _motorDriver->getServo(i + 1);
	}
}

void Crawler::setServoBaseAngle(uint8_t baseAngle)
{
	_servoBaseAngle = baseAngle;
}

void Crawler::setServoAngle(CrawlerServoKind servoKind, byte angle)
{
	int servoAngle;
	if (angle > 360)
	{
		return;
	}

	if (angle == 90 || angle == 270)
	{
		servoAngle = _servoBaseAngle;
	}
	else if (angle >= 0 && angle <= 180)
	{
		servoAngle = _servoBaseAngle - 90 + angle;   // 180-degree-diff
	}

	uint8_t servoIndex = ((uint8_t)servoKind - 1);

	_servos[servoIndex]->writeServo(servoAngle);
}

void Crawler::initBehaviour()
{
	_behaviour = new CrawlerBehaviour(_soundPlayer, _lightController);
}

void Crawler::useBehaviour(CrawlerBehaviourKind behaviourKind)
{
	_behaviour->switchTo(behaviourKind, _status);
}