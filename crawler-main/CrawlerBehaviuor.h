// CrawlerBehaviuor.h

#ifndef _CRAWLERBEHAVIUOR_h
#define _CRAWLERBEHAVIUOR_h

#include "SoundPlayer.h"
#include "LightController.h"

enum class CrawlerBehaviourKind : uint8_t
{
	Default,
	Police
};

enum class CrawlerStatus : uint8_t;

class CrawlerBehaviourStrategy
{
public:
	virtual void init(SoundPlayer* soundPlayer, LightController* lightController);
	virtual void onStart(SoundPlayer* soundPlayer, LightController* lightController);
	virtual void onStatusChanged(SoundPlayer* soundPlayer, LightController* lightController, CrawlerStatus newStatus);
	virtual void onSpeedChanged(SoundPlayer* soundPlayer, LightController* lightController, uint8_t newSpeed);
};

class DefaultCrawlerBehaviourStrategy : public CrawlerBehaviourStrategy
{
};

class PoliceCrawlerBehaviuorStrategy : public CrawlerBehaviourStrategy
{
};

class CrawlerBehaviour
{
private:
	const DefaultCrawlerBehaviourStrategy _defaultStrategy;
	const PoliceCrawlerBehaviuorStrategy _policeStrategy;

	SoundPlayer* const _soundPlayer;
	LightController* const _lightController;
public:
	CrawlerBehaviour(SoundPlayer* soundPlayer, LightController* lightController);
	~CrawlerBehaviour();

	void use(CrawlerBehaviourKind behaviourKind);

	void onStart();
	void onStatusChanged(CrawlerStatus newStatus);
	void onSpeedChanged(uint8_t newSpeed);
};

#endif
