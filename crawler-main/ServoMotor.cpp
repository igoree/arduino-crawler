#include "ServoMotor.h"
#include "Emakefun_MotorDriver.h"

uint8_t ServoMotor::limitAngle(uint8_t realAngle) const
{
	if (realAngle < _minAngle)
	{
		return _minAngle;
	}

	if (realAngle > _maxAngle)
	{
		return _maxAngle;
	}

	return realAngle;
}

ServoMotor::ServoMotor(Emakefun_Servo* servoDriver, uint8_t baseAngle, uint8_t minAngle, uint8_t maxAngle)
	: _servoDriver(servoDriver),
	_maxAngle(maxAngle > SERVO_MOTOR_MAX_ANGLE ? SERVO_MOTOR_MAX_ANGLE : maxAngle),
	_minAngle(minAngle > _maxAngle ? _maxAngle : minAngle),
	_baseAngle(baseAngle < _minAngle ? _minAngle : baseAngle > _maxAngle ? _maxAngle : baseAngle)
{
}

ServoMotor::~ServoMotor()
{
}

int16_t ServoMotor::getAngle() const
{
	auto realAngle = limitAngle(_servoDriver->readDegrees());

	return (int16_t)realAngle - (int16_t)_baseAngle;
}

bool ServoMotor::setAngle(int16_t angle)
{
	auto realAngle = (int16_t)_baseAngle + angle;
	auto limitedRealAngle = limitAngle(realAngle);

	_servoDriver->writeServo(limitedRealAngle);

	return realAngle == limitedRealAngle;
}

bool ServoMotor::turn(int16_t angleDelta)
{
	return setAngle(getAngle() + angleDelta);
}
