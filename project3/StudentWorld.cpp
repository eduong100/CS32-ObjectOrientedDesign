#include "StudentWorld.h"
#include "Actor.h"
#include "GameConstants.h"
#include <sstream>
#include <string>

using namespace std;


GameWorld* createStudentWorld(string assetPath)
{
	return new StudentWorld(assetPath);
}


// ============================= PUBLIC STUDENTWORLD METHODS =============================
StudentWorld::StudentWorld(string assetPath)
: GameWorld(assetPath)
{
    bonus = 0;
    racer = nullptr;
    lastWBLY = 0;
}


StudentWorld::~StudentWorld()
{
    cleanUp();
}


int StudentWorld::init()
{
    // Reset GhostRacer and Bonus
    bonus = 5000;
    racer = new GhostRacer(this);

    // Declare Road Measurements for BorderLines
    const int N = VIEW_HEIGHT / SPRITE_HEIGHT;
    const int M = VIEW_HEIGHT / (4*SPRITE_HEIGHT);
    const int LEFT_EDGE = ROAD_CENTER - ROAD_WIDTH / 2;
    const int RIGHT_EDGE = ROAD_CENTER + ROAD_WIDTH / 2;

    // Create Yellow BorderLines
    for (int i = 0; i < N; i++)
    {
        actors.push_back(new BorderLine(IID_YELLOW_BORDER_LINE, LEFT_EDGE, i * (double)SPRITE_HEIGHT, this));
        actors.push_back(new BorderLine(IID_YELLOW_BORDER_LINE, RIGHT_EDGE, i * (double)SPRITE_HEIGHT, this));
    }

    // Create White BorderLines
    for (int i = 0; i < M; i++) 
    {
        actors.push_back(new BorderLine(IID_WHITE_BORDER_LINE, LEFT_EDGE + (double)ROAD_WIDTH/3, i * (double)(SPRITE_HEIGHT*4), this) );
        actors.push_back(new BorderLine(IID_WHITE_BORDER_LINE, RIGHT_EDGE - (double)ROAD_WIDTH/3, i * (double)(SPRITE_HEIGHT*4), this) );        
    }

    // Store Last WBL Y Coordinate and Start Game
    lastWBLY = actors.back()->getY();
    return GWSTATUS_CONTINUE_GAME;
}


int StudentWorld::move()
{
    int soulsToSave;

    // For Each Actor
    for (std::list<Actor*>::iterator it = actors.begin(); it != actors.end(); it++)
    {
        if ((*it)->isAlive())
        {
            // Tell Actor to doSomething()
            (*it)->doSomething();

            // If Racer Is Alive Then Fail Level
            if (!racer->isAlive())
            {
                decLives();
                return GWSTATUS_PLAYER_DIED;
            }

            // If Racer Has Enough Souls Then Complete Level
            soulsToSave = (getLevel() * 2 + 5) - racer->getSouls();
            if (soulsToSave <= 0)
            {
                increaseScore(bonus);
                return GWSTATUS_FINISHED_LEVEL;
            }
        }    
    }

    // Tell the GhostRacer to doSomething()
    lastWBLY = -4 - racer->getVert() + lastWBLY;
    racer->doSomething();

    // Remove dead actors
    std::list<Actor*>::iterator it = actors.begin();
    while (it!= actors.end())
    {
        if (!(*it)->isAlive())
        {
            delete* it;
            it = actors.erase(it);
        }
        else
            it++;
    }

    // Add new actors
    addActors();

    // Update Display and Decrease Bonus
    std::ostringstream oss;
    oss << "Score: " << getScore() << " Lvl: " << getLevel() << " Souls2Save: " << soulsToSave << " Health: "
        << racer->getHP() << " Sprays: " << racer->getSprays() << " Bonus: " << bonus;
    setGameStatText(oss.str());
    bonus--;

    return GWSTATUS_CONTINUE_GAME;
}


void StudentWorld::cleanUp()
{
    // Free Actor Memory and Clear Actors List
    std::list<Actor*>::iterator it;
    for (it = actors.begin(); it != actors.end(); it++)
    {
        delete* it;
    }
    actors.clear();
    delete racer;
}


