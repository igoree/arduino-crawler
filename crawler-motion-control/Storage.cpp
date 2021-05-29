#include "Storage.h"
#include "avr/eeprom.h"

const Storage storage;

MPUCalibrationSettings EEMEM mpuCalibrationSettings;

void Storage::setMPUCalibrationSettings(const MPUCalibrationSettings& settings) const
{
	eeprom_update_block(&settings, &mpuCalibrationSettings, sizeof(settings));
}

MPUCalibrationSettings Storage::getMPUCalibrationSettings() const
{
	MPUCalibrationSettings settings;
	eeprom_read_block(&settings, &mpuCalibrationSettings, sizeof(settings));

	return settings;
}
