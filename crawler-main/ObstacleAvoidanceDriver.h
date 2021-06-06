#pragma once

#include "stdint.h"
#include "Coroutine.h"

class Emakefun_Sensor;
class Emakefun_Servo;
class Crawler;
struct ObstacleAvoidanceDriverState;

class ObstacleAvoidanceDriver
{
private:
	Coroutine* const _coroutine;
	ObstacleAvoidanceDriverState* const _state;

public:
	ObstacleAvoidanceDriver(Crawler* crawler, Emakefun_Sensor* sensorDriver, Emakefun_Servo* servoDriver, Coroutine* driverCoroutine);
	~ObstacleAvoidanceDriver();

	void goForward();
	void goLeft();
	void goRight();
	void stop();
	bool isActive();
};

