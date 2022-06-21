#include "Actor.h"
#include "GameConstants.h"
#include "StudentWorld.h"

// ============================= ACTOR =============================
Actor::Actor(int imageID, double startX, double startY, int dir, double size, unsigned int depth, StudentWorld* world) 
	: GraphObject(imageID, startX, startY, dir, size, depth)
{
	alive = true;
	this->world = world;
	vertSpeed = 0;
	horSpeed = 0;
}

void Actor::setSpeed(double v, double h)
{
	horSpeed = h;
	vertSpeed = v;
}

void Actor::move()
{
	double new_y = (vertSpeed - getWorld()->getRacer()->getVert()) + getY(); 
	double new_x = horSpeed + getX();
	moveTo(new_x, new_y);
	if (isOffScreen()) kill();
}

bool Actor::isOffScreen()
{
	return getX() < 0 || getY() < 0 || getX() > VIEW_WIDTH || getY() > VIEW_HEIGHT;
}

bool Actor::doesOverlap(Actor* other)
{
	double delta_x = abs(other->getX() - getX());
	double delta_y = abs(other->getY() - getY());
	double r_sum = other->getRadius() + getRadius();
	if (delta_x < r_sum * 0.25 && delta_y < r_sum * 0.6)
		return true;
	return false;
}


// ============================= PROJECTILE =============================
Projectile::Projectile(double startX, double startY, int dir, StudentWorld* world)
	: Actor(IID_HOLY_WATER_PROJECTILE, startX, startY, dir, 1.0, 1, world)
{
	travelDistance = 160;
}

void Projectile::doSomething()
{
	if (!isAlive()) return;
	if (getWorld()->projectileSearch(this))
		return;
	moveForward(SPRITE_HEIGHT);
	travelDistance -= SPRITE_HEIGHT;
	if (isOffScreen())
	{
		kill();
		return;
	}
	if (travelDistance <= 0)
		kill();
}


// ============================= ROADOBJECTS =============================
RoadObject::RoadObject(int imageID, double startX, double startY, int dir, double size, unsigned int depth, StudentWorld* world)
	: Actor(imageID, startX, startY, dir, size, depth, world)
{
	setSpeed(-4, 0);
}

void RoadObject::doSomething()
{
	if (!isAlive()) return;
	move();
	if (!isAlive()) return;
	if (doesOverlap(getWorld()->getRacer()))
		attemptCollision();
	rotate();
}


// ============================= BORDERLINE =============================
BorderLine::BorderLine(int ID, double x, double y, StudentWorld* world) : RoadObject(ID, x, y, 0, 2, 2, world)
{
}


// ============================= OILSLICK =============================
OilSlick::OilSlick(double startX, double startY, StudentWorld* world)
	: RoadObject(IID_OIL_SLICK, startX, startY, 0, (double)(rand() % 4) + 2, 2, world)
{
}

void OilSlick::attemptCollision()
{
	getWorld()->playSound(SOUND_OIL_SLICK);
	getWorld()->getRacer()->spin();
}


// ============================= GOODIE =============================
DestroyableGoodie::DestroyableGoodie(int imageID, double startX, double startY, int dir, double size, unsigned int depth, StudentWorld* world)
	: RoadObject(imageID, startX, startY, dir, size, depth, world)
{
}


// ============================= HEALINGGOODIE =============================
HealingGoodie::HealingGoodie(double startX, double startY, StudentWorld* world)
	: DestroyableGoodie(IID_HEAL_GOODIE, startX, startY, 0, 1.0, 2, world)
{
}

void HealingGoodie::attemptCollision()
{
	GhostRacer* racer = getWorld()->getRacer();
	racer->setHP(std::min(100, racer->getHP() + 10));
	kill();
	getWorld()->playSound(SOUND_GOT_GOODIE);
	getWorld()->increaseScore(250);
}


// ============================= HOLYWATERGOODIE =============================
HolyWaterGoodie::HolyWaterGoodie(double startX, double startY, StudentWorld* world)
	: DestroyableGoodie(IID_HOLY_WATER_GOODIE, startX, startY, 90, 2.0, 2, world)
{
}

void HolyWaterGoodie::attemptCollision()
{
	GhostRacer* racer = getWorld()->getRacer();
	racer->increaseSprays();
	kill();
	getWorld()->playSound(SOUND_GOT_GOODIE);
	getWorld()->increaseScore(50);
}


// ============================= SOULGOODIE =============================
SoulGoodie::SoulGoodie(double startX, double startY, StudentWorld* world)
	: RoadObject(IID_SOUL_GOODIE, startX, startY, 0, 4.0, 2, world)
{
}

void SoulGoodie::attemptCollision()
{
	getWorld()->getRacer()->increaseSouls();
	kill();
	getWorld()->playSound(SOUND_GOT_SOUL);
	getWorld()->increaseScore(100);
}

