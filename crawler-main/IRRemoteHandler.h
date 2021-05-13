#ifndef _IRREMOTEHANDLER_h
#define _IRREMOTEHANDLER_h

#include "IRKeyMap.h"
#include "Coroutine.h"

class IRRemote;
class Crawler;
struct IRRemoteHandlerState;

class IRRemoteHandler
{
private:
	Coroutine* const _coroutine;
	IRRemoteHandlerState* const _state;
public:
	IRRemoteHandler(IRRemote* irRemote, Crawler* crawler, Coroutine* irRemoteCoroutine);
	~IRRemoteHandler();
};

#endif

