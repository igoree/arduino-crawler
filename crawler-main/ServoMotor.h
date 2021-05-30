#ifndef _SERVOMOTOR_h
#define _SERVOMOTOR_h

#include "stdint.h"

#define SERVO_MOTOR_MIN_ANGLE UINT8_C(0)
#define SERVO_MOTOR_MAX_ANGLE UINT8_C(180)

class Emakefun_Servo;

class ServoMotor
{
private:
	Emakefun_Servo* const _servoDriver;
	const uint8_t _maxAngle;
	const uint8_t _minAngle;
	const uint8_t _baseAngle;

	uint8_t limitAngle(uint8_t realAngle) const;

public:
	ServoMotor(Emakefun_Servo* servoDriver, uint8_t baseAngle = SERVO_MOTOR_MIN_ANGLE, uint8_t minAngle = SERVO_MOTOR_MIN_ANGLE, uint8_t maxAngle = SERVO_MOTOR_MAX_ANGLE);
	~ServoMotor();

	int16_t getAngle() const;
	bool setAngle(int16_t angle);
	bool turn(int16_t angleDelta);
};

#endif

