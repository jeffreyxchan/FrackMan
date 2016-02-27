#include "Actor.h"
#include "StudentWorld.h"
#include "GraphObject.h"
#include <string>
#include <algorithm>
#include <math.h>
#include <queue>
using namespace std;

// global ints used for state of objects
const int STABLE = 0;
const int WAITING = 1;
const int FALLING = 2;
const int PERMANENT = 3;
const int TEMPORARY = 4;
const int STAYING = 5;
const int LEAVING = 6;
const int STUNNED = 7;
const int RESTING = 8;

class Coordinate
{
public:
	Coordinate(int x, int y) { m_x = x; m_y = y; }
	int x() { return m_x; }
	int y() { return m_y; }
private:
	int m_x;
	int m_y;
};



Actor::Actor(int id, int x, int y, Direction dir, double size, unsigned int depth,
	StudentWorld* studentWorldPointer)
	: GraphObject(id, x, y, dir, size, depth)
{
	setVisible(true); // all actors initially visible unless otherwise stated
	m_dead = false; // all actors initially alive
	m_studentWorld = studentWorldPointer; // all actors have a sw pointer
}

StudentWorld* Actor::studentWorld() const
{
	return m_studentWorld;
}

bool Actor::isDead() const
{
	return m_dead;
}

void Actor::setDead()
{
	m_dead = true;
}

bool Actor::canPassThroughMe()
{
	return true; // all actors default to true
}

bool Actor::canPickMeUp()
{
	return false;
}

bool Actor::canBeAnnoyed()
{
	return false;
}

bool Actor::canDigThroughDirt()
{
	return false;
}

bool Actor::annoy(int amt)
{
	return true;
}

bool Actor::addGold()
{
	return true;
}

bool Actor::moveToIfPossible(int x, int y)
{
	if (studentWorld()->canActorMoveTo(this, x, y))
	{
		moveTo(x, y);
		return true;
	}
	else
		return false;
}

Agent::Agent(int id, int x, int y, Direction dir, double size, unsigned int depth,
	StudentWorld* studentWorldPointer, int hp)
	: Actor(id, x, y, dir, size, depth, studentWorldPointer)
{
	m_health = hp;
	m_gold = 0; // initially starts off with no gold
}

int Agent::health() const
{
	return m_health;
}

bool Agent::decHealth(int amt)
{
	if (m_health > 0) // while the agent still has health
	{
		if (m_health < amt) // if current health is less than the damage you want to inflict
			m_health = 0; // decrement health to 0
		else
			m_health -= amt; // m_health = m_heatlh - amt;
		return true;
	}
	return false;
}

int Agent::gold() const
{
	return m_gold;
}

bool Agent::addGold()
{
	m_gold++;
	return true;
}

bool Agent::decGold()
{
	if (m_gold > 0)
	{
		m_gold--;
		return true;
	}
	return false;
}

bool Agent::canPassThroughMe()
{
	return true;
}

bool Agent::canPickMeUp()
{
	return true;
}

bool Agent::canBeAnnoyed()
{
	return true;
}


bool Agent::annoy(int amt)
{
	decHealth(amt);
	if (health() == 0) // if he died from being annoyed
	{
		setDead();
	};
	return true;
}

Agent::~Agent() {}

FrackMan::FrackMan(int x, int y, StudentWorld* studentWorldPointer)
	: Agent(IID_PLAYER, x, y, right, 1, 0, studentWorldPointer, 10)
{
	m_squirts = 5; // initially given 5 squirtsp
	m_sonar = 1; // initially starts off with one sonar charge
}

int FrackMan::squirts() const
{
	return m_squirts; // returns the number of squirts he has left
}

bool FrackMan::replenishWater()
{
	m_squirts = 5; // replenish to 5 squirts
	return true;
}

int FrackMan::sonar() const
{
	return m_sonar;
}

bool FrackMan::addSonar()
{
	m_sonar++;
	return true;
}

bool FrackMan::annoy(int amt)
{
	decHealth(amt);
	if (health() == 0) // if he died from being annoyed
	{
		studentWorld()->playSound(SOUND_PLAYER_GIVE_UP);
		setDead();
	}
	else
		studentWorld()->playSound(SOUND_PLAYER_ANNOYED);
	return true;
}

