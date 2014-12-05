#ifndef WRAPPER_H
#define WRAPPER_H

// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <vector>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <glfw3.h>
GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

class Wrapper
{
    void setup();

    bool loadOBJ(
        const char * path,
        std::vector<glm::vec3> & out_vertices,
        std::vector<glm::vec2> & out_uvs,
        std::vector<glm::vec3> & out_normals
    );



    bool loadAssImp(
        const char * path,
        std::vector<unsigned short> & indices,
        std::vector<glm::vec3> & vertices,
        std::vector<glm::vec2> & uvs,
        std::vector<glm::vec3> & normals
    );

    // Load a .BMP file using our custom loader
    GLuint loadBMP_custom(const char * imagepath);

    // Load a .DDS file using GLFW's own loader
    GLuint loadDDS(const char * imagepath);

    // Load shader
    GLuint LoadShaders(const char * vertex_file_path,const char * fragment_file_path);
};

#endif
