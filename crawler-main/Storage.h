#ifndef _STORAGE_h
#define _STORAGE_h

class Storage
{
public:
	void setSoundEnabled(bool enabled) const;
	bool getSoundEnabled() const;
};

extern const Storage storage;

#endif
