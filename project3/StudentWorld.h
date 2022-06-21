#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include <string>
#include <list>

class Actor;
class GhostRacer;
class Projectile;
class ZombieCab;

class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetPath);
    ~StudentWorld();

    virtual int init();
    virtual int move();
    virtual void cleanUp();

    GhostRacer* getRacer() { return racer; }
    void addActor(Actor* actor);

    bool cabVicinityCheck(ZombieCab* a, int lane, bool front);

    bool projectileSearch(Projectile* proj);
private:
    Actor* topBottomSearch(int lane, bool searchTop);
    bool cabVicinityHelper(bool front, double cabY, double actorY);
    void addActors();
    bool isBetween(int leftBound, int rightBound, int x);
    void getLaneCoord(int& leftBound, int& rightBound, int lane);
    bool standardRand(int v1, int v2);
    void addZombieCab();

    int bonus;
    double lastWBLY;
    std::list<Actor*> actors;
    GhostRacer* racer;
};

#endif // STUDENTWORLD_H_