bool FrackMan::canPickMeUp()
{
	return false; // the FrackMan, as an agent, can not pick up temporary gold
}

bool FrackMan::canBeAnnoyed()
{
	return false;
}

bool FrackMan::canDigThroughDirt()
{
	return true;
}

void FrackMan::doSomething()
{
	if (health() == 0 || isDead()) // if the FrackMan is dead, return immediately
	{
		setDead();
		return;
	}
	int ch; // space in memory to hold the key that was pressed
	if (studentWorld()->getKey(ch) == true) // if the game world received a key input
	{
		switch (ch) // respond to the key input accordingly
		{
		case KEY_PRESS_RIGHT:
			if (getDirection() != right)
				setDirection(right);
			else if (studentWorld()->canActorMoveTo(this, getX() + 1, getY()))
				moveTo(getX() + 1, getY());
			else
				moveTo(getX(), getY()); // else move in place
			break;
		case KEY_PRESS_LEFT:
			if (getDirection() != left)
				setDirection(left);
			else if (studentWorld()->canActorMoveTo(this, getX() - 1, getY()))
				moveTo(getX() - 1, getY());
			else
				moveTo(getX(), getY());
			break;
		case KEY_PRESS_DOWN:
			if (getDirection() != down)
				setDirection(down);
			else if (studentWorld()->canActorMoveTo(this, getX(), getY() - 1))
				moveTo(getX(), getY() - 1);
			else
				moveTo(getX(), getY() - 1);
			break;
		case KEY_PRESS_UP:
			if (getDirection() != up)
				setDirection(up);
			else if (studentWorld()->canActorMoveTo(this, getX(), getY() + 1)) // handles upper boundary behavior
				moveTo(getX(), getY() + 1);
			else
				moveTo(getX(), getY());
			break;
		case KEY_PRESS_ESCAPE:
			setDead(); // kills the FrackMan
			return;
			break;
		case KEY_PRESS_SPACE:
			if (m_squirts > 0) // if he still has squirts left
			{
				m_squirts--; // decrement the number of squirts he has
				studentWorld()->squirt();
			}
			break;
		case KEY_PRESS_TAB: // drops gold at the FrackMan's location
			if (gold() > 0)
			{
				decGold();
				studentWorld()->spawnGold();
			}
			break;
		case 'z':
		case 'Z':
			// TODO: implement calling sonar stuff
			if (m_sonar > 0)
			{
				m_sonar--;
				studentWorld()->playSound(SOUND_SONAR);
				studentWorld()->revealAllNearbyObjects(getX(), getY(), 12);
			}
			break;
		default:
			moveTo(getX(), getY()); // animates movement in place with any other key press
			break;
		}
		if (studentWorld()->deleteDirt(getX(), getY())) // remove dirt around FrackMan
			studentWorld()->playSound(SOUND_DIG); // play the digging sound
	}
	return;
}

RegularProtester::RegularProtester(int x, int y, StudentWorld* studentWorldPointer)
	: Agent(IID_PROTESTER, x, y, left, 1, 0, studentWorldPointer, 5)
{
	m_state = STAYING; // all regular Protesters are in the NOT LEAVING state
	m_movementsLeft = studentWorldPointer->randInt(8, 60);
	int level = studentWorldPointer->getLevel();
	m_timeBetweenMoves = max(0, 3 - (level / 4));
	m_timer = 0;
	m_stunCounter = 0;
	m_resting = 0;
	m_turningCounter = 0;
}

bool RegularProtester::annoy(int amt)
{
	decHealth(amt);
	if (health() == 0 && m_state != LEAVING) // if he died from being annoyed
	{
		studentWorld()->playSound(SOUND_PROTESTER_GIVE_UP);
		m_state = LEAVING;
		if (amt == 2)
			studentWorld()->increaseScore(100); // if death by squirt, increase by 100
		else if (amt > 2 || amt == 10)
			studentWorld()->increaseScore(500); // if death by boulder, increase by 500
	}
	else if (m_state == STAYING || m_state == STUNNED)
	{
		studentWorld()->playSound(SOUND_PROTESTER_ANNOYED);
		m_state = STUNNED;
		int level = studentWorld()->getLevel();
		int N = max(50, 100 - (level * 10));
		m_stunCounter = N; // intialize stun counter
	}
	return true;
}

