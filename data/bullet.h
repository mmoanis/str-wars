#ifndef BULLET_H
#define BULLET_H
#include "gameobject.h"

class Bullet : public GameObject
{
public:
    Bullet(vec3 position);
    virtual ~Bullet();

    //especify how the object is drawn to the screen
    virtual void render(const GLuint &MatrixID, const GLuint &ModelMatrixID, const GLuint &ViewMatrixID, const mat4 &Projection, const mat4 &View);

    //update the object state
    virtual bool update(GLFWwindow* window, std::list<GameObject *> *);

private:
    // Number of bullet objects created
    static int bulletCount;
};

#endif // BULLET_H
