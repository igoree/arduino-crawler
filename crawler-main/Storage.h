#ifndef _STORAGE_h
#define _STORAGE_h

#include "stdint.h"

class Storage
{
public:
	void setSoundEnabled(bool enabled) const;
	bool getSoundEnabled() const;
	void setCrawlerBehaviourKind(uint8_t behaviourKind) const;
	uint8_t getCrawlerBehaviourKind() const;
};

extern const Storage storage;

#endif
