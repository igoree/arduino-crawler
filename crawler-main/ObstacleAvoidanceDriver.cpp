#include "ObstacleAvoidanceDriver.h"
#include "Emakefun_MotorDriver.h"
#include "ServoMotor.h"
#include "Crawler.h"

#include "DebugLevels.h"
#define DEBUG_LEVEL DEBUG_LEVEL_INFO
#include "DebugOutput.h"

#define MIN_DISTANCE UINT16_C(10)
#define CLEAR_DIRECTION_MIN_DISTANCE UINT16_C(40)
#define SERVO_ANGLE_STEP INT16_C(10)
auto constexpr SERVO_ANGLE_SPEED = 1 / 4.0;
auto constexpr SERVO_ANGLE_STEP_DURATION = static_cast<unsigned long>(SERVO_ANGLE_STEP / SERVO_ANGLE_SPEED);
#define MAX_SERVO_ANGLE UINT8_C(90)
#define DISTANCE_CHECK_PERIOD 5ul

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
		: crawler(crawler), sensorDriver(sensorDriver), sensorServo(new ServoMotor(servoDriver, 96, 6, 178)), 
		direction(DriveDirection::None), currentDirection(DriveDirection::None), 
		closestObstacleAngle(0), closestObstacleDistance(0u), servoTargetAngle(0)
	{
	}

	~ObstacleAvoidanceDriverState()
	{
		delete sensorServo;
	}

	Crawler* const crawler;
	Emakefun_Sensor* const sensorDriver;
	ServoMotor* const sensorServo;

	DriveDirection direction;
	DriveDirection currentDirection;

	int16_t closestObstacleAngle;
	uint16_t closestObstacleDistance;
	int16_t servoTargetAngle;
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

CoroutineTaskResult* adjustServoAngle(const CoroutineTaskContext* context)
{
	auto state = (ObstacleAvoidanceDriverState*)context->data;
	int16_t angle;

	switch (context->step)
	{
	case 0:
		angle = state->sensorServo->getAngle();
		if (angle == state->servoTargetAngle)
		{
			return context->end();
		}

		state->sensorServo->setAngle(state->servoTargetAngle);

		return context->delayThenNext(static_cast<unsigned long>(abs(state->servoTargetAngle - angle) / SERVO_ANGLE_SPEED));

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
		state->servoTargetAngle = 0;
		return context->executeThenNext(CoroutineTask(&adjustServoAngle, state));

	case 1:
		state->closestObstacleAngle = 0;
		state->closestObstacleDistance = state->sensorDriver->GetUltrasonicDistance();
		state->servoTargetAngle = -static_cast<int16_t>(MAX_SERVO_ANGLE);

		return context->executeThenNext(CoroutineTask(&adjustServoAngle, state));

	case 2:
		distance = state->sensorDriver->GetUltrasonicDistance();
		if (distance < state->closestObstacleDistance)
		{
			state->closestObstacleDistance = distance;
			state->closestObstacleAngle = state->sensorServo->getAngle();
		}

		if (!state->sensorServo->turn(SERVO_ANGLE_STEP))
		{
			DEBUG_INFO("closest obstacle: angle=%d, distance=%u cm", state->closestObstacleAngle, state->closestObstacleDistance);

			state->servoTargetAngle = state->closestObstacleAngle;
			return context->executeThenNext(CoroutineTask(&adjustServoAngle, state));
		}

		return context->delayThenRepeat(SERVO_ANGLE_STEP_DURATION);

	case 3:
		if (state->closestObstacleDistance < MIN_DISTANCE)
		{
			state->crawler->goBack();

			return context->next();
		}

		return context->goTo(context->step + 2);

	case 4:
		distance = state->sensorDriver->GetUltrasonicDistance();
		if (distance < MIN_DISTANCE)
		{
			return context->delayThenRepeat(DISTANCE_CHECK_PERIOD);
		}

		state->crawler->stop();

		return context->next();

	case 5:
		state->servoTargetAngle = 0;
		return context->executeThenNext(CoroutineTask(&adjustServoAngle, state));

	case 6:
		switch (state->direction)
		{
		case DriveDirection::Forward:
			if (state->closestObstacleAngle > 0)
			{
				state->crawler->turnLeftRotate();
			}
			else
			{
				state->crawler->turnRightRotate();
			}
			break;

		case DriveDirection::Left:
			state->crawler->turnLeftRotate();
			break;

		case DriveDirection::Right:
			state->crawler->turnRightRotate();
			break;

		default:
			return context->end();
		}

		return context->next();

	case 7:
		distance = state->sensorDriver->GetUltrasonicDistance();
		if (distance >= CLEAR_DIRECTION_MIN_DISTANCE)
		{
			state->crawler->stop();

			return context->end();
		}
		return context->delayThenRepeat(DISTANCE_CHECK_PERIOD);
	}
}

void runCrawler(Crawler* crawler, DriveDirection direction)
{
	switch (direction)
	{
	case DriveDirection::Forward:
		crawler->goForward();
		break;

	case DriveDirection::Left:
		crawler->turnLeft();
		break;

	case DriveDirection::Right:
		crawler->turnRight();
		break;
	}
}

CoroutineTaskResult* driveAsync(const CoroutineTaskContext* context)
{
	auto state = (ObstacleAvoidanceDriverState*)context->data;

	switch (context->step)
	{
	case 0:
		state->servoTargetAngle = 0;
		return context->executeThenNext(CoroutineTask(&adjustServoAngle, state));

	default:
		if (state->direction == DriveDirection::None)
			context->end();

		if (state->direction != state->currentDirection)
		{
			state->currentDirection = state->direction;
			runCrawler(state->crawler, state->currentDirection);
		}

		auto distance = state->sensorDriver->GetUltrasonicDistance();
		if (distance < MIN_DISTANCE)
		{
			DEBUG_INFO("obstacle detected: %u cm", distance);

			state->crawler->stop();
			state->currentDirection = DriveDirection::None;

			return context->executeThenRepeat(CoroutineTask(&findClearDirectionAsync, state));
		}

		return context->delayThenRepeat(DISTANCE_CHECK_PERIOD);
	}
}

void go(Coroutine* driverCoroutine, ObstacleAvoidanceDriverState* state, DriveDirection direction)
{
	if (state->direction == DriveDirection::None)
	{
		driverCoroutine->start(CoroutineTask(&driveAsync, state));
	}

	state->direction = direction;
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
		_state->servoTargetAngle = 0;
		_coroutine->start(CoroutineTask(&adjustServoAngle, _state));
		_state->direction = DriveDirection::None;
		_state->currentDirection = DriveDirection::None;
	}
}

bool ObstacleAvoidanceDriver::isActive()
{
	return _state->direction != DriveDirection::None;
}
