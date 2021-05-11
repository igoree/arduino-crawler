#include "Storage.h"
#include "avr/eeprom.h"

const Storage storage;

uint8_t EEMEM soundEnabled;

void Storage::setSoundEnabled(bool enabled) const
{
	eeprom_update_byte(&soundEnabled, (uint8_t) enabled);
}

bool Storage::getSoundEnabled() const
{
	return (bool)eeprom_read_byte(&soundEnabled);
}
