#ifndef BULLET_H
#define BULLET_H
#include "gameobject.h"

class Bullet : public GameObject
{
public:
    Bullet(Lane lane, vec3 position);

    /**
    * especify how the object is drawn to the screen.
    **/
    virtual void render(GLuint & programID,GLuint &MatrixID,
                        mat4 Projection, mat4  View);

    /**
    * update the object state.
    **/
    virtual void update();

    /**
    * Checks collision with another GameObject.
    **/
    virtual bool checkCollision(GameObject *);
};

#endif // BULLET_H