void SoulGoodie::rotate()
{
	setDirection(getDirection() - 10);
}


// ============================= BODY =============================
Body::Body(int imageID, double startX, double startY, int dir, double size, unsigned int depth, StudentWorld* world)
	: Actor(imageID, startX, startY, dir, size, depth, world)
{
	HP = 0;
}

void Body::receiveDamage(int damage)
{
	HP -= damage;
	damageEffect();
}


// ============================= GHOSTRACER =============================
GhostRacer::GhostRacer(StudentWorld* world) : Body(IID_GHOST_RACER, 128, 32, 90, 4.0, 0, world) 
{
	setHP(100);
	holyWater = 10;
	soulsSaved = 0;
}

void GhostRacer::damageEffect()
{
	if (getHP() <= 0)
	{
		getWorld()->playSound(SOUND_PLAYER_DIE);
		kill();
	}
}

void GhostRacer::spin()
{
	int randSign = rand() % 2;
	int randNum = (rand() % 16) + 5;
	if (randSign == 0) randNum *= -1;
	int newDirection = getDirection() + randNum;
	if (newDirection < 66) newDirection = 66;
	if (newDirection > 114) newDirection = 114;
	setDirection(newDirection);
}

void GhostRacer::doSomething()
{
	if (!isAlive()) return;
	// Decide what action to take
	int ch;
	const int LEFT_EDGE = ROAD_CENTER - ROAD_WIDTH / 2;
	const int RIGHT_EDGE = ROAD_CENTER + ROAD_WIDTH / 2;
	if (getX() <= LEFT_EDGE)
	{
		if (getDirection() > 90)
		{
			receiveDamage(10);
			getWorld()->playSound(SOUND_VEHICLE_CRASH);
		}
			setDirection(82);
	}
	else if (getX() >= RIGHT_EDGE)
	{
		if (getDirection() < 90)
		{
			receiveDamage(10);
			setDirection(98);
			getWorld()->playSound(SOUND_VEHICLE_CRASH);
		}
	}
	else if (getWorld()->getKey(ch))
	{
		// user hit a key during this tick!
		switch (ch)
		{
			case KEY_PRESS_SPACE:
				if (holyWater > 0)
				{
					double direction = getDirection();
					direction = abs(direction - 90);
					direction = direction * 3.14159265 / 180.0;
					double delta_x = sin(direction) * SPRITE_HEIGHT;
					if (getDirection() > 90) delta_x *= -1;
					double delta_y = cos(direction) * SPRITE_HEIGHT;
					Projectile* proj = new Projectile(getX() + delta_x, getY() + delta_y, getDirection(), getWorld());
					getWorld()->addActor(proj);

					getWorld()->playSound(SOUND_PLAYER_SPRAY);
					holyWater--;
				}
				break;
			case KEY_PRESS_LEFT:
				if (getDirection() < 114)
					setDirection(getDirection() + 8);
				break;
			case KEY_PRESS_RIGHT:
				if (getDirection() > 66)
					setDirection(getDirection() - 8);
				break;
			case KEY_PRESS_UP:
				if (getVert() < 5)
					setSpeed(getVert()+1, 0);
				break;
			case KEY_PRESS_DOWN:
				if (getVert() > -1)
					setSpeed(getVert()-1, 0);
				break;
		}
	}
	// Move the racer
	double max_shift_per_tick = 4.0;
	double direction = getDirection();
	direction = direction * 3.14159265 / 180.0;
	double delta_x = cos(direction) * max_shift_per_tick;
	double cur_x = getX();
	double cur_y = getY();
    moveTo(delta_x + cur_x, cur_y);
}


// ============================= PLANNERS =============================
Planners::Planners(int imageID, double startX, double startY, int dir, double size, unsigned int depth, StudentWorld* world)
	: Body(imageID, startX, startY, dir, size, depth, world)
{
	movementPlan = 0;
	setSpeed(-4, 0);
	setHP(2);
}

void Planners::setMovementPlan(int plan)
{
	movementPlan = plan;
}

void Planners::pickNewPlan()
{
	if (movementPlan > 0)
	{
		movementPlan--;
		return;
	}
	int k = rand() % 6;
	movementPlan = (rand() % 29) + 4;
	int arr[6] = { -3,-2,-1,1,2,3 };
	setSpeed(getVert(), arr[k]);
	if (getHor() < 0)
		setDirection(180);
	setDirection(0);
}

bool Planners::postMove()
{
	return false;
}

void Planners::doSomething()
{
	if (!isAlive())
		return;
	if (preMove())
		return;
	move();
	if (!isAlive())
		return;
	if (postMove())
		return;
	pickNewPlan();
}


