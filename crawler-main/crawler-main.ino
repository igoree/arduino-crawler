#include "Coroutine.h"
#include "ProtocolParser.h"
#include "Crawler.h"

#include "DebugLevels.h"
#define DEBUG_LEVEL DEBUG_LEVEL_INFO
#include "DebugOutput.h"

Coroutine _freeMemoryCoroutine("freeMemory");
Coroutine _irRemoteCoroutine("IRRemote");
Coroutine _lightCoroutine("speedLight", 3);
Coroutine _soundCoroutine("sound", 4);

ProtocolParser _protocol(&Serial);
Crawler _crawler;

int _lastFreeMemory(0);

void setup()
{
	INIT_DEBUG();
	_crawler.init();
	_crawler.initServo();
	_crawler.initLights(&_lightCoroutine);
	_crawler.initSoundPlayer(&_soundCoroutine);
	_crawler.initIRRemote(&_irRemoteCoroutine);
	_crawler.setSpeed(50);
	_crawler.setServoBaseAngle(90);
	_crawler.setServoAngle(CrawlerServoKind::Ultrasonic, 90);
	_crawler.initUltrasonic();
	_crawler.initBehaviour();

	DEBUG_INFO("init ok");

	_crawler.playSound(Sound::TurnOn);
	_crawler.showLightEffect(LightEffect::TurnOn);

#if DEBUG_LEVEL <= DEBUG_LEVEL_INFO
	_freeMemoryCoroutine.start(CoroutineTask(&monitorFreeMemoryAsync));
#endif
}

CoroutineTaskResult* monitorFreeMemoryAsync(const CoroutineTaskContext* context)
{
	auto freeMemory = debug_freeMemory();
	if (freeMemory != _lastFreeMemory)
	{
		DEBUG_INFO("free memory: %d", freeMemory);

		_lastFreeMemory = freeMemory;
	}

	return context->delayThenRepeat(500);
}

void loop()
{
	_freeMemoryCoroutine.continueExecution();
	_irRemoteCoroutine.continueExecution();
	_lightCoroutine.continueExecution();
	_soundCoroutine.continueExecution();

	/*_protocol.RecevData();
	if (recv_flag = _protocol.ParserPackage()) {
		if (_protocol->GetRobotControlFun() == E_CONTROL_MODE) {
		}
	}*/
}