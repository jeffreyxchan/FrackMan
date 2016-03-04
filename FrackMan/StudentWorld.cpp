#include "StudentWorld.h"
#include "GameWorld.h"
#include "GraphObject.h"
#include "Actor.h"
#include <string>
#include <algorithm>
#include <sstream>
#include <random>
#include <math.h>
using namespace std;

const int STABLE = 0;
const int WAITING = 1;
const int FALLING = 2;
const int PERMANENT = 3;
const int TEMPORARY = 4;

int StudentWorld::randInt(int min, int max)
{
	if (max < min)
		swap(max, min);
	static random_device rd;
	static mt19937 generator(rd());
	uniform_int_distribution<> distro(min, max);
	return distro(generator);

}

string numberToString(int Number, int id)
{
	ostringstream ss;
	ss << Number;
	string returnString = ss.str();
	switch (id)
	{
	case 0: // dealing with score
		while (returnString.size() != 6) // while it's not full size
			returnString = "0" + returnString; // add a 0 to the beginning
		break;
	case 1: // dealing with anything that needs to be size 2
		while (returnString.size() != 2)
			returnString = " " + returnString;
		break;
	case 2: // dealing with lives
		break;
	case 3: // dealing with health percentage
		while (returnString.size() != 3)
			returnString = " " + returnString;
		break;
	}
	return returnString;
}

string statusFormatter(int& score, int& level, int& lives,
	int& health, int& squirt, int& gold, int& sonar,
	int& barrelsLeft) // formats text for setGameStatText
{
	string sscore = numberToString(score, 0);
	string slevel = numberToString(level, 1);
	string slives = numberToString(lives, 2);
	string shealth = numberToString(health, 3);
	string ssquirt = numberToString(squirt, 1);
	string sgold = numberToString(gold, 1);
	string ssonar = numberToString(sonar, 1);
	string sbarrels = numberToString(barrelsLeft, 3);
	string s = "Scr: " + sscore + "  Lvl: " + slevel + "  Lives: " + slives +
		"  Hlth: " + shealth + "%  Wtr: " + ssquirt + "  Gld: " + sgold +
		"  Sonar: " + ssonar + "  Oil Left:	" + sbarrels;
	return s;
}

GameWorld* createStudentWorld(string assetDir) // came with skeleton
{
	return new StudentWorld(assetDir);
}

StudentWorld::StudentWorld(std::string assetDir)
	: GameWorld(assetDir)
{
	for (int x = 0; x < 64; x++)
		for (int y = 0; y < 60; y++)
			m_dirt[x][y] = nullptr;
	m_FrackMan = nullptr;
	m_barrels = 0;
	m_ticksUntilNextProtester = 0;
	m_waitingTime = 0;
	m_nProtesters = 0; // initially 0 Protesters
}

void StudentWorld::setDisplayText() // displays status bar
{
	int score = getScore(); // get the score from the game world
	int level = getLevel(); // get the level from the game world
	int lives = getLives(); // get the number of lives left from the game world
	int health = (m_FrackMan->health()) * 10; // get FrackMan's health
	int squirt = m_FrackMan->squirts(); // get FrackMan's number of squirts left
	int gold = m_FrackMan->gold(); // get the amount of gold FrackMan is holding
	int sonar = m_FrackMan->sonar(); // get the number of charges FrackMan has
	int barrelsLeft = m_barrels; // get the number of barrels left in the game

								 // display formatted text
	setGameStatText(statusFormatter(score, level, lives, health,
		squirt, gold, sonar, barrelsLeft)); // provided function
}

FrackMan* StudentWorld::theFrackMan() const
{
	return m_FrackMan;
}

bool StudentWorld::deleteDirt(int x, int y)
{
	bool wasDirtDeleted = false;
	for (int k = x; k < 64 && k < x + 4; k++)
		for (int m = y; m < 60 && m < y + 4; m++)
			if (m_dirt[k][m] != nullptr) // if you have a dirt to delete
			{
				delete m_dirt[k][m]; // delete dirt object at specified location
				m_dirt[k][m] = nullptr; // set it to nullptr to avoid blowing chunk
				wasDirtDeleted = true;
			}
	return wasDirtDeleted;
}

bool StudentWorld::isThereDirt(int x, int y) const
{
	if (m_dirt[x][y] != nullptr) // if there's not dirt at the coordinate
		return true;
	return false; // if there is, return false;
}

