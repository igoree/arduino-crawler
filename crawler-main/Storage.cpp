#include "Storage.h"
#include "avr/eeprom.h"

const Storage storage;

uint8_t EEMEM soundEnabled;
uint8_t EEMEM crawlerBehaviourKind;

void Storage::setSoundEnabled(bool enabled) const
{
	eeprom_update_byte(&soundEnabled, (uint8_t) enabled);
}

bool Storage::getSoundEnabled() const
{
	return (bool)eeprom_read_byte(&soundEnabled);
}

void Storage::setCrawlerBehaviourKind(uint8_t behaviourKind)
{
	eeprom_update_byte(&crawlerBehaviourKind, behaviourKind);
}

uint8_t Storage::getCrawlerBehaviourKind()
{
	return eeprom_read_byte(&crawlerBehaviourKind);
}