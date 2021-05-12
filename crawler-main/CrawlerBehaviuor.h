// CrawlerBehaviuor.h

#ifndef _CRAWLERBEHAVIUOR_h
#define _CRAWLERBEHAVIUOR_h

#include "SoundPlayer.h"

enum class CrawlerBehaviourKind
{
	Default,
	Police
};

class CrawlerBehaviourStrategy
{
};

class DefaultCrawlerBehaviourStrategy : public CrawlerBehaviourStrategy
{

};

class PoliceCrawlerBehaviuorStrategy : public CrawlerBehaviourStrategy 
{

};

enum class CrawlerStatus : uint8_t;

class CrawlerBehaviour
{
private:
	const DefaultCrawlerBehaviourStrategy _defaultStrategy;
	const PoliceCrawlerBehaviuorStrategy _policeStrategy;
	SoundPlayer* const _soundPlayer;
public:
	CrawlerBehaviour(SoundPlayer* soundPlayer);
	~CrawlerBehaviour();

	void init();
	void onStart();
	void onStatusChanged(CrawlerStatus newStatus);
	void onSpeedChanged(uint8_t newSpeed);
};

#endif