void StudentWorld::squirt()
{
	int FMX = m_FrackMan->getX();
	int FMY = m_FrackMan->getY();
	switch (m_FrackMan->getDirection())
	{
	case GraphObject::Direction::up:
		if (FMY + 4 < 61)
		{
			bool canYouSpawn = true;
			for (int k = FMX; k < 64 && k < FMX + 4; k++)
				for (int m = FMY + 4; m < 60 && m < FMY + 8; m++)
				{
					if (isThereDirt(k, m))
						canYouSpawn = false;
				}
			if (!canYouSpawn)
				break;
			m_actors.push_back(new Squirt(FMX, FMY + 4, GraphObject::Direction::up, this));
		}
		break;
	case GraphObject::Direction::down:
		if (FMY > 3)
		{
			bool canYouSpawn = true;
			for (int k = FMX; k < 64 && k < FMX + 4; k++)
				for (int m = FMY - 4; m < 60 && m < FMY; m++)
				{
					if (isThereDirt(k, m))
						canYouSpawn = false;
				}
			if (!canYouSpawn)
				break;
			m_actors.push_back(new Squirt(FMX, FMY - 4, GraphObject::Direction::down, this));
		}
		break;
	case GraphObject::Direction::left:
		if (FMX > 3)
		{
			bool canYouSpawn = true;
			for (int k = FMX - 4; k < 64 && k < FMX; k++)
				for (int m = FMY; m < 60 && m < FMY + 4; m++)
				{
					if (isThereDirt(k, m))
						canYouSpawn = false;
				}
			if (!canYouSpawn)
				break;
			m_actors.push_back(new Squirt(FMX - 4, FMY, GraphObject::Direction::left, this));
		}
		break;
	case GraphObject::Direction::right:
		if (FMX < 57)
		{
			bool canYouSpawn = true;
			for (int k = FMX + 4; k < 64 && k < FMX + 8; k++)
				for (int m = FMY; m < 60 && m < FMY + 4; m++)
				{
					if (isThereDirt(k, m))
						canYouSpawn = false;
				}
			if (!canYouSpawn)
				break;
			m_actors.push_back(new Squirt(FMX + 4, FMY, GraphObject::Direction::right, this));
		}
		break;
	}
}

bool StudentWorld::decBarrels()
{
	if (m_barrels > 0) // only if there are more than 0 barrels
	{
		m_barrels--;
		return true;
	}
	return false;
}

bool StudentWorld::spawnGold()
{
	m_actors.push_back(new Gold(m_FrackMan->getX(), m_FrackMan->getY(), this, TEMPORARY));
	return true;
}

bool StudentWorld::spawnProtester()
{
	int currentLevel = getLevel();
	int myNumber = 2 + (currentLevel * 1.5);
	int targetAmount = min(15, myNumber);
	if (m_nProtesters < targetAmount)
	{
		int probabilityNumber = (getLevel() * 10) + 30;
		int probabilityOfHardcore = min(90, probabilityNumber);
		int RNG = randInt(1, probabilityOfHardcore);
		// UNCOMMENT THIS PART WHEN YOU IMPLEMENT HARDCORE PROTESTORS
		//if (RNG == 2)
		//	m_actors.push_back(new HardcoreProtester(60, 60, this));
		//else
			m_actors.push_back(new RegularProtester(60, 60, this));
		m_nProtesters++;
		return true;
	}
	return false;
}

Actor* StudentWorld::findNearbyFrackMan(Actor* ptr, int radius)
{
	int differenceInX = ptr->getX() - m_FrackMan->getX();
	if (differenceInX < 0)
		differenceInX *= -1;
	int differenceInY = ptr->getY() - m_FrackMan->getY();
	if (differenceInY < 0)
		differenceInY *= -1;
	double distanceInBetween = pow(pow(differenceInX, 2) + pow(differenceInY, 2), 0.5);
	if (distanceInBetween <= radius)
		return m_FrackMan;
	return nullptr;
}

Actor* StudentWorld::findNearbyActor(int x, int y, int radius)
{
	// give it coordinates and it'll search for any actor that's within a radius away from the coordinate
	for (std::vector<Actor*>::iterator it = m_actors.begin(); it != m_actors.end(); it++)
	{
		int differenceInX = (*it)->getX() - x;
		if (differenceInX < 0)
			differenceInX *= -1;
		int differenceInY = (*it)->getY() - y;
		if (differenceInY < 0)
			differenceInY *= -1;
		// distance formula
		double distanceInBetween = pow(pow(differenceInX, 2) + pow(differenceInY, 2), 0.5);
		if (distanceInBetween <= radius) // if the distance is <= radius,
			return (*it); // return a pointer to that actor
	}
	return nullptr;
}

void StudentWorld::revealAllNearbyObjects(int x, int y, int radius)
{
	for (std::vector<Actor*>::iterator it = m_actors.begin(); it != m_actors.end(); it++)
	{
		int differenceInX = x - (*it)->getX();
		if (differenceInX < 0)
			differenceInX *= -1;
		int differenceInY = y - (*it)->getY();
		if (differenceInY < 0)
			differenceInY *= -1;
		double distanceInBetween = pow(pow(differenceInX, 2) + pow(differenceInY, 2), 0.5);
		if (distanceInBetween <= radius)
			(*it)->setVisible(true);
	}
}

