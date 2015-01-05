#ifndef PLAYER_H
#define PLAYER_H

#include "gameobject.h"

class Player : public GameObject
{
public:
    Player(vec3 position);
    virtual ~Player();

    // Especify how the object is drawn to the screen.
    virtual void render(const GLuint &MatrixID, const GLuint &ModelMatrixID, const GLuint &ViewMatrixID, const mat4 &Projection, const mat4 &View);

    // Update the object state
    virtual bool update(GLFWwindow* window, std::list<GameObject *> *);

    // Checks collision with another GameObject
    virtual bool checkCollision(GameObject *);
};

#endif // PLAYER_H