bool RegularProtester::addGold()
{
	m_state = LEAVING; // immediately set it to a leaving state
	studentWorld()->playSound(SOUND_PROTESTER_FOUND_GOLD); // play sound
	studentWorld()->increaseScore(25); // increase score by 25 for bribery
	return true;
}

bool RegularProtester::updateMaze(int maze[][60], int sx, int sy, int m)
{
	queue<Coordinate> coordQueue;
	Coordinate start(sx, sy); // create starting coordinate
	coordQueue.push(start);
	maze[sx][sy] = m; // assign first breadcrumb
	return true;
}

void RegularProtester::lookForFrackMan()
{
	bool foundFrackMan = true;
	int FMX = studentWorld()->theFrackMan()->getX();
	int FMY = studentWorld()->theFrackMan()->getY();
	if (getX() == FMX)
	{
		// let's look for FrackMan next to us
		int differenceInX = FMX - getX();
		if (differenceInX > 0) // FM is to the right of the right of the protestor
		{
			for (int k = 0; k < differenceInX; k++)
				if (!studentWorld()->canActorMoveTo(this, getX() + k, getY()))
					foundFrackMan = false;
			if (foundFrackMan && getDirection() == right)
				moveToIfPossible(getX() + 1, getY());
			else
				setDirection(right);
		}
		else
		{
			for (int k = 0; k < (differenceInX * -1); k++)
				if (!studentWorld()->canActorMoveTo(this, getX() - k, getY()))
					foundFrackMan = false;
			if (foundFrackMan && getDirection() == left)
				moveToIfPossible(getX() - 1, getY());
			else
				setDirection(left);
		}
	}
	if (getY() == FMY)
	{
		// let's look for FrackMan above us
		int differenceInY = FMY - getY();
		if (differenceInY > 0) // FM is on top of the protester
		{
			for (int k = 0; k < differenceInY; k++)
				if (!studentWorld()->canActorMoveTo(this, getX(), getY() + k))
					foundFrackMan = false;
			if (foundFrackMan && getDirection() == up)
				setDirection(up);
		}
		else
		{
			for (int k = 0; k < (differenceInY * -1); k++)
				if (!studentWorld()->canActorMoveTo(this, getX(), getY() - k))
					foundFrackMan = false;
			if (foundFrackMan && getDirection() == down)
				moveToIfPossible(getX(), getY() - 1);
			else
				setDirection(down);
		}
	}
}

bool RegularProtester::choosePerpendicularDirection()
{
	bool ready = true;
	switch (getDirection())
	{
	case up: case down:
		// check right first
		ready = true;
		if (!studentWorld()->canActorMoveTo(this, getX() + 1, getY()))
			ready = false;
		if (ready)
		{
			setDirection(right); // turn right
			m_movementsLeft = studentWorld()->randInt(8, 60); // set some steps to move forward
			return true;
		}
		// check left now
		ready = true;
		if (!studentWorld()->canActorMoveTo(this, getX() - 1, getY()))
			ready = false;
		if (ready)
		{
			setDirection(left); // turn left
			m_movementsLeft = studentWorld()->randInt(8, 60);
			return true;
		}
		break;
	case right: case left:
		// check up first
		ready = true;
		if (!studentWorld()->canActorMoveTo(this, getX(), getY() + 1))
			ready = false;
		if (ready)
		{
			setDirection(up);
			m_movementsLeft = studentWorld()->randInt(8, 60);
			return true;
		}
		// check down now
		ready = true;
		if (!studentWorld()->canActorMoveTo(this, getX(), getY() - 1))
			ready = false;
		if (ready)
		{
			setDirection(down);
			m_movementsLeft = studentWorld()->randInt(8, 60);
			return true;
		}
		break;
	}
	return false;
}

