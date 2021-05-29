#ifndef _ORIENTATIONSENSOR_h
#define _ORIENTATIONSENSOR_h

#include "stdint.h"
#include "Coroutine.h"

struct Orientation
{
	int16_t yaw;
	int16_t pitch;
	int16_t roll;
};

class MPU6050;
struct OrientationSensorState;

class OrientationSensor
{
private:
	Coroutine* const _coroutine;
	OrientationSensorState* const _state;

public:
	OrientationSensor(Coroutine* orientationCoroutine);
	~OrientationSensor();

	void initialize();
	void calibrate();
	Orientation getOrientation();
};

#endif

