#ifndef _PANTHER_TANK_H_
#define _PANTHER_TANK_H_

#include "IRRemoteHandler.h"
#include "SoundPlayer.h"
#include "LightController.h"
#include "CrawlerBehaviour.h"

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

class Emakefun_MotorDriver;
class Emakefun_DCMotor;
class Emakefun_Servo;
class Emakefun_Sensor;

class Crawler {
private:
	CrawlerStatus _status;
	uint8_t _bateryPin;
	uint8_t _batteryValue;
	uint8_t _servoBaseAngle;
	uint8_t _speed;
	Emakefun_MotorDriver* _motorDriver;
	Emakefun_DCMotor* _leftDrive, * _rightDrive;
	Emakefun_Servo* _servos[CRAWLER_SERVO_COUNT];
	Emakefun_Sensor* _sensors;
	SoundPlayer* _soundPlayer;
	LightController* _lightController;
	CrawlerBehaviour* _behaviour;
	IRRemoteHandler* _irRemoteHandler;

	void stopDrive(Emakefun_DCMotor* drive);
	void runDrive(Emakefun_DCMotor* drive, uint8_t speed, uint8_t direction);
	void setStatus(CrawlerStatus status);
	void validateState();
	uint8_t getOppositeDriveRotationSpeed();

public:
	Crawler();
	~Crawler();
	void init();
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

	void initIRRemote(Coroutine* irRemoteCoroutine);

	void initSoundPlayer(Coroutine* soundCoroutine);
	void playSound(Sound sound);
	void mute();
	void unmute();
	bool isMuted();

	void initLights(Coroutine* lightCoroutine);
	void showLightEffect(LightEffect effect);

	void initUltrasonic();

	void initServo();
	void setServoBaseAngle(uint8_t baseAngle);
	void setServoAngle(CrawlerServoKind servoKind, uint8_t angle);

	void initBehaviour();
	void useBehaviour(CrawlerBehaviourKind behaviourKind);
};

#endif  /* _CRAWLER_H_ */
