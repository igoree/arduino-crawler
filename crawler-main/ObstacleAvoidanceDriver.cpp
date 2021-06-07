#include "ObstacleAvoidanceDriver.h"
#include "Emakefun_MotorDriver.h"
#include "ServoMotor.h"
#include "Crawler.h"

#include "DebugLevels.h"
#define DEBUG_LEVEL DEBUG_LEVEL_INFO
#include "DebugOutput.h"

#define MIN_DISTANCE UINT16_C(3)
#define CLEAR_DIRECTION_MIN_DISTANCE UINT16_C(15)
#define SERVO_ANGLE_STEP INT16_C(10)
auto constexpr SERVO_ANGLE_SPEED = 1 / 3.5;
auto constexpr SERVO_ANGLE_STEP_DURATION = static_cast<unsigned long>(SERVO_ANGLE_STEP / SERVO_ANGLE_SPEED);
#define MAX_CLEAR_ANGLE UINT8_C(90)
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
		direction(DriveDirection::None), currentDirection(DriveDirection::None), leftClearAngle(MAX_CLEAR_ANGLE), rightClearAngle(MAX_CLEAR_ANGLE)
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
	uint8_t leftClearAngle;
	uint8_t rightClearAngle;
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
	int16_t angle;

	switch (context->step)
	{
	case 0:
		angle = state->sensorServo->getAngle();
		if (angle == 0)
		{
			return context->end();
		}

		state->sensorServo->setAngle(0);

		return context->delayThenNext(static_cast<unsigned long>(abs(angle) / SERVO_ANGLE_SPEED));

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
		switch (state->direction)
		{
		case DriveDirection::Forward:
			state->leftClearAngle = MAX_CLEAR_ANGLE;
			state->rightClearAngle = MAX_CLEAR_ANGLE;
			return context->executeThenNext(CoroutineTask(&resetServoAsync, state));

		case DriveDirection::Left:
			state->crawler->turnLeftRotate();
			return context->goTo(4);

		case DriveDirection::Right:
			state->crawler->turnRightRotate();
			return context->goTo(4);

		default:
			return context->end();
		}

	case 1:
		distance = state->sensorDriver->GetUltrasonicDistance();
		if (distance < CLEAR_DIRECTION_MIN_DISTANCE)
		{
			if (!state->sensorServo->turn(-SERVO_ANGLE_STEP))
			{
				state->leftClearAngle = MAX_CLEAR_ANGLE;
			}
			else
			{
				return context->delayThenRepeat(SERVO_ANGLE_STEP_DURATION);
			}
		}
		else 
		{
			state->leftClearAngle = static_cast<uint8_t>(-1 * state->sensorServo->getAngle());
		}

		DEBUG_INFO("LeftClearAngle=%u, distance=%u", state->leftClearAngle, distance);

		return context->executeThenNext(CoroutineTask(&resetServoAsync, state));

	case 2:
		distance = state->sensorDriver->GetUltrasonicDistance();
		if (distance < CLEAR_DIRECTION_MIN_DISTANCE)
		{
			if (!state->sensorServo->turn(SERVO_ANGLE_STEP))
			{
				state->rightClearAngle = MAX_CLEAR_ANGLE;
			}
			else
			{
				return context->delayThenRepeat(SERVO_ANGLE_STEP_DURATION);
			}
		}
		else
		{
			state->rightClearAngle = static_cast<uint8_t>(state->sensorServo->getAngle());
		}

		DEBUG_INFO("RightClearAngle=%u, distance=%u", state->rightClearAngle, distance);

		return context->executeThenNext(CoroutineTask(&resetServoAsync, state));

	case 3:
		if (state->leftClearAngle <= state->rightClearAngle)
		{
			state->crawler->turnLeftRotate();
		}
		else
		{
			state->crawler->turnRightRotate();
		}

		return context->next();

	case 4:
		distance = state->sensorDriver->GetUltrasonicDistance();
		if (distance >= CLEAR_DIRECTION_MIN_DISTANCE)
		{
			state->crawler->stop();

			return context->end();
		}
		return context->repeat();
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

	if (state->direction == DriveDirection::None)
		return context->end();

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
		_coroutine->start(CoroutineTask(&resetServoAsync, _state));
		_state->direction = DriveDirection::None;
		_state->currentDirection = DriveDirection::None;
	}
}

bool ObstacleAvoidanceDriver::isActive()
{
	return _state->direction != DriveDirection::None;
}