// Return True if, Within a Certain Distance, there is a 
// Collision Avoidance Worthy Actor in Front of or Behind Cab a 
bool StudentWorld::cabVicinityCheck(ZombieCab* a, int lane, bool front)
{
    // Get Proper Bounds for the Cab's Lane
    int leftBound, rightBound;
    getLaneCoord(leftBound, rightBound, lane);

    // For Each Actor
    actors.push_back(racer);
    for (list<Actor*>::iterator it = actors.begin(); it != actors.end(); it++)
    {
        // Return True if Actor in Desired Position
        Actor* curActor = *it;
        if (curActor->isCollisionAvoidanceWorthy() && isBetween(leftBound, rightBound, curActor->getX())
            && cabVicinityHelper(front, a->getY(), curActor->getY()))
        {
            actors.pop_back();
            return true;
        }
    }
    actors.pop_back();

    // Otherwise Return False
    return false;
}


// Determine if proj Collides with any Actors
bool StudentWorld::projectileSearch(Projectile* proj)
{
    // For Each Actor
    std::list<Actor*>::iterator it;
    for (it = actors.begin(); it != actors.end(); it++)
    {
        // If Actor is Destroyable and Touches proj
        if ((*it)->isAlive() && (*it)->collidesWithProjectile() && (*it)->doesOverlap(proj))
        {
            // Damage tha Actor, Kill Proj, and Return True
            (*it)->receiveDamage(1);
            proj->kill();
            return true;
        }
    }
    // Otherwise Return False
    return false;
}


// Add New Actor to List actors
void StudentWorld::addActor(Actor* actor)
{
    actors.push_back(actor);
}


// ============================= PRIVATE STUDENTWORLD METHODS =============================
void StudentWorld::addActors() 
{
    // Add BorderLines
    int new_border_y = VIEW_HEIGHT - SPRITE_HEIGHT;
    int delta_y = new_border_y - lastWBLY;
    double leftBorder = ROAD_CENTER - ROAD_WIDTH / 2.0;
    double rightBorder = ROAD_CENTER + ROAD_WIDTH / 2.0;
    int roadWidth = rightBorder - leftBorder;
    if (delta_y >= SPRITE_HEIGHT)
    {
        actors.push_back(new BorderLine(IID_YELLOW_BORDER_LINE, leftBorder, new_border_y, this));
        actors.push_back(new BorderLine(IID_YELLOW_BORDER_LINE, rightBorder, new_border_y, this));
    }
    if (delta_y >= 4*SPRITE_HEIGHT)
    {
        actors.push_back(new BorderLine(IID_WHITE_BORDER_LINE, leftBorder + ROAD_WIDTH / 3, new_border_y, this));
        actors.push_back(new BorderLine(IID_WHITE_BORDER_LINE, rightBorder - ROAD_WIDTH / 3, new_border_y, this));
        lastWBLY = new_border_y;
    }
    
    // Add HumanPedestrian
    if (standardRand(200,30))
        actors.push_back(new HumanPedestrian(rand() % (VIEW_WIDTH + 1), VIEW_HEIGHT, this));

    // Add ZombiePedestrian
    if (standardRand(100,20))
        actors.push_back(new ZombiePedestrian(rand() % (VIEW_WIDTH + 1), VIEW_HEIGHT, this));
    
    // Add ZombieCab
    addZombieCab();

    // Add OilSlick
    if (standardRand(150, 40))
        actors.push_back(new OilSlick((rand() % roadWidth) + leftBorder, VIEW_HEIGHT, this));

    // Add HolyWaterGoodie
    int chance = 100 + 10 * getLevel();
    if (rand() % chance == 0)
        actors.push_back(new HolyWaterGoodie((rand() % roadWidth) + leftBorder, VIEW_HEIGHT, this));

    // Add SoulGoodie
    if (rand() % 100 == 0)
        actors.push_back(new SoulGoodie((rand() % roadWidth) + leftBorder, VIEW_HEIGHT, this));

}


// Help Decide Randomly if Actor Should Spawn
bool StudentWorld::standardRand(int v1, int v2)
{
    int chance = max(v1 - getLevel() * 10, v2);
    int randNum = rand() % chance;
    if (randNum == 0)
        return true;
    return false;
}


