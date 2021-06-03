#pragma once

#include "stdint.h"

struct MPUCalibrationSettings
{
	int16_t accelOffsetX;
	int16_t accelOffsetY;
	int16_t accelOffsetZ;

	int16_t gyroOffsetX;
	int16_t gyroOffsetY;
	int16_t gyroOffsetZ;
};

class Storage
{
public:
	void setMPUCalibrationSettings(const MPUCalibrationSettings& settings) const;
	MPUCalibrationSettings getMPUCalibrationSettings() const;
};

extern const Storage storage;
