#ifndef _OBSTACLESENSOR_h
#define _OBSTACLESENSOR_h

#include "stdint.h"

class Emakefun_Sensor;
class Emakefun_Servo;
class ServoMotor;

class ObstacleSensor 
{
private:
	Emakefun_Sensor* const _sensorDriver;
	ServoMotor* const _sensorServo;
public:
	ObstacleSensor(Emakefun_Sensor* sensorDriver, Emakefun_Servo* servoDriver);
	~ObstacleSensor();

	uint16_t getDistance();
};

#endif

