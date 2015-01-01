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
/// \brief The Scene class holds the context of the game scene.
///
class Scene
{
public:
    Scene();
    inline  ~Scene(){}
    void render(const GLuint MatrixID, const mat4 Projection, const mat4 View);

    // Update the scene according to the level.
    void update();

    // Reset the scene to the default.
    void reset();

    //
    glm::mat4 MVP;
    GLuint vertexPosition_modelspaceID;
    GLuint vertexUVID ;
    GLuint Texture;
    GLuint TextureID;
    glm::mat4 Model;
    GLuint MatrixID ;
    GLuint ViewMatrixID ;
    GLuint ModelMatrixID ;
    GLuint vertexbuffer;
    GLuint uvbuffer;
    float anglex;
    float angley;

    int levelCount;

};

#endif // SCENE_H
