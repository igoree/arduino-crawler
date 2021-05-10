#ifndef _PANTHER_TANK_H_
#define _PANTHER_TANK_H_

#include "Emakefun_MotorDriver.h"
#include "IRKeyMap.h"
#include "SoundPlayer.h"

#define CRAWLER_SERVO_COUNT 1

enum class CrawlerServoKind : uint8_t
{
	Ultrasonic = 1,
	Servo2 = 2,
	Servo3 = 3,
	Servo4 = 4,
	Servo5 = 5,
	Servo6 = 6
};

enum class CrawlerStatus : uint8_t
{
	Stop = 0,
	RunForward,
	RunBackward,
	TurnLeft,
	TurnRight,
	TurnLeftBackward,
	TurnRightBackward,
	TurnRightRotate,
	TurnLeftRotate,
	LowPower
};

enum class CrawlerUltrasonicServoDirection : uint8_t
{
	Front = 0,
	Left,
	Right
};

enum class CrawlerRgbEffect : uint8_t
{
	Breathing = 0,
	Rotate = 1,
	Flash = 2
};

class Crawler {
private:
	CrawlerStatus _status;
	uint8_t _bateryPin;
	uint8_t _batteryValue;
	uint8_t _servoBaseAngle;
	uint8_t _speed;
	Emakefun_MotorDriver _motorDriver;
	Emakefun_DCMotor* _leftDrive, * _rightDrive;
	Emakefun_Servo* _servos[CRAWLER_SERVO_COUNT];
	Emakefun_Sensor* _sensors;
	IRRemote* _ir;
	SoundPlayer* _soundPlayer;

	void stopDrive(Emakefun_DCMotor* drive);
	void runDrive(Emakefun_DCMotor* drive, uint8_t speed, uint8_t direction);
	void setStatus(CrawlerStatus status);
	void validateState();
	uint8_t getOppositeDriveRotationSpeed();

public:
	RGBLed* Rgb;
	Nrf24l* Nrf24L01;

	Crawler();
	~Crawler();
	void init(int leftDrive, int rightDrive);
	void goForward();
	void goBack();
	void turnLeft();
	void turnRight();
	void turnLeftBackward();
	void turnRightBackward();
	void turnLeftRotate();
	void turnRightRotate();
	void stop();
	void setSpeed(int8_t s);
	bool speedUp(int8_t delta = 5);
	bool speedDown(int8_t delta = 5);
	uint8_t  getSpeed();
	CrawlerStatus getStatus();
	uint8_t getBattery();

	void initIR();
	IRKeyCode getPressedIRKey();

	void initSoundPlayer(Coroutine* soundCoroutine);
	void playSound(Sound sound);

	void initRgb();
	void setRgbColor(E_RGB_INDEX index, long Color);
	void setRgbEffect(E_RGB_INDEX index, long Color, CrawlerRgbEffect effect);
	void lightOff();

	void initUltrasonic();
	byte getUltrasonicValue(CrawlerUltrasonicServoDirection direction);

	void initServo();
	void setServoBaseAngle(uint8_t baseAngle);
	void setServoAngle(CrawlerServoKind servoKind, uint8_t angle);

	void initNrf24L01(uint8_t* rxAddr);
};

#endif  /* _CRAWLER_H_ */
