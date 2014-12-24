/*#ifndef MENU_H
#define MENU_H

#include "gameobject.h"

class Menu : public GameObject {

public:
    Menu(vec3 position);
    virtual ~Player();

    // Setup and initialize object
    void setup();

    // Especify how the object is drawn to the screen.
    virtual void render(const GLuint &MatrixID, const mat4 &Projection, const mat4 &View);

    // Update the object state
    virtual bool update(GLFWwindow* window, std::vector<GameObject *> *);

    // Checks collision with another GameObject
    virtual bool checkCollision(GameObject *);
};

#endif
*/
