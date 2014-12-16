#ifndef BULLET_H
#define BULLET_H
#include "gameobject.h"

class Bullet : public GameObject
{
public:
    Bullet(Lane lane, vec3 position);
    virtual ~Bullet();

    //especify how the object is drawn to the screen
    virtual void render(const GLuint &MatrixID, const mat4 &Projection, const mat4 &View);

    //setup and initialize bullet object
    virtual void setup();

    //update the object state
    virtual void update(GLFWwindow* window);

    //Checks collision with another GameObject
    virtual bool checkCollision(GameObject *);

    //check wether the bullet is still in screen
    bool isInRange() const;
private:
    //bullet is still alive
    bool inRange;

    // Number of bullet objects created
    static int bulletCount;
};

#endif // BULLET_H
