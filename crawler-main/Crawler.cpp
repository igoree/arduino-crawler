#include "Crawler.h"
#include "Sounds.h"
#include "debug.h"

//MotorDriverBoard V4.0
Crawler::Crawler()
{
	_status = CrawlerStatus::Stop;
	_batteryValue = 0;
	_speed = 0;
	_leftDrive = _rightDrive = NULL;
	_ir = NULL;
	Buzzer = NULL;
	Rgb = NULL;
	Nrf24L01 = NULL;

	for (uint8_t i = 0; i < CRAWLER_SERVO_COUNT; i++)
	{
		_servos[i] = NULL;
	}

	setStatus(CrawlerStatus::Stop);
}

Crawler::~Crawler()
{
	delete _leftDrive;
	delete _rightDrive;
	delete _ir;
	delete Buzzer;
	delete Rgb;
	delete Nrf24L01;

	for (uint8_t i = 0; i < CRAWLER_SERVO_COUNT; i++)
	{
		delete _servos[i];
	}
}

void Crawler::init(int leftDrive, int rightDrive)
{
	_motorDriver = Emakefun_MotorDriver(0x60, MOTOR_DRIVER_BOARD_V5);
	_sensors = (Emakefun_Sensor*)_motorDriver.getSensor(E_SENSOR_MAX);
	_leftDrive = _motorDriver.getMotor(leftDrive);
	_rightDrive = _motorDriver.getMotor(rightDrive);
	delay(200);
	_motorDriver.begin(50);
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
	runDrive(_leftDrive, _speed / 3, FORWARD);
	runDrive(_rightDrive, _speed, FORWARD);
}

void Crawler::turnRight(void)
{
	setStatus(CrawlerStatus::TurnRight);
	runDrive(_leftDrive, _speed, FORWARD);
	runDrive(_rightDrive, _speed / 3, FORWARD);
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

void Crawler::setSpeed(int8_t speed)
{
	if (speed > 100) {
		_speed = 100;
		return;
	}
	else if (speed < 0) {
		_speed = 0;
		return;
	}
	_speed = speed;
}

uint8_t Crawler::getSpeed()
{
	return _speed;
}

bool Crawler::speedUp(int8_t delta = 5)
{
	_status = CrawlerStatus::SpeedUp;
	uint8_t oldSpeed = _speed;
	setSpeed(_speed + delta);

	return oldSpeed != _speed;
}

bool Crawler::speedDown(int8_t delta = 5)
{
	_status = CrawlerStatus::SpeedDown;
	uint8_t oldSpeed = _speed;
	setSpeed(_speed - delta);

	return oldSpeed != _speed;
}

void Crawler::setStatus(CrawlerStatus status)
{
	_status = status;
}

CrawlerStatus Crawler::getStatus()
{
	return _status;
}

uint8_t Crawler::getBattery()
{
	return _batteryValue;
}

void Crawler::initIr()
{
	_ir = (IRRemote*)_motorDriver.getSensor(E_IR);
}

IRKeyCode Crawler::getPressedIRKey()
{
	return (IRKeyCode) _ir->getCode();
}

void Crawler::initBuzzer(void)
{
	Buzzer = (::Buzzer*)_motorDriver.getSensor(E_BUZZER);
}

void Crawler::playSound(byte songName)
{
	_sensors->Sing(songName);
}

void Crawler::initRgb(void)
{
	Rgb = (RGBLed*)_motorDriver.getSensor(E_RGB);
}

void Crawler::setRgbColor(E_RGB_INDEX index, long Color)
{
	_sensors->SetRgbColor(index, Color);
}

void Crawler::lightOff(void)
{
	_sensors->SetRgbColor(E_RGB_ALL, RGB_BLACK);
}

void Crawler::setRgbEffect(E_RGB_INDEX index, long Color, CrawlerRgbEffect effect)
{
	if (Rgb != NULL) {
		switch (effect) {
		case CrawlerRgbEffect::Breathing:
			for (long i = 0; i < 256; i++) {
				setRgbColor(index, (i << 16) | (i << 8) | i);
				delay((i < 18) ? 18 : (256 / i));
			}
			for (long i = 255; i >= 0; i--) {
				setRgbColor(index, (i << 16) | (i << 8) | i);
				delay((i < 18) ? 18 : (256 / i));
			}
			break;
		case CrawlerRgbEffect::Flash:
			for (byte i = 0; i < 6; i++) {
				setRgbColor(index, Color);
				delay(100);
				setRgbColor(index, 0);
				delay(100);
			}
			break;
		}
	}
}

void Crawler::initUltrasonic(void)
{
	_motorDriver.getSensor(E_ULTRASONIC);
}

byte Crawler::getUltrasonicValue(CrawlerUltrasonicServoDirection direction)
{
	byte distance;
	if (direction == CrawlerUltrasonicServoDirection::Front) {
		setServoAngle(CrawlerServoKind::Ultrasonic, 90);
		distance = _sensors->GetUltrasonicDistance();
	}
	else if (direction == CrawlerUltrasonicServoDirection::Left) {
		setServoAngle(CrawlerServoKind::Ultrasonic, 180);
		distance = _sensors->GetUltrasonicDistance();
		delay(400);
		setServoAngle(CrawlerServoKind::Ultrasonic, 90);
	}
	else if (direction == CrawlerUltrasonicServoDirection::Right) {
		setServoAngle(CrawlerServoKind::Ultrasonic, 15);
		distance = _sensors->GetUltrasonicDistance();
		delay(400);
		setServoAngle(CrawlerServoKind::Ultrasonic, 90);
	}
	return distance;
}

void Crawler::initServo()
{
	for (uint8_t i = 0; i < CRAWLER_SERVO_COUNT; i++)
	{
		_servos[i] = _motorDriver.getServo(i + 1);
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

void Crawler::initNrf24L01(uint8_t* rxAddr)
{
	Nrf24L01 = (Nrf24l*) _motorDriver.getSensor(E_NRF24L01);
	Nrf24L01->setRADDR(rxAddr);
}