bool RegularProtester::chooseNewDirection()
{
	bool notReady = true;
	while (notReady)
	{
		notReady = false;
		int compass = studentWorld()->randInt(1, 4);
		switch (compass)
		{
		case 1: // UP
			if (!studentWorld()->canActorMoveTo(this, getX(), getY() + 1))
				notReady = true;
			else
			{
				setDirection(up);
				return true;
			}
			break;
		case 2: // DOWN
			if (!studentWorld()->canActorMoveTo(this, getX(), getY() - 1))
				notReady = true;
			else
			{
				setDirection(down);
				return true;
			}
			break;
		case 3: // RIGHT
			if (!studentWorld()->canActorMoveTo(this, getX() + 1, getY()))
				notReady = true;
			else
			{
				setDirection(right);
				return true;
			}
			break;
		case 4: // LEFT
			if (!studentWorld()->canActorMoveTo(this, getX() - 1, getY()))
				notReady = true;
			else
			{
				setDirection(left);
				return true;
			}
			break;
		}
	}
	return true;
}

void RegularProtester::doSomething()
{
	if (isDead()) // if the RegularProtester is dead
		return; // return immediately
	if (m_turningCounter > 200) // check to see if it can turn
	{
		if (choosePerpendicularDirection()) // if it successfully chooses a new direction
		{
			m_turningCounter = 0; // reset the turning counter
			return; // return immediately
		}
	}
	else
		m_turningCounter++; // if it can't turn, increment turning counter
	if (m_movementsLeft == 0) // if it can make no further movements
	{
		m_movementsLeft = studentWorld()->randInt(8, 60); // generate new number of steps
		chooseNewDirection(); // choose a new direction to move in
		return;
	}
	Actor* ptr = studentWorld()->findNearbyFrackMan(this, 4); // keep track of FrackMan
	switch (m_state)
	{
	case RESTING:
		if (m_resting > 0)
		{
			m_resting--;
			return;
		}
		else
			m_state = STAYING;
		break;
	case STAYING:
		if (ptr != nullptr)
		{
			studentWorld()->playSound(SOUND_PROTESTER_YELL);
			studentWorld()->theFrackMan()->annoy(2);
			m_state = RESTING;
			m_resting = 15;
			return;
		}
		if (m_timer == m_timeBetweenMoves)
		{
			m_timer = 0;
			switch (getDirection())
			{
			case up:
				if (!moveToIfPossible(getX(), getY() + 1))
					m_movementsLeft = 0; // stop and change direction
				break;
			case down:
				if (!moveToIfPossible(getX(), getY() - 1))
					m_movementsLeft = 0;
				break;
			case right:
				if (!moveToIfPossible(getX() + 1, getY()))
					m_movementsLeft = 0;
				break;
			case left:
				if (!moveToIfPossible(getX() - 1, getY()))
					m_movementsLeft = 0;
				break;
			}
			m_movementsLeft--;
		}
		else
		{
			m_timer++;
			return;
		}
		break;
	case STUNNED:
		if (m_stunCounter == 0 && health() > 0)
			m_state = STAYING; // if still alive and run out of stun time, return to normal moving state
		else if (m_stunCounter > 0)
			m_stunCounter--; // decrement stunCounter
		else if (health() == 0)
		{
			m_state = LEAVING;
			return;
		}
		break;
	case LEAVING:
		m_resting = 0;
		if (getX() == 60 && getY() == 60)
		{
			setDead();
			return;
		}
		if (m_timer == m_timeBetweenMoves)
		{
			m_timer = 0;
			moveTo(getX() + 1, getY());
		}
		else
		{
			m_timer++;
			return;
		}
		break;
	}
}

HardcoreProtester::HardcoreProtester(int x, int y, StudentWorld* studentWorldPointer)
	: Agent(IID_HARD_CORE_PROTESTER, x, y, left, 1, 0, studentWorldPointer, 20)
{
	m_state = STAYING; // all hardcore Protesters are in the NOT LEAVING state
	m_movementsLeft = studentWorldPointer->randInt(8, 60);
	int level = studentWorldPointer->getLevel();
	m_timeBetweenMoves = max(0, 3 - (level / 4));
	m_timer = 0;
}

