#pragma once

#include "stdint.h"

class Storage
{
public:
	void setSoundEnabled(bool enabled) const;
	bool getSoundEnabled() const;
	void setCrawlerBehaviourKind(uint8_t behaviourKind) const;
	uint8_t getCrawlerBehaviourKind() const;
	void setIRRemoteControlMode(uint8_t mode) const;
	uint8_t getIRRemoteControlMode() const;
};

extern const Storage storage;
