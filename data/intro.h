#ifndef SCENE_H
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

#include <common/shader.hpp>
#include <common/texture.hpp>

#include <glm/gtx/euler_angles.hpp>//rotation
#include <glm/gtx/norm.hpp>//rotation

///
/// \brief
///
class Intro
{
public:
    Intro();
    void render(const GLuint &MatrixID, const GLuint &ModelMatrixID, const GLuint &ViewMatrixID, const mat4 &Projection, const mat4 &View);

    //
    mat4 MVP;
    GLuint vertexPosition_modelspaceID;
    GLuint vertexUVID ;
    GLuint Texture;
    GLuint TextureID;
    GLuint vertexbuffer;
    GLuint uvbuffer;
};

#endif // SCENE_H