bool HardcoreProtester::annoy(int amt)
{
	decHealth(amt);
	if (health() == 0 && m_state != LEAVING) // if he died from being annoyed
	{
		studentWorld()->playSound(SOUND_PROTESTER_GIVE_UP);
		m_state = LEAVING;
	}
	else if (m_state == STAYING || m_state == STUNNED)
	{
		studentWorld()->playSound(SOUND_PROTESTER_ANNOYED);
		m_state = STUNNED;
	}
	return true;
}

bool HardcoreProtester::addGold()
{
	m_state = LEAVING;
	studentWorld()->playSound(SOUND_PROTESTER_FOUND_GOLD);
	return true;
}

void HardcoreProtester::doSomething()
{
	if (isDead()) // if the RegularProtester is dead
		return; // return immediately
	if (m_timer < m_timeBetweenMoves)
		m_timer++;
	else if (m_timer == m_timeBetweenMoves && m_state == STAYING)
	{
		m_timer = 0; // reset the Protester's timer
					 // implement movement swich statement
		moveToIfPossible(getX() - 1, getY());
	}
	else if (health() == 0)
		moveTo(getX() + 1, getY());
}

Dirt::Dirt(int x, int y, StudentWorld* studentWorldPointer)
	: Actor(IID_DIRT, x, y, right, 0.25, 3, studentWorldPointer)
{}

void Dirt::doSomething()
{}

Boulder::Boulder(int x, int y, StudentWorld* studentWorldPointer)
	: Actor(IID_BOULDER, x, y, down, 1, 1, studentWorldPointer)
{
	boulderState = STABLE; // initially starts off as stable?
	waitingCounter = 0; // initializes the counter to 0 ticks
	studentWorldPointer->deleteDirt(getX(), getY()); // delete dirt around boulder
}

int Boulder::state() const
{
	return boulderState; // returns the boulder's state
}

bool Boulder::canPassThroughMe()
{
	return false;
}

void Boulder::doSomething()
{
	if (isDead()) // if the boulder is dead
		return; // return immediately

	switch (boulderState)
	{
	case STABLE:
		// check to see if there's any dirt below in the 4x
		if (studentWorld()->canActorMoveTo(this, getX(), getY() - 1))
			boulderState = WAITING; // set to waiting state
		break;
	case WAITING:
		waitingCounter++; // increment waitingCounter
		if (waitingCounter > 30) // if it's been in waiting state for 30 ticks
		{
			waitingCounter = 0; // reset waitingCounter
			boulderState = FALLING; // transition to falling state
			studentWorld()->playSound(SOUND_FALLING_ROCK); // play falling rock sound
		}
		break;
	case FALLING:
		if (studentWorld()->findNearbyFrackMan(this, 3) != nullptr)
			studentWorld()->theFrackMan()->annoy(10); // annoy them to death
		if (!moveToIfPossible(getX(), getY() - 1))
			setDead();
		break;
	}
}

Squirt::Squirt(int x, int y, Direction dir, StudentWorld* studentWorldPointer)
	: Actor(IID_WATER_SPURT, x, y, dir, 1, 1, studentWorldPointer)
{
	m_travelDistance = 4; // initialize travel distance to 4
}