bool StudentWorld::canActorMoveTo(Actor* ptr, int x, int y)
{
	Actor* ptr2 = findNearbyActor(x, y, 3);
	if (ptr2 != nullptr && !ptr2->canPassThroughMe()) // if you find an actor that you cant pass through
		if (ptr2 != ptr)
			return false; // return that you can't move to this place
	if (x > 60 || x < 0 || y > 60 || y < 0) // handles bounds
		return false;
	// checking if there's dirt
	switch (ptr->getDirection())
	{
	case GraphObject::Direction::up:
		for (int k = ptr->getX(); k < ptr->getX() + 4 && k < 64; k++) // if dirt above, regenerate
			if (isThereDirt(k, ptr->getY() + 1) && !ptr->canDigThroughDirt())
				return false;
		break;
	case GraphObject::Direction::down:
		for (int k = ptr->getX(); k < ptr->getX() + 4 && k < 64; k++) // if dirt above, regenerate
			if (isThereDirt(k, ptr->getY() - 1)/* && !ptr->canDigThroughDirt()*/)
				return false;
		break;
	case GraphObject::Direction::right:
		for (int k = ptr->getY(); k < ptr->getY() + 4 && k < 60; k++) // if dirt above, regenerate
			if (isThereDirt(ptr->getX() + 4, k) && !ptr->canDigThroughDirt())
				return false;
		break;
	case GraphObject::Direction::left:
		for (int k = ptr->getY(); k < ptr->getY() + 4 && k < 60; k++) // if dirt above, regenerate
			if (isThereDirt(ptr->getX() - 1, k) && !ptr->canDigThroughDirt())
				return false;
		break;
	}
	return true; // if everything checks out, return true;
}

