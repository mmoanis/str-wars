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

    //run game logic
    void run();

private:
    ///game managment functions

    //render objects to screen
    void render();

    //update gameobjects status
    void update();

    //gameobjects
    Player * player;

    //setup gl and open window
    bool setupGL();

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
