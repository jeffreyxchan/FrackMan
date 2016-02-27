#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GraphObject.h"
#include "GameWorld.h"
#include <string>
#include <vector>
class Actor;
class FrackMan;
class Dirt;

class StudentWorld : public GameWorld
{
public:
	StudentWorld(std::string assetDir);
	void setDisplayText();
	int randInt(int min, int max);
	FrackMan* theFrackMan() const;
	bool deleteDirt(int x, int y); // deletes dirt in a 4x4 square
	bool isThereDirt(int x, int y) const;
	void squirt();
	bool decBarrels();
	bool spawnGold();
	bool spawnProtester();
	Actor* findNearbyFrackMan(Actor* ptr, int radius);
	Actor* findNearbyActor(int x, int y, int radius);
	void revealAllNearbyObjects(int x, int y, int radius);
	bool canActorMoveTo(Actor* ptr, int x, int y);
	virtual int init();
	virtual int move();
	virtual void cleanUp();
	virtual ~StudentWorld();

private:
	FrackMan* m_FrackMan;
	Dirt* m_dirt[64][60];
	int m_barrels; // number of oil barrels left in the game
	int m_ticksUntilNextProtester;
	int m_waitingTime;
	int m_nProtesters;
	std::vector<Actor*> m_actors;
};

#endif
