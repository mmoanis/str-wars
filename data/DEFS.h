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
#define MAX_POSITIVE_X 5
#define MAX_POSITIVE_Y 1
#define MAX_POSITIVE_Z 5
#define MAX_NEGATIVE_X -5
#define MAX_NEGATIVE_Y -2
#define MAX_NEGATIVE_Z -5
#define BULLET_BARRIER 10
#define UPDATES_PER_SECOND 60

///
/// \Defines the bounderies of the collider, used in collision detection
///
struct Collider
{

};

enum Lane {
    LEFT,
    CENTER,
    RIGHT
};

enum GameObjectType {
    PLAYER,
    MONSTER,
    OBSTECL,
    BULLET,
    SCENE
};

#endif // DEFS_H
