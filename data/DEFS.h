#ifndef DEFS_H
#define DEFS_H
#include <GL/glew.h>
#include "glfw3.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>//rotation
#include <glm/gtx/norm.hpp>//rotation
using namespace glm;

//global definations
#define MAX_POSITIVE_X 7
#define MAX_POSITIVE_Y 7
#define MAX_POSITIVE_Z 7
#define MAX_NEGATIVE_X -7
#define MAX_NEGATIVE_Y -7
#define MAX_NEGATIVE_Z -5
#define UPDATES_PER_SECOND 60

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

#endif // DEFS_H
