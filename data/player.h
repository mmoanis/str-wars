#ifndef PLAYER_H
#define PLAYER_H

#include "gameobject.h"
#include "vector"
#include "bullet.h"

class Player : public GameObject
{
public:
    Player(Lane lane, vec3 position);
    ~Player();
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

    GLuint vertexPosition_modelspaceID;
    GLuint vertexUVID;
    GLuint uvbuffer;
    GLuint vertexbuffer;
private:
    std::vector<Bullet *> bullets;
};

#endif // PLAYER_H