int StudentWorld::init()
{
	for (int x = 0; x < 64; x++) // filling up the field with dirt
		for (int y = 0; y < 60; y++)
			m_dirt[x][y] = new Dirt(x, y, this);
	for (int y = 4; y < 60; y++) // emptying out mine shaft
		for (int x = 30; x < 34; x++)
		{
			delete m_dirt[x][y];
			m_dirt[x][y] = nullptr;
		}
	m_FrackMan = new FrackMan(30, 60, this);
	// spawn the first protester
	spawnProtester();
	int currentLevel = getLevel();
	int myNumber = 2 + (currentLevel * 1.5);
	m_ticksUntilNextProtester = max(25, 200 - currentLevel);
	// initialize boulders
	int n1 = (getLevel() / 2) + 2;
	int B = min(n1, 6); // number of boulders in the level
	for (int k = 0; k < B; k++) // for loop to generate as many boulders
	{
		int boulderX = 0; // initizlie a boulderX coordinate
		int boulderY = randInt(20, 56); // initialize a boulder y coordinate
		bool notReady = true; // bool to keep track of wheter or not you have a good X
		while (notReady)
		{
			boulderX = randInt(0, 60); // generate for X

			if (!(boulderX < 34 && boulderX > 26)) // if part of boulder is within the mine shaft, regenerate
			{
				notReady = false; // boulderX ready to go it it's satisfiese the margin stuff
				std::vector<Actor*>::iterator it = m_actors.begin(); // declare iterator that can point into vector of Actor *
				while (it != m_actors.end())
				{
					if (findNearbyActor(boulderX, boulderY, 6) != nullptr)
						notReady = true;
					it++; // increment iterator
				}
			}
		}
		m_actors.push_back(new Boulder(boulderX, boulderY, this));
	}
	// initialize oil barrels
	int n2 = getLevel() + 2;
	int L = min(n2, 20);
	for (int k = 0; k < L; k++)
	{
		int barrelX = 0; // initizlie a boulderX coordinate
		int barrelY = 0; // initialize a boulder y coordinate
		bool notReady = true; // bool to keep track of wheter or not you have a good X
		while (notReady)
		{
			barrelX = randInt(0, 60); // generate for X
			barrelY = randInt(0, 56); // generate for y
			notReady = false;
			std::vector<Actor*>::iterator it = m_actors.begin();
			while (it != m_actors.end())
			{
				if (findNearbyActor(barrelX, barrelY, 6) != nullptr)
					notReady = true;
				it++;
			}
		}
		m_actors.push_back(new Barrel(barrelX, barrelY, this)); // push a new barrel into the game
		m_barrels++; // increase the number of barrels within the game
	}
	// initialize gold nuggets
	int n3 = (5 - getLevel()) / 2;
	int G = max(n3, 2);
	for (int k = 0; k < G; k++)
	{
		int goldX = 0; // initizlie a boulderX coordinate
		int goldY = 0; // initialize a boulder y coordinate
		bool notReady = true; // bool to keep track of wheter or not you have a good X
		while (notReady)
		{
			goldX = randInt(0, 60); // generate for X
			goldY = randInt(0, 56); // generate for y
			notReady = false;
			std::vector<Actor*>::iterator it = m_actors.begin();
			while (it != m_actors.end())
			{
				if (findNearbyActor(goldX, goldY, 6) != nullptr)
					notReady = true;
				it++;
			}
		}
		m_actors.push_back(new Gold(goldX, goldY, this, PERMANENT)); // gold created at init is permanent
	}
	return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
	setDisplayText(); // continuously update the display text at the top

	// go through each array and call the actor's doSomething function
	for (std::vector<Actor*>::iterator it = m_actors.begin(); it != m_actors.end(); it++)
	{
		if ((*it) != nullptr)
			(*it)->doSomething();
	}
	m_FrackMan->doSomething();

	// Adding new actors based tick
	// Adding Protesters
	if (m_waitingTime == m_ticksUntilNextProtester) // counting till spawn Protester
	{
		m_waitingTime = 0; // reset waiting counter
		spawnProtester(); // spawn Protester
	}
	else
		m_waitingTime++;
	// probability of adding a water pool or sonar kit
	int G = (getLevel() * 25) + 300; // upper limit of the probability
	int oneInG = randInt(1, G); // generate a random integer in this pool
	if (oneInG == 50) // if it happens to be this one random number (1 in G chance)
	{
		int oneIn5 = randInt(1, 5); // generate another random number
		if (oneIn5 == 3) // if it happens to be 3 (1 in 5 chance)
			m_actors.push_back(new SonarKit(0, 60, this)); // generate sonar kit
		else // 4/5 chance
		{
			int waterPoolX = 0; // initizlize a waterPoolX coordinate
			int waterPoolY = 0; // initialize a waterPoolY coordinate
			bool notReady = true; // until you have a good coordinate
			while (notReady)
			{
				notReady = false;
				waterPoolX = randInt(0, 60);
				waterPoolY = randInt(0, 56);
				for (int k = waterPoolX; k < 64 && k < waterPoolX + 4; k++)
					for (int m = waterPoolY; m < 60 && m < waterPoolY + 4; m++)
						if (isThereDirt(k, m)) // if the water pool wants to spawn where there's dirt, regenerate
							notReady = true;
			}
			m_actors.push_back(new WaterPool(waterPoolX, waterPoolY, this));
		}
	}

	// remove dead characters
	std::vector<Actor*>::iterator it = m_actors.begin(); // iterator that points int a vector of Actor*
	while (it != m_actors.end())
	{
		if (*it == nullptr)
			it = m_actors.erase(it); // remove nullptr's
		else if (*it != nullptr && (*it)->isDead())
		{
			delete *it; // delete the dead actors
			it = m_actors.erase(it); // remove the pointer to the actor
		}
		else
			it++; // increment iterator
	}
	if (m_FrackMan->isDead()) // if FrackMan's health is 0
	{
		decLives(); // decrement the # of lives in the game
		return GWSTATUS_PLAYER_DIED; // tell game that FrackMan has died
	}
	if (m_barrels == 0) // check to see if all the barrels have been picked up
	{
		playSound(SOUND_FINISHED_LEVEL);
		return GWSTATUS_FINISHED_LEVEL;
	}
	return GWSTATUS_CONTINUE_GAME; // tell game to continue calling the move function
}
void StudentWorld::cleanUp()
{
	// delete all the dirt objects
	for (int x = 0; x < 64; x++)
		for (int y = 0; y < 60; y++)
		{
			delete m_dirt[x][y];
			m_dirt[x][y] = nullptr;
		}
	delete m_FrackMan; // delete the player
	// delete everything within m_actors
	std::vector<Actor*>::iterator it = m_actors.begin();
	while (it != m_actors.end())
	{
		if (*it == nullptr)
			it = m_actors.erase(it); // remove nullptr's
		else
		{
			delete *it;
			it = m_actors.erase(it); // remove any character
		}
	}
	m_barrels = 0; // reset m_barrels to 0
	m_nProtesters = 0; // reset protestor counter
}

StudentWorld::~StudentWorld()
{
	// delete all the dirt objects
	for (int x = 0; x < 64; x++)
		for (int y = 0; y < 60; y++)
		{
			delete m_dirt[x][y];
			m_dirt[x][y] = nullptr;
		}
	delete m_FrackMan; // delete the player
	// delete everything within m_actors
	std::vector<Actor*>::iterator it = m_actors.begin();
	while (it != m_actors.end())
	{
		if (*it == nullptr)
			it = m_actors.erase(it); // remove nullptr's
		else
		{
			delete *it;
			it = m_actors.erase(it); // remove any character
		}
	}
	m_barrels = 0; // reset m_barrels to 0
	m_nProtesters = 0; // reset the number of Protesters
}
