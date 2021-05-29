#include "OrientationSensor.h"
#include "MPU6050_6Axis_MotionApps_V6_12.h"
#include "Storage.h"

#include "DebugLevels.h"
#define DEBUG_LEVEL DEBUG_LEVEL_INFO
#include "DebugOutput.h"

struct OrientationSensorState 
{
	OrientationSensorState(MPU6050* mpu)
		: _mpu(mpu), _buffer(nullptr), _currentOrientation()
	{
	}

	~OrientationSensorState()
	{
		delete _mpu;
		delete _buffer;
	}

	MPU6050* const _mpu;
	uint8_t* _buffer;
	Orientation _currentOrientation;
};

OrientationSensor::OrientationSensor(Coroutine* orientationCoroutine)
	: _coroutine(orientationCoroutine), _state(new OrientationSensorState(new MPU6050()))
{
}

OrientationSensor::~OrientationSensor()
{
	delete _state;
}

CoroutineTaskResult* calculateOrientationAsync(const CoroutineTaskContext* context)
{
	auto state = (OrientationSensorState*)context->data;

	if (state->_mpu->dmpGetCurrentFIFOPacket(state->_buffer)) {
		Quaternion quaternion;
		VectorFloat gravity;
		float yawPitchRoll[3];

		state->_mpu->dmpGetQuaternion(&quaternion, state->_buffer);
		state->_mpu->dmpGetGravity(&gravity, &quaternion);
		state->_mpu->dmpGetYawPitchRoll(yawPitchRoll, &quaternion, &gravity);

		state->_currentOrientation.yaw = static_cast<int16_t>(yawPitchRoll[0] * RAD_TO_DEG);
		state->_currentOrientation.pitch = static_cast<int16_t>(yawPitchRoll[2] * RAD_TO_DEG);
		state->_currentOrientation.roll = static_cast<int16_t>(-1 * yawPitchRoll[1] * RAD_TO_DEG);
	}

	return context->delayThenRepeat(5);
}

void OrientationSensor::initialize()
{
	Wire.begin();
	Wire.setClock(400000);

	_state->_mpu->initialize();

	auto dmpInitializationStatus = _state->_mpu->dmpInitialize();
	if (dmpInitializationStatus != 0)
	{
		DEBUG_ERR("failed to init DMP");
		return;
	}

	_state->_mpu->setDMPEnabled(true);
	_state->_buffer = new uint8_t[_state->_mpu->dmpGetFIFOPacketSize()];

	auto calibrationSettings = storage.getMPUCalibrationSettings();

	_state->_mpu->setXAccelOffset(calibrationSettings.accelOffsetX);
	_state->_mpu->setYAccelOffset(calibrationSettings.accelOffsetY);
	_state->_mpu->setZAccelOffset(calibrationSettings.accelOffsetZ);

	_state->_mpu->setXGyroOffset(calibrationSettings.gyroOffsetX);
	_state->_mpu->setYGyroOffset(calibrationSettings.gyroOffsetY);
	_state->_mpu->setZGyroOffset(calibrationSettings.gyroOffsetZ);

	DEBUG_INFO("MPU set calibration: accel %d,%d,%d gyro %d,%d,%d", calibrationSettings.accelOffsetX, calibrationSettings.accelOffsetY, calibrationSettings.accelOffsetZ, calibrationSettings.gyroOffsetX, calibrationSettings.gyroOffsetY, calibrationSettings.gyroOffsetZ);

	_coroutine->start(CoroutineTask(&calculateOrientationAsync, _state));
}

void OrientationSensor::calibrate()
{
	DEBUG_INFO("start MPU calibration");

	_state->_mpu->CalibrateAccel();
	_state->_mpu->CalibrateGyro();

	MPUCalibrationSettings calibrationSettings;
	calibrationSettings.accelOffsetX = _state->_mpu->getXAccelOffset();
	calibrationSettings.accelOffsetY = _state->_mpu->getYAccelOffset();
	calibrationSettings.accelOffsetZ = _state->_mpu->getZAccelOffset();

	calibrationSettings.gyroOffsetX = _state->_mpu->getXGyroOffset();
	calibrationSettings.gyroOffsetY = _state->_mpu->getYGyroOffset();
	calibrationSettings.gyroOffsetZ = _state->_mpu->getZGyroOffset();

	storage.setMPUCalibrationSettings(calibrationSettings);

	DEBUG_INFO("MPU calibrated: accel %d,%d,%d gyro %d,%d,%d", calibrationSettings.accelOffsetX, calibrationSettings.accelOffsetY, calibrationSettings.accelOffsetZ, calibrationSettings.gyroOffsetX, calibrationSettings.gyroOffsetY, calibrationSettings.gyroOffsetZ);
}

Orientation OrientationSensor::getOrientation()
{
	return _state->_currentOrientation;
}
