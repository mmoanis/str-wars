#ifndef ENGINE_H
#define ENGINE_H
// headers
#include <stdio.h>
#include <vector>

// project files
#include "gameobject.h"
#include "player.h"
#include "bullet.h"

#include <common/shader.hpp>

class Engine
{
public:
    //constructor and destructor
    Engine();
    ~Engine();
    void run();

private:
    //game managment functions
    void render();
    void update();

    //gameobjects
    Player * player;

    //graphics managment functions
    bool setupGL();

    //RenderData renderData;

    GLuint programID;
    GLuint MatrixID;
    GLuint vertexPosition_modelspaceID;
    GLuint vertexColorID;
    GLuint vertexbuffer;
    GLuint colorbuffer;
    mat4 Projection;//prospective matrix
    mat4  View;//camera matrix
};

#endif // ENGINE_H
