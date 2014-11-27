#ifndef DEFS_H
#define DEFS_H
#include <GL/glew.h>
#include "GL/glfw.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>//rotation
#include <glm/gtx/norm.hpp>//rotation
using namespace glm;

enum Lane {
    LEFT,
    CENTER,
    RIGHT
};

enum GameObjectType {
    PLAYER,
    MONSTER,
    OBSTECL,
    BULLET
};

///
/// \brief The RenderData struct
///stores information for drawing
struct RenderData {
    GLuint programID;
    GLuint MatrixID;
    GLuint vertexPosition_modelspaceID;
    GLuint vertexColorID;
    GLuint vertexbuffer;
    GLuint colorbuffer;
    mat4 Projection;//prospective matrix
    mat4  View;//camera matrix
};

struct Coordinate {
    float x, y, z;
    Coordinate();
    Coordinate(float x, float y, float z)
    {
        this->x = x;
        this->y = y;
        this->z = z;
    }

    vec3 to_vec()
    {
        return vec3(x, y, z);
    }
};

#endif // DEFS_H
