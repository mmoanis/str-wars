#ifndef ENGINE_H
#define ENGINE_H

#include <stdio.h>

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

    std::vector<Bullet *> bullets;

    //setup gl and open window
    bool setupGL();

    //program id
    GLuint programID;

    //mvp matrix id
    GLuint MatrixID;

    GLuint vertexPosition_modelspaceID;
    GLuint vertexColorID;

    //prospective matrix
    mat4 Projection;

    //camera matrix
    mat4  View;

    //decrease update rate of certain objects
    int updateSensitivity;
};

#endif // ENGINE_H
