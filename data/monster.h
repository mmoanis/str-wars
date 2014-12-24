#ifndef MONSTER_H
#define MONSTER_H
#include "gameobject.h"

class Monster : public GameObject
{
public:
    Monster(vec3 position);
    virtual ~Monster();

    // Setup and initialize object
    void setup();

    // Especify how the object is drawn to the screen.
    virtual void render(const GLuint &MatrixID, const mat4 &Projection, const mat4 &View);

    // Update the object state
    virtual bool update(GLFWwindow* window, std::vector<GameObject *> *);

    // Checks collision with another GameObject
    virtual bool checkCollision(GameObject *);

    // Checks if monster still in screen
    bool isInRange() const;
private:
    bool inRange;

    // Number of monsters objects created
    static int monsterCount;
};


#endif // MONSTER_H
