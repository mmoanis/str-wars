#ifndef ENGINE_H
#define ENGINE_H
// headers
#include <stdio.h>
#include <vector>

// project files
#include "GameObject.h"
#include "Player.h"
#include "Bullet.h"

#include <common/shader.hpp>
using namespace glm;

//////////////////////////////////////////////
/// \brief The Engine class
///
/// //////////////////////////////////////////
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
    std::vector<Bullet *> bullets;

    //graphics managment functions
    bool setupGL();
    GLuint programID;
    GLuint MatrixID;
    GLuint vertexPosition_modelspaceID;
    GLuint vertexColorID;
    GLuint vertexbuffer;
    GLuint colorbuffer;
    mat4 Projection;    //prospective matrix
    mat4 View;  //camera matrix

    // One color for each vertex.
    static const GLfloat g_color_buffer_data[];

    // Our vertices. Tree consecutive floats give a 3D vertex; Three consecutive vertices give a triangle.
    // A cube has 6 faces with 2 triangles each, so this makes 6*2=12 triangles, and 12*3 vertices
    static const GLfloat g_vertex_buffer_data[];
};

#endif // ENGINE_H
