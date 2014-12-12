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
class Scene
{
public:
    Scene();
   inline  ~Scene(){ printf("Game Ended");}
   void render(const GLuint MatrixID, const mat4 Projection, const mat4 View);

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


};

#endif // SCENE_H