// Spawn ZombieCab if Correct Conditions are Met
void StudentWorld::addZombieCab()
{
    // Proceed if we Pass Random Check
    if (standardRand(100, 20))
    {
        vector<int> randomVec = { 0, 1, 2 };

        int chosenLane = -1;
        int startY, startX, startVertSpeed;

        // Loop Three Times
        for (int j = 3; j > 0; j--)
        {
            // Pick Random Number i from { 0, 1 ,2 } Without Replacement 
            int randomSpot = rand() % j;
            int i = randomVec[randomSpot];
            randomVec.erase(randomVec.begin() + randomSpot);

            // Find Actor Closest to Bottom in Lane i
            Actor* tmp = topBottomSearch(i, false);

            // If There is Room at Bottom of Lane i
            if (tmp == nullptr || tmp->getY() > VIEW_HEIGHT / 3)
            {
                // Choose Starting Lane/Position Successfully and Exit Loop
                chosenLane = i;
                startY = SPRITE_HEIGHT / 2;
                startVertSpeed = racer->getVert() + ((rand() % 3) + 2.0);
                break;
            }

            // Find Actor Closest to Top in Lane i
            tmp = topBottomSearch(i, true);

            // If There is Room at Top in Lane i
            if (tmp == nullptr || tmp->getY() < VIEW_HEIGHT * 2 / 3)
            {
                // Choose Starting Lane/Position Successfully and Exit Loop
                chosenLane = i;
                startY = VIEW_HEIGHT - SPRITE_HEIGHT / 2;
                startVertSpeed = racer->getVert() - ((rand() % 3) + 2.0);
                break;
            }
        }

        // If Starting Lane/Position Successfully Chosen Then Spawn New Cab
        if (chosenLane != -1)
        {
            if (chosenLane == 0)
                startX = ROAD_CENTER - ROAD_WIDTH / 3;
            else if (chosenLane == 1)
                startX = ROAD_CENTER;
            else
                startX = ROAD_CENTER + ROAD_WIDTH / 3;

            actors.push_back(new ZombieCab(startX, startY, startVertSpeed, chosenLane, this));
        }
    }
}


// Find Actor Closest to Top or Bottom of Lane lane
Actor* StudentWorld::topBottomSearch(int lane, bool searchTop)
{
    Actor* result = nullptr;
    int leftBound;
    int rightBound;

    // Determine Bounds of lane
    getLaneCoord(leftBound, rightBound, lane);

    // Perform Linear Search for Desired Actor
    // Desired Actor is Closest to Top/Bottom and is Within lane
    actors.push_back(racer);
    for (list<Actor*>::iterator it = actors.begin(); it != actors.end(); it++)
    {
        Actor* curActor = *it;
        if (curActor->isCollisionAvoidanceWorthy() && isBetween(leftBound, rightBound, curActor->getX()))
        {
            if (result == nullptr) result = curActor;
            else if (!searchTop && curActor->getY() < result->getY()) result = curActor;
            else if (searchTop && curActor->getY() > result->getY()) result = curActor;
        }
    }
    actors.pop_back();

    return result;
}


// Compare YCoords of a Cab and an Actor
bool StudentWorld::cabVicinityHelper(bool front, double cabY, double actorY)
{
    switch (front)
    {
        // Return true if we Desire the Actor to be in Front
        // AND the Actor IS Within a Certain Distance in the Front
    case true:
        if (actorY > cabY && actorY - cabY < 96)
            return true;
        break;
        // Return true if we Desire the Actor to be behind
        // AND the Actor IS Within a Certain Distance behind
    case false:
        if (cabY > actorY && cabY - actorY < 96)
            return true;
        break;
    }
    // Otherwise Return False
    return false;
}


// Return True if leftBound <= x < rightBound
bool StudentWorld::isBetween(int leftBound, int rightBound, int x)
{
    return (x < rightBound && x >= leftBound);
}


// Give leftBound and rightBound the Proper Lane Coordinates
void StudentWorld::getLaneCoord(int& leftBound, int& rightBound, int lane)
{
    if (lane == 0)
    {
        leftBound = ROAD_CENTER - ROAD_WIDTH / 2;
        rightBound = leftBound + ROAD_CENTER / 3;
    }
    else if (lane == 1)
    {
        leftBound = ROAD_CENTER - ROAD_WIDTH / 2 + ROAD_CENTER / 3;
        rightBound = ROAD_CENTER + ROAD_WIDTH / 2 - ROAD_CENTER / 3;
    }
    else
    {
        rightBound = ROAD_CENTER + ROAD_WIDTH / 2;
        leftBound = rightBound - ROAD_CENTER / 3;
    }
}