void Squirt::doSomething()
{
	if (m_travelDistance > 0)
	{
		switch (getDirection())
		{
		case up:
			if (getY() < 60)
			{
				bool canIMove = true;
				for (int k = getX(); k < 64 && k < getX() + 4; k++)
				{
					if (studentWorld()->isThereDirt(k, getY() + 1))
						canIMove = false;
				}
				if (canIMove && studentWorld()->canActorMoveTo(this, getX(), getY() + 1))
					moveTo(getX(), getY() + 1);
				else
					setDead();
				if (studentWorld()->findNearbyActor(getX(), getY(), 3) != nullptr && studentWorld()->findNearbyActor(getX(), getY(), 3)->canBeAnnoyed())
				{
					studentWorld()->findNearbyActor(getX(), getY(), 3)->annoy(2);
					setDead();
				}
			}
			else
				setDead(); // else kill it
			break;
		case down:
			if (getY() > 0)
			{
				bool canIMove = true;
				for (int k = getX(); k < 64 && k < getX() + 4; k++)
				{
					if (studentWorld()->isThereDirt(k, getY() - 1))
						canIMove = false;
				}
				if (canIMove && studentWorld()->canActorMoveTo(this, getX(), getY() - 1))
					moveTo(getX(), getY() - 1);
				else
					setDead();
				if (studentWorld()->findNearbyActor(getX(), getY(), 3) != nullptr && studentWorld()->findNearbyActor(getX(), getY(), 3)->canBeAnnoyed())
				{
					studentWorld()->findNearbyActor(getX(), getY(), 3)->annoy(2);
					setDead();
				}
			}
			else
				setDead();
			break;
		case right:
			if (getX() < 60)
			{
				bool canIMove = true;
				for (int k = getY(); k < 60 && k < getY() + 4; k++)
				{
					if (studentWorld()->isThereDirt(getX() + 1, k))
						canIMove = false;
				}
				if (canIMove && studentWorld()->canActorMoveTo(this, getX() + 1, getY()))
					moveTo(getX() + 1, getY());
				else
					setDead();
				if (studentWorld()->findNearbyActor(getX(), getY(), 3) != nullptr && studentWorld()->findNearbyActor(getX(), getY(), 3)->canBeAnnoyed())
				{
					studentWorld()->findNearbyActor(getX(), getY(), 3)->annoy(2);
					setDead();
				}
			}
			else
				setDead();
			break;
		case left:
			if (getX() > 0)
			{
				bool canIMove = true;
				for (int k = getY(); k < 60 && k < getY() + 4; k++)
				{
					if (studentWorld()->isThereDirt(getX() - 1, k))
						canIMove = false;
				}
				if (canIMove && studentWorld()->canActorMoveTo(this, getX() - 1, getY()))
					moveTo(getX() - 1, getY());
				else
					setDead();
				if (studentWorld()->findNearbyActor(getX(), getY(), 3) != nullptr && studentWorld()->findNearbyActor(getX(), getY(), 3)->canBeAnnoyed())
				{
					studentWorld()->findNearbyActor(getX(), getY(), 3)->annoy(2);
					setDead();
				}
			}
			else
				setDead();
			break;
		}
		m_travelDistance--;
	}
	else
		setDead();
	return;
}

ActivatingObject::ActivatingObject(int id, int x, int y, Direction dir, double size, unsigned int depth,
	StudentWorld* studentWorldPointer)
	: Actor(id, x, y, dir, size, depth, studentWorldPointer)
{
	m_visible = false;
}

void ActivatingObject::setVisibility(bool visibility)
{
	m_visible = visibility;
}

bool ActivatingObject::visibility() const
{
	return m_visible;
}

bool ActivatingObject::canPassThroughMe()
{
	return true;
}

ActivatingObject::~ActivatingObject() {}

Barrel::Barrel(int x, int y, StudentWorld* studentWorldPointer)
	: ActivatingObject(IID_BARREL, x, y, right, 1, 2, studentWorldPointer)
{
	setVisible(false);
}

void Barrel::doSomething()
{
	if (isDead())
		return; // if dead, return immediately

	// what to do if the FrackMan is nearby
	if (!visibility() && studentWorld()->findNearbyFrackMan(this, 4) != nullptr) // if within r = 4
	{
		setVisible(true); // reveal the barrel
		setVisibility(true);
		return; // return immediately
	}
	else if (visibility() && studentWorld()->findNearbyFrackMan(this, 3) != nullptr) // if within r = 3
	{
		setDead(); // set itself to dead
		studentWorld()->playSound(SOUND_FOUND_OIL); // play the found oil sound
		studentWorld()->increaseScore(1000); // increase score by 1000
		studentWorld()->decBarrels(); // dec number of barrels within studentWorld
	}
}

Gold::Gold(int x, int y, StudentWorld* studentWorldPointer, int state)
	: ActivatingObject(IID_GOLD, x, y, right, 1, 2, studentWorldPointer)
{
	if (state == PERMANENT)
	{
		setVisible(false); //NOTE, WILL NEED TO SET INVISIBLE BY THE TIME YOU TURN THIS IN
		m_state = state;
	}
	else if (state == TEMPORARY) // means that the FrackMan dropped it to excite a Protester
	{
		setVisible(true);
		m_state = state; // all gold objects start off as only pick-up-able by the FrackMan
	}
	m_remainingTime = 100; // lifetime of 100 ticks
}