// ============================= HUMANPEDESTRIAN =============================
HumanPedestrian::HumanPedestrian(double startX, double startY, StudentWorld* world)
	: Planners(IID_HUMAN_PED, startX, startY, 0, 2.0, 0, world)
{
}

void HumanPedestrian::receiveDamage(int)
{
	setSpeed(getVert(), getHor() * -1);
	int newDirec = getHor() > 0 ? 0 : 180;
	setDirection(newDirec);
	getWorld()->playSound(SOUND_PED_HURT);
}

bool HumanPedestrian::preMove()
{
	GhostRacer* racer = getWorld()->getRacer();
	if (doesOverlap(racer))
	{
		racer->kill();
		return true;
	}
	return false;
}


// ============================= ZOMBIEPEDESTRIAN =============================
ZombiePedestrian::ZombiePedestrian(double startX, double startY, StudentWorld* world)
	: Planners(IID_ZOMBIE_PED, startX, startY, 0, 3.0, 0, world)
{
	ticksTilGrunt = 0;
}

bool ZombiePedestrian::preMove()
{
	GhostRacer* racer = getWorld()->getRacer();
	if (doesOverlap(racer))
	{
		racer->receiveDamage(5);
		receiveDamage(2);
		return true;
	}
	int delta_x = getX() - racer->getX();
	if (abs(delta_x) <= 30 && getY() > racer->getY())
	{
		setDirection(270);
		if (delta_x != 0)
			setSpeed(getVert(), delta_x > 0 ? -1 : 1);
		else
			setSpeed(getVert(), 0);
		ticksTilGrunt--;
		if (ticksTilGrunt <= 0)
		{
			getWorld()->playSound(SOUND_ZOMBIE_ATTACK);
			ticksTilGrunt = 20;
		}
	}
	return false;
}

void ZombiePedestrian::damageEffect()
{
	if (getHP() <= 0)
	{
		kill();
		getWorld()->playSound(SOUND_PED_DIE);
		if (!doesOverlap(getWorld()->getRacer()))
		{
			int i = rand() % 5;
			if (i == 0)
			{
				HealingGoodie* goodie = new HealingGoodie(getX(), getY(), getWorld());
				getWorld()->addActor(goodie);
			} 
		}
		getWorld()->increaseScore(150);
	}
	else
	{
		getWorld()->playSound(SOUND_PED_HURT);
	}
}


// ============================= ZOMBIECAB =============================
ZombieCab::ZombieCab(double startX, double startY, double vertSpeed, int lane, StudentWorld* world) 
	: Planners(IID_ZOMBIE_CAB, startX, startY, 90, 4.0, 0, world)
{
	setHP(3);
	setSpeed(vertSpeed, 0);
	hasDamagedRacer = false;
	this->lane = lane;
}

bool ZombieCab::preMove()
{
	GhostRacer* racer = getWorld()->getRacer();
	if (doesOverlap(racer))
	{
		if (hasDamagedRacer)
			return false;
		getWorld()->playSound(SOUND_VEHICLE_CRASH);
		racer->receiveDamage(20);
		if (getX() <= racer->getX())
		{
			setSpeed(getVert(), -5);
			setDirection(120 + (rand() % 20));
		}
		else
		{
			setSpeed(getVert(), 5);
			setDirection(60 - (rand() % 20));
		}
		hasDamagedRacer = true;
	}
	return false;
}

bool ZombieCab::postMove()
{
	GhostRacer* racer = getWorld()->getRacer();
	// Check Existence of Front Actor
	if (getVert() > racer->getVert())
	{
		if (getWorld()->cabVicinityCheck(this, this->lane, true))
		{
			setSpeed(getVert() - 0.5, getHor());
			return true;
		}
	}
	// Check Existence of Back Actor
	else if(getVert() <= racer->getVert())
	{
		if (getWorld()->cabVicinityCheck(this, this->lane, false))
		{
			setSpeed(getVert() + 0.5, getHor());
			return true;
		}
	}
	return false;
}

void ZombieCab::pickNewPlan()
{
	setPlan(getPlan() - 1);
	if (getPlan() > 0)
	{
		return;
	}
	int k = (rand() % 29) + 4;
	int j = (rand() % 5) - 2;
	setPlan(k);
	setSpeed(getVert() + j, getHor());
}

void ZombieCab::damageEffect()
{
	if (getHP() <= 0)
	{
		kill();
		getWorld()->playSound(SOUND_VEHICLE_DIE);
		int i = rand() % 5;
		if (i == 0)
		{
			OilSlick* slick = new OilSlick(getX(), getY(), getWorld());
			getWorld()->addActor(slick);
		}  
		getWorld()->increaseScore(200);
		return;
	}
	else
	{
		getWorld()->playSound(SOUND_VEHICLE_HURT);
	}
}

