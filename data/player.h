#ifndef PLAYER_H
#define PLAYER_H

#include "gameobject.h"

class Player : public GameObject
{
public:
    Player(vec3 position);
    virtual ~Player();

    // Setup and initialize object
    void setup();

    // Especify how the object is drawn to the screen.
    virtual void render(const GLuint &MatrixID, const mat4 &Projection, const mat4 &View);

    // Update the object state
    virtual bool update(GLFWwindow* window, std::vector<GameObject *> *);

    // Checks collision with another GameObject
    virtual bool checkCollision(GameObject *);

private:

    // Player health
    int _health;
};

#endif // PLAYER_H