int Gold::state() const
{
	return m_state;
}

void Gold::doSomething()
{
	if (isDead()) // if it's dead
		return; // return immediately
	// what to do if the FrackMan is nearby
	if (m_state == PERMANENT && !visibility() && studentWorld()->findNearbyFrackMan(this, 4) != nullptr)
	{
		setVisible(true); // reveal the barrel
		setVisibility(true);
		return; // return immediately
	}
	else if (m_state == PERMANENT && visibility() && studentWorld()->findNearbyFrackMan(this, 3) != nullptr)
	{
		setDead(); // kill itself
		studentWorld()->playSound(SOUND_GOT_GOODIE);
		studentWorld()->increaseScore(10);
		studentWorld()->theFrackMan()->addGold();
	}
	else if (m_state == TEMPORARY && studentWorld()->findNearbyActor(getX(), getY(), 3) != nullptr && studentWorld()->findNearbyActor(getX(), getY(), 3)->canPickMeUp())
	{
		setDead(); // kill itself
		// call function that tells a Protester that they got bribed
		studentWorld()->findNearbyActor(getX(), getY(), 3)->addGold();
	}
	else if (m_state == TEMPORARY)
	{
		if (m_remainingTime > 0)
			m_remainingTime--; // decrement remainingTime
		else if (m_remainingTime == 0)
		{
			setDead();
			return;
		}
	}
}

SonarKit::SonarKit(int x, int y, StudentWorld* studentWorldPointer)
	: ActivatingObject(IID_SONAR, x, y, right, 1, 2, studentWorldPointer)
{
	// sonar kits are only pickup-able by the FrackMan
	m_state = TEMPORARY; // all sonar kits start out in a temporary state
	setVisible(true); // all sonar kits start off as visible
	setVisibility(true); // set it as "visible"
	int T = 300 - (10 * (studentWorldPointer->getLevel()));
	if (T > 100)
		m_remainingTime = T;
	else
		m_remainingTime = 100;
}

void SonarKit::doSomething()
{
	if (isDead()) // if it's dead
		return; // return immediately
	if (studentWorld()->findNearbyFrackMan(this, 3) != nullptr) // if Frackman is within r = 3
	{
		setDead(); // kill itself
		studentWorld()->playSound(SOUND_GOT_GOODIE); // play goodie sound
		studentWorld()->theFrackMan()->addSonar(); // increase sonar by 1
		studentWorld()->theFrackMan()->addSonar(); // he gets 2 for picking 1 up....
		studentWorld()->increaseScore(75); // increase score by 75 points
	}
	if (m_remainingTime > 0) // if it still has time to be alive
		m_remainingTime--; // decrement remainingTime
	if (m_remainingTime == 0)
		setDead();
	// TODO: ask question about tick lifetimes
}

WaterPool::WaterPool(int x, int y, StudentWorld* studentWorldPointer)
	: ActivatingObject(IID_WATER_POOL, x, y, right, 1, 2, studentWorldPointer)
{
	m_state = TEMPORARY; // all water pools are temporary and only last a fixed time
	setVisible(true); // all water pools start off as visible
	setVisibility(true); // set is as "visible"
	int T = 300 - (10 * (studentWorldPointer->getLevel()));
	if (T > 100)
		m_remainingTime = T;
	else
		m_remainingTime = 100;
}

void WaterPool::doSomething()
{
	if (isDead())
		return; // if dead, return immediately
	if (studentWorld()->findNearbyFrackMan(this, 3) != nullptr) // if Frackman is within r = 3
	{
		setDead(); // kill itself
		studentWorld()->playSound(SOUND_GOT_GOODIE); // play goodie sound
		studentWorld()->theFrackMan()->replenishWater(); // increase sonar by 1
		studentWorld()->increaseScore(100); // increase score by 75 points
	}
	if (m_remainingTime > 0) // if it still has time to be alive
		m_remainingTime--; // decrement remainingTime
	if (m_remainingTime == 0)
		setDead();
}