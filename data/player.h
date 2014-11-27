#ifndef PLAYER_H
#define PLAYER_H
#include "gameobject.h"

class Player : public GameObject
{
public:
    Player(Lane lane, vec3 position);

    /**
    * especify how the object is drawn to the screen.
    **/
    virtual void render();

    /**
    * update the object state.
    **/
    virtual void update();

    /**
    * Checks collision with another GameObject.
    **/
    virtual bool checkCollision(GameObject *);
};

#endif // PLAYER_H
