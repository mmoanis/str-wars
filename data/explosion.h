#ifndef EXPLODE_H
#include <stdio.h>
#include <stdlib.h>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <glfw3.h>

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include "gameobject.h"

class Explode : public GameObject
{
public:
    Explode();
    virtual ~Explode();

    // Especify how the object is drawn to the screen.
    virtual void render(const GLuint &MatrixID, const GLuint &ModelMatrixID, const GLuint &ViewMatrixID, const mat4 &Projection, const mat4 &View);

private:
    mat4 MVP;
};

#endif // EXPLODE_H
