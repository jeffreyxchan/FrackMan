#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include <string>
class StudentWorld;
class Actor : public GraphObject
{
public:
	Actor(int id, int x, int y, Direction dir, double size, unsigned int depth, 
		StudentWorld* studentWorldPointer);
	StudentWorld* studentWorld() const;
	bool isDead() const;
	void setDead();
	virtual bool canPassThroughMe();
	virtual bool canPickMeUp();
	virtual bool canBeAnnoyed();
	virtual bool canDigThroughDirt();
	virtual bool annoy(int amt);
	virtual bool addGold();
	bool moveToIfPossible(int x, int y);
	virtual void doSomething() = 0;
	virtual ~Actor() {}

private:
	bool m_dead;
	StudentWorld* m_studentWorld;
};

class Agent : public Actor
{
public:
	Agent(int id, int x, int y, Direction dir, double size, unsigned int depth,
		StudentWorld* studentWorldPointer, int health);
	int health() const;
	bool decHealth(int amt);
	int gold() const;
	virtual bool addGold();
	virtual bool decGold();
	virtual bool canPassThroughMe();
	virtual bool canPickMeUp();
	virtual bool canBeAnnoyed();
	virtual bool annoy(int amt);
	virtual void doSomething() = 0;
	virtual ~Agent();
private:
	int m_health; // how much health an Agent has
	int m_gold; // how much gold an Agent has
};

class FrackMan : public Agent
{
public:
	FrackMan(int x, int y, StudentWorld* studentWorldPointer);
	int squirts() const;
	bool replenishWater();
	int sonar() const;
	bool addSonar();
	virtual bool annoy(int amt);
	virtual bool canPickMeUp();
	virtual bool canBeAnnoyed();
	virtual bool canDigThroughDirt();
	virtual void doSomething();
	virtual ~FrackMan() {}
private:
	int m_squirts; // number of squirts
	int m_sonar; // count of how many sonar charges the sonar has left
};

class RegularProtester : public Agent
{
public:
	RegularProtester(int x, int y, StudentWorld* studentWorldPoiter);
	virtual bool annoy(int amt);
	virtual bool addGold();
	virtual bool choosePerpendicularDirection();
	virtual bool chooseNewDirection();
	virtual void lookForFrackMan();
	bool updateMaze(int maze[][60], int sx, int sy, int m);
	virtual void doSomething();
	virtual ~RegularProtester() {};
private:
	int m_state;
	int m_movementsLeft;
	int m_timeBetweenMoves;
	int m_timer;
	int m_stunCounter;
	int m_turningCounter;
	int m_resting;
	int m_exitMap[64][60];
	int m_FrackManMap[64][60];
};

class HardcoreProtester : public Agent
{
public:
	HardcoreProtester(int x, int y, StudentWorld* studentWorldPointer);
	virtual bool annoy(int amt);
	virtual bool addGold();
	virtual void doSomething();
	virtual ~HardcoreProtester() {}
private:
	int m_state;
	int m_movementsLeft;
	int m_timeBetweenMoves;
	int m_timer;
	int m_stunCounter;
	int m_turningCounter;
	int m_resting;
	int m_exitMap[64][60];
	int m_FrackManMap[64][60];
};

class Dirt : public Actor
{
public:
	Dirt(int x, int y, StudentWorld* studentWorldPointer);
	virtual void doSomething();
	virtual ~Dirt() {}
};

class Boulder : public Actor
{
public:
	Boulder(int x, int y, StudentWorld* studentWorldPointer);
	int state() const;
	virtual bool canPassThroughMe();
	virtual void doSomething();
	virtual ~Boulder() {}
private:
	int boulderState;
	int waitingCounter;
};

class Squirt : public Actor
{
public:
	Squirt(int x, int y, Direction dir, StudentWorld* studentWorldPointer);
	virtual void doSomething();
	virtual ~Squirt() {}
private:
	int m_travelDistance;
};

class ActivatingObject : public Actor
{
public:
	ActivatingObject(int id, int x, int y, Direction dir, double size, 
		unsigned int depth, StudentWorld* studentWorldPointer);
	void setVisibility(bool visibility);
	bool visibility() const;
	virtual bool canPassThroughMe();
	virtual void doSomething() = 0;
	virtual ~ActivatingObject();
private:
	bool m_visible;
};

class Barrel : public ActivatingObject
{
public:
	Barrel(int x, int y, StudentWorld* studentWorldPointer);
	virtual void doSomething();
	virtual ~Barrel() {}
};

class Gold : public ActivatingObject
{
public:
	Gold(int x, int y, StudentWorld* studentWorldPointer, int state);
	int state() const;
	virtual void doSomething();
	virtual ~Gold() {}
private:
	int m_state;
	int m_remainingTime;
};

class SonarKit : public ActivatingObject
{
public:
	SonarKit(int x, int y, StudentWorld* studentWorldPointer);
	virtual void doSomething();
	virtual ~SonarKit() {}
private:
	int m_state;
	int m_remainingTime;
};

class WaterPool : public ActivatingObject
{
public:
	WaterPool(int x, int y, StudentWorld* studentWorldPointer);
	virtual void doSomething();
	virtual ~WaterPool() {}
private:
	int m_state;
	int m_remainingTime;
};
#endif
