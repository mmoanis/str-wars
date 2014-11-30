#ifndef PLAYER_H
#define PLAYER_H

#include "gameobject.h"
#include "vector"
#include "bullet.h"

class Player : public GameObject
{
public:
    Player(Lane lane, vec3 position);
    virtual ~Player();

    //setup and initialize object
    void setup(GLuint programID);

    //especify how the object is drawn to the screen.
    virtual void render(const GLuint &MatrixID, const mat4 &Projection, const mat4 &View);

    //update the object state
    virtual void update();

    //Checks collision with another GameObject
    virtual bool checkCollision(GameObject *);

private:
};

#endif // PLAYER_H
