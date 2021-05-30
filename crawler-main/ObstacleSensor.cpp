#include "ObstacleSensor.h"
#include "Emakefun_MotorDriver.h"
#include "ServoMotor.h"

ObstacleSensor::ObstacleSensor(Emakefun_Sensor* sensorDriver, Emakefun_Servo* servoDriver)
	: _sensorDriver(sensorDriver), _sensorServo(new ServoMotor(servoDriver, 96, 6, 178))
{
	_sensorServo->setAngle(0);
}

ObstacleSensor::~ObstacleSensor()
{
	delete _sensorServo;
}

uint16_t ObstacleSensor::getDistance()
{
	return _sensorDriver->GetUltrasonicDistance();
}
