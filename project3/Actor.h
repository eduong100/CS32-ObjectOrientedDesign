#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"

class StudentWorld;

class Actor: public GraphObject
{
public:
	Actor(int imageID, double startX, double startY, int dir = 0, double size = 1.0, unsigned int depth = 0, StudentWorld* world = nullptr );
	virtual void doSomething() = 0;

	void kill() { alive = false; }
	bool isAlive() const { return alive; }

	double getVert() { return vertSpeed; }
	double getHor() { return horSpeed; }
	void setSpeed(double v, double h);
	bool isOffScreen();
	void move();

	virtual bool collidesWithProjectile() { return false; }
	virtual void receiveDamage(int) { return; }
	virtual bool isCollisionAvoidanceWorthy() { return false; }

	bool doesOverlap(Actor* other);
	StudentWorld* getWorld() { return world; }
private:
	bool alive;
	double vertSpeed;
	double horSpeed;
	StudentWorld* world;
};

class RoadObject : public Actor
{
public:
	RoadObject(int imageID, double startX, double startY, int dir = 0, double size = 1.0, unsigned int depth = 0, StudentWorld* world = nullptr);
	virtual void doSomething();
private:
	virtual void attemptCollision() { return; }
	virtual void rotate() { return; }
};

class OilSlick : public RoadObject
{
public:
	OilSlick(double startX, double startY, StudentWorld* world);
private:
	virtual void attemptCollision();
};

class DestroyableGoodie : public RoadObject
{
public:
	DestroyableGoodie(int imageID, double startX, double startY, int dir = 0, double size = 1.0, unsigned int depth = 0, StudentWorld* world = nullptr);
	virtual void receiveDamage(int) { kill(); }
	virtual bool collidesWithProjectile() { return true; }
};

class HealingGoodie : public DestroyableGoodie
{
public:
	HealingGoodie(double startX, double startY, StudentWorld* world);
private:
	virtual void attemptCollision();
};

class HolyWaterGoodie : public DestroyableGoodie
{
public:
	HolyWaterGoodie(double startX, double startY, StudentWorld* world);
private:
	virtual void attemptCollision();
};

class SoulGoodie : public RoadObject
{
public:
	SoulGoodie(double startX, double startY, StudentWorld* world);
private:
	virtual void attemptCollision();
	virtual void rotate();
};

class Projectile : public Actor
{
public: 
	Projectile(double startX, double startY, int dir, StudentWorld* world);
	virtual void doSomething();
private:
	int travelDistance;
};

class BorderLine: public RoadObject
{
public:
	BorderLine(int ID, double x, double y, StudentWorld* world);
};

class Body : public Actor
{
public: 
	Body(int imageID, double startX, double startY, int dir, double size, unsigned int depth, StudentWorld* world);
	virtual bool isCollisionAvoidanceWorthy() { return true; }

	int getHP() const { return HP; }
	void setHP(int health) { HP = health; }

	virtual void receiveDamage(int damage);
private:
	virtual void damageEffect() { return; }
	int HP;
};

class GhostRacer : public Body
{
public:
	GhostRacer(StudentWorld* world);
	virtual void doSomething();

	void spin();

	int getSprays() const { return holyWater; }
	void increaseSprays() { holyWater += 10; }

	int getSouls() const { return soulsSaved; }
	void increaseSouls() { soulsSaved++; }
private:
	virtual void damageEffect();
	int holyWater;
	int soulsSaved;
};

// Planner class
class Planners : public Body
{
public:
	Planners(int imageID, double startX, double startY, int dir, double size, unsigned int depth, StudentWorld* world);
	virtual void doSomething();

	virtual bool collidesWithProjectile() { return true; }

	int getPlan() const { return movementPlan; }
	void setPlan(int num) { movementPlan = num; }
private:
	virtual bool preMove() = 0;
	virtual bool postMove();
	virtual void pickNewPlan();
	void setMovementPlan(int plan);
	int movementPlan;
};


// SMART AI
class HumanPedestrian : public Planners
{
public:
	HumanPedestrian(double startX, double startY, StudentWorld* world);
	virtual void receiveDamage(int damage);
private:
	virtual bool preMove();
};

class ZombiePedestrian : public Planners
{
public:
	ZombiePedestrian(double startX, double startY, StudentWorld* world);
private:
	virtual bool preMove();
	virtual void damageEffect();
	int ticksTilGrunt;
};

class ZombieCab : public Planners
{
public:
	ZombieCab(double startX, double startY, double vertSpeed, int lane, StudentWorld* world);
private:
	virtual bool preMove();
	virtual bool postMove();
	virtual void pickNewPlan();
	virtual void damageEffect();
	int lane;
	bool hasDamagedRacer;
}; 


#endif // ACTOR_H_
