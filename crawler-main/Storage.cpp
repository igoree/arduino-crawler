#include "Storage.h"
#include "avr/eeprom.h"

const Storage storage;

uint8_t EEMEM soundEnabled;
uint8_t EEMEM crawlerBehaviourKind;
uint8_t EEMEM irRemoteControlMode;

void Storage::setSoundEnabled(bool enabled) const
{
	eeprom_update_byte(&soundEnabled, (uint8_t) enabled);
}

bool Storage::getSoundEnabled() const
{
	return (bool)eeprom_read_byte(&soundEnabled);
}

void Storage::setCrawlerBehaviourKind(uint8_t behaviourKind) const
{
	eeprom_update_byte(&crawlerBehaviourKind, behaviourKind);
}

uint8_t Storage::getCrawlerBehaviourKind() const
{
	return eeprom_read_byte(&crawlerBehaviourKind);
}

void Storage::setIRRemoteControlMode(uint8_t mode) const
{
	eeprom_update_byte(&irRemoteControlMode, mode);
}

uint8_t Storage::getIRRemoteControlMode() const
{
	return eeprom_read_byte(&irRemoteControlMode);
}