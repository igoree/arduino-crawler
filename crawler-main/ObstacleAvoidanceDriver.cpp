#include "ObstacleAvoidanceDriver.h"
#include "Emakefun_MotorDriver.h"
#include "ServoMotor.h"

#include "DebugLevels.h"
#define DEBUG_LEVEL DEBUG_LEVEL_INFO
#include "DebugOutput.h"

#define MIN_DISTANCE UINT16_C(3)
#define CLEAR_DIRECTION_MIN_DISTANCE UINT16_C(15)
#define SERVO_ANGLE_STEP INT16_C(10)
#define MAX_CLEAR_ANGLE UINT8_C(90)

enum class DriveDirection : uint8_t
{
	None = 0,
	Forward = 1,
	Left = 2,
	Right = 3
};

struct ObstacleAvoidanceDriverState
{
	ObstacleAvoidanceDriverState(Crawler* crawler, Emakefun_Sensor* sensorDriver, Emakefun_Servo* servoDriver)
		: _crawler(crawler), _sensorDriver(sensorDriver), _sensorServo(new ServoMotor(servoDriver, 96, 6, 178)), 
		_direction(DriveDirection::None), _leftClearAngle(MAX_CLEAR_ANGLE), _rightClearAngle(MAX_CLEAR_ANGLE)
	{
	}

	~ObstacleAvoidanceDriverState()
	{
		delete _sensorServo;
	}

	Crawler* const _crawler;
	Emakefun_Sensor* const _sensorDriver;
	ServoMotor* const _sensorServo;

	DriveDirection _direction;
	uint8_t _leftClearAngle;
	uint8_t _rightClearAngle;
};

ObstacleAvoidanceDriver::ObstacleAvoidanceDriver(Crawler* crawler, Emakefun_Sensor* sensorDriver, Emakefun_Servo* servoDriver, Coroutine* driverCoroutine)
	: _state(new ObstacleAvoidanceDriverState(crawler, sensorDriver, servoDriver)), _coroutine(driverCoroutine)
{
}

ObstacleAvoidanceDriver::~ObstacleAvoidanceDriver()
{
	_coroutine->stop();
	delete _state;
}

CoroutineTaskResult* resetServoAsync(const CoroutineTaskContext* context)
{
	auto state = (ObstacleAvoidanceDriverState*)context->data;

	switch (context->step)
	{
	case 0:
		if (state->_sensorServo->getAngle() == 0)
		{
			return context->end();
		}

		state->_sensorServo->setAngle(0);

		return context->delayThenNext(300);

	default:
		return context->end();
	}
}

CoroutineTaskResult* findClearDirectionAsync(const CoroutineTaskContext* context)
{
	auto state = (ObstacleAvoidanceDriverState*)context->data;
	uint16_t distance(0);

	switch (context->step)
	{
	case 0:
		if (state->_direction == DriveDirection::Forward || state->_direction == DriveDirection::Left)
		{
			return context->executeThenNext(CoroutineTask(&resetServoAsync, state));
		}
		return context->executeThenGoTo(CoroutineTask(&resetServoAsync, state), 2);

	case 1:
		distance = state->_sensorDriver->GetUltrasonicDistance();
		if (distance < CLEAR_DIRECTION_MIN_DISTANCE)
		{
			if (!state->_sensorServo->turn(-SERVO_ANGLE_STEP))
			{
				state->_leftClearAngle = MAX_CLEAR_ANGLE;
			}
			else
			{
				return context->delayThenRepeat(35);
			}
		}
		else 
		{
			state->_leftClearAngle = static_cast<uint8_t>(-1 * state->_sensorServo->getAngle());
		}

		DEBUG_INFO("LeftClearAngle=%u, distance=%u", state->_leftClearAngle, distance);

		if (state->_direction == DriveDirection::Right || state->_direction == DriveDirection::Forward)
		{
			return context->executeThenNext(CoroutineTask(&resetServoAsync, state));
		}

		return context->endThenExecute(CoroutineTask(&resetServoAsync, state));

	case 2:
		distance = state->_sensorDriver->GetUltrasonicDistance();
		if (distance < CLEAR_DIRECTION_MIN_DISTANCE)
		{
			if (!state->_sensorServo->turn(SERVO_ANGLE_STEP))
			{
				state->_rightClearAngle = MAX_CLEAR_ANGLE;
			}
			else
			{
				return context->delayThenRepeat(35);
			}
		}
		else
		{
			state->_rightClearAngle = static_cast<uint8_t>(state->_sensorServo->getAngle());
		}

		DEBUG_INFO("RightClearAngle=%u, distance=%u", state->_rightClearAngle, distance);

		return context->endThenExecute(CoroutineTask(&resetServoAsync, state));
	}
}

CoroutineTaskResult* driveAsync(const CoroutineTaskContext* context)
{
	auto state = (ObstacleAvoidanceDriverState*)context->data;

	if (state->_direction == DriveDirection::None)
		return context->end();

	auto distance = state->_sensorDriver->GetUltrasonicDistance();
	if (distance < MIN_DISTANCE)
	{
		DEBUG_INFO("obstacle detected: %u cm", distance);

		return context->executeThenRepeat(CoroutineTask(&findClearDirectionAsync, state));
	}

	return context->delayThenRepeat(5);
}

void go(Coroutine* driverCoroutine, ObstacleAvoidanceDriverState* state, DriveDirection direction)
{
	if (state->_direction == DriveDirection::None)
	{
		driverCoroutine->start(CoroutineTask(&driveAsync, state));
	}

	state->_direction = direction;
}

void ObstacleAvoidanceDriver::goForward()
{
	go(_coroutine, _state, DriveDirection::Forward);
}

void ObstacleAvoidanceDriver::goLeft()
{
	go(_coroutine, _state, DriveDirection::Left);
}

void ObstacleAvoidanceDriver::goRight()
{
	go(_coroutine, _state, DriveDirection::Right);
}

void ObstacleAvoidanceDriver::stop()
{
	if (isActive()) 
	{
		_coroutine->start(CoroutineTask(&resetServoAsync, _state));
		_state->_direction = DriveDirection::None;
	}
}

bool ObstacleAvoidanceDriver::isActive()
{
	return _state->_direction != DriveDirection::None;
}
