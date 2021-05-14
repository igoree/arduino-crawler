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
	virtual void init(SoundPlayer* soundPlayer, LightController* lightController, CrawlerStatus status);
	virtual void onStatusChanged(SoundPlayer* soundPlayer, LightController* lightController, CrawlerStatus status);
	virtual void onSpeedChanged(SoundPlayer* soundPlayer, LightController* lightController, uint8_t newSpeed, uint8_t oldSpeed);
};

class DefaultCrawlerBehaviourStrategy : public CrawlerBehaviourStrategy
{
};

class PoliceCrawlerBehaviourStrategy : public CrawlerBehaviourStrategy
{
public:
	void init(SoundPlayer* soundPlayer, LightController* lightController, CrawlerStatus status) override;
	void onStatusChanged(SoundPlayer* soundPlayer, LightController* lightController, CrawlerStatus status) override;
};

class CrawlerBehaviour
{
private:
	const DefaultCrawlerBehaviourStrategy _defaultStrategy;
	const PoliceCrawlerBehaviourStrategy _policeStrategy;

	SoundPlayer* const _soundPlayer;
	LightController* const _lightController;

	CrawlerBehaviourStrategy* getCurrentStrategy();
public:
	CrawlerBehaviour(SoundPlayer* soundPlayer, LightController* lightController);
	~CrawlerBehaviour();

	void switchTo(CrawlerBehaviourKind behaviourKind, CrawlerStatus status);

	void onStatusChanged(CrawlerStatus status);
	void onSpeedChanged(uint8_t newSpeed, uint8_t oldSpeed);
};

#endif
