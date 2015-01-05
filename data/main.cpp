// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <list>
#include <vector>
#include <math.h>
#include <SFML/Audio.hpp>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <glfw3.h>
GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include <common/shader.hpp>
#include <common/texture.hpp>
#include <common/objloader.hpp>
#include <common/text2D.hpp>

#include "player.h"
#include "bullet.h"
#include "scene.h"
#include "monster.h"
#include "obstecle.h"
#include "explosion.h"
#include "level.h"
#include "intro.h"

Intro * intro;
Scene * scene;
Player * player;
Explode * explosion;

std::list < std::list<GameObject* >::iterator > deletedObjects;
std::list < GameObject * > objects;

// Remove objects that have got out of game ranges, or remove all objects.
void removeUnessaryObjects(bool clear);

///
/// Holds the main context of the game
///
///
int main( void )
{
    /// ----------------------------------* Initialise GLFW *-----------------------------------------------
    if( !glfwInit() )
    {
        fprintf( stderr, "Failed to initialize GLFW\n" );
        return -1;
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_RESIZABLE, 0);


    // Open a window and create its OpenGL context
    window = glfwCreateWindow( 1024, 768, "str-wars", NULL, NULL);
    if( window == NULL ){
        fprintf( stderr, "Failed to open GLFW window.\n" );
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Initialize GLEW
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        return -1;
    }

    printf("main::main():61 glfw context created\n");

    /// Window properties

    // Ensure we can capture the escape key being pressed below
    //glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    // Dark blue background
    glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    // Accept fragment if it closer to the camera than the former one
    glDepthFunc(GL_LESS);

    // Cull triangles which normal is not towards the camera
    glEnable(GL_CULL_FACE);

    // Create and compile our GLSL program from the shaders
    GLuint program2ID = LoadShaders( "StandardShading.vertexshader", "StandardShading.fragmentshader" );
    GLuint programID = LoadShaders( "TransformVertexShader.vertexshader", "TextureFragmentShader.fragmentshader" );

    // Get a handle for our "MVP" uniform
    // Model matrix gives the coordinates and transformation of objects in global coordinates
    // returns an integer that represents the location of a specific uniform variable
    // Get a handle for our "MVP" uniform.
    // Only at initialisation time.
    GLuint MatrixID = glGetUniformLocation(programID, "MVP");
    GLuint Matrix2ID = glGetUniformLocation(program2ID, "MVP");
    GLuint ViewMatrixID = glGetUniformLocation(program2ID, "V");
    GLuint ModelMatrixID = glGetUniformLocation(program2ID, "M");

    // Setup the lightening
    GLuint LightID = glGetUniformLocation(program2ID, "LightPosition_worldspace");
    glm::vec3 lightPos = glm::vec3(0,-3,-10);
    glUniform3f(LightID, lightPos.x, lightPos.y, lightPos.z);

    // Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
    glm::mat4 Projection = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 100.0f);
    // Camera matrix
    glm::mat4 View       = glm::lookAt(
                 glm::vec3(0, 2,-10), //position
                 glm::vec3(0,0,0), // and looks at the origin
                 glm::vec3(0,1,0)  // Head is up
                 );

    printf("main::main() finished initialization\n");

    /// ----------------------------------------* GameObjects *------------------------------------------

    // /////////////////////////////////////////////////////////////////////////////////////////////////
    // The Scene
    printf("main::main() making scene\n");

    // define our scene
    scene = new Scene();

    GLuint vertexPosition_modelspaceID = glGetAttribLocation(programID, "vertexPosition_modelspace");
    GLuint vertexUVID = glGetAttribLocation(programID, "vertexUV");
   // Load the texture
    GLuint Tex = loadBMP_custom("download.bmp");

   // Get a handle for our "myTextureSampler" uniform
    GLuint TexID  = glGetUniformLocation(programID, "myTextureSampler");

    scene->vertexPosition_modelspaceID = vertexPosition_modelspaceID;
    scene->vertexUVID = vertexUVID;
    scene->Texture = Tex;
    scene->TextureID = TexID;
    //
    // The scene ends
    // /////////////////////////////////////////////////////////////////////////////////////////////////////

    // /////////////////////////////////////////////////////////////////////////////////////////////////
    // The intro

    intro = new Intro();
    // Load the textures
    GLuint Tex1 = loadBMP_custom("1.bmp");
    GLuint Tex2 = loadBMP_custom("2.bmp");
    GLuint Tex3 = loadBMP_custom("3.bmp");

    intro->vertexPosition_modelspaceID = vertexPosition_modelspaceID;
    intro->vertexUVID = vertexUVID;
    intro->Texture = Tex1;
    intro->TextureID = TexID;

    //
    // The intro ends
    // /////////////////////////////////////////////////////////////////////////////////////////////////////

    // /////////////////////////////////////////////////////////////////////////////////////////////////
    // The levels

    LevelLoader levelLoader;

    if (!levelLoader.loadLevels())
    {
        printf("failed to load levels\n");
        glfwTerminate();
        return -1;
    }

    //
    // The levels ends
    // /////////////////////////////////////////////////////////////////////////////////////////////////////

    // /////////////////////////////////////////////////////////////////////////////////////////////////
    // The player
    printf("main::main(): making player\n");

    // Define our player
    player = new Player(vec3(0, 0, 0));

    // Get a handle for our buffers
    GLuint p_vertexPosition_modelspaceID = glGetAttribLocation(program2ID, "vertexPosition_modelspace");
    GLuint p_vertexUVID = glGetAttribLocation(program2ID, "vertexUV");
    GLuint p_vertexNormal_modelspaceID = glGetAttribLocation(program2ID, "vertexNormal_modelspace");

    // Load the texture
    GLuint p_Texture = loadBMP_custom("planeMonster.bmp");

    // Get a handle for our "myTextureSampler" uniform
    GLuint p_TextureID  = glGetUniformLocation(program2ID, "myTextureSampler");

    // Read our .obj file
    std::vector<glm::vec3> p_vertices;
    std::vector<glm::vec2> p_uvs;
    std::vector<glm::vec3> p_normals;
    loadOBJ("planeMonster.obj", p_vertices, p_uvs, p_normals);

    // Load it into a VBO
    GLuint p_vertexbuffer;
    glGenBuffers(1, &p_vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, p_vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, p_vertices.size() * sizeof(glm::vec3), &p_vertices[0], GL_STATIC_DRAW);

    GLuint p_uvbuffer;
    glGenBuffers(1, &p_uvbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, p_uvbuffer);
    glBufferData(GL_ARRAY_BUFFER, p_uvs.size() * sizeof(glm::vec2), &p_uvs[0], GL_STATIC_DRAW);

    GLuint p_normalbuffer;
    glGenBuffers(1, &p_normalbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, p_normalbuffer);
    glBufferData(GL_ARRAY_BUFFER, p_normals.size() * sizeof(glm::vec3), &p_normals[0], GL_STATIC_DRAW);

    player->setIDs(p_vertexPosition_modelspaceID, p_vertexUVID, p_uvbuffer, p_vertexbuffer, 0, 0, p_vertexNormal_modelspaceID, p_normalbuffer);
    player->setNormals(p_normals);
    player->setTexture(p_TextureID, p_Texture);
    player->setUVs(p_uvs);
    player->setVertices(p_vertices);

    //
    // The player ends
    // /////////////////////////////////////////////////////////////////////////////////////////////////////

    // /////////////////////////////////////////////////////////////////////////////////////////////////////
    // The bullet begins
    printf("main::main() making bullets\n");

    static const GLfloat g_vertex_buffer_data[] = {
            -1.0f,-1.0f,-1.0f,
            -1.0f,-1.0f, 1.0f,
            -1.0f, 1.0f, 1.0f,
             1.0f, 1.0f,-1.0f,
            -1.0f,-1.0f,-1.0f,
            -1.0f, 1.0f,-1.0f,
             1.0f,-1.0f, 1.0f,
            -1.0f,-1.0f,-1.0f,
             1.0f,-1.0f,-1.0f,
             1.0f, 1.0f,-1.0f,
             1.0f,-1.0f,-1.0f,
            -1.0f,-1.0f,-1.0f,
            -1.0f,-1.0f,-1.0f,
            -1.0f, 1.0f, 1.0f,
            -1.0f, 1.0f,-1.0f,
             1.0f,-1.0f, 1.0f,
            -1.0f,-1.0f, 1.0f,
            -1.0f,-1.0f,-1.0f,
            -1.0f, 1.0f, 1.0f,
            -1.0f,-1.0f, 1.0f,
             1.0f,-1.0f, 1.0f,
             1.0f, 1.0f, 1.0f,
             1.0f,-1.0f,-1.0f,
             1.0f, 1.0f,-1.0f,
             1.0f,-1.0f,-1.0f,
             1.0f, 1.0f, 1.0f,
             1.0f,-1.0f, 1.0f,
             1.0f, 1.0f, 1.0f,
             1.0f, 1.0f,-1.0f,
            -1.0f, 1.0f,-1.0f,
             1.0f, 1.0f, 1.0f,
            -1.0f, 1.0f,-1.0f,
            -1.0f, 1.0f, 1.0f,
             1.0f, 1.0f, 1.0f,
            -1.0f, 1.0f, 1.0f,
             1.0f,-1.0f, 1.0f
    };

    // Two UV coordinatesfor each vertex. They were created with Blender.
        static const GLfloat g_uv_buffer_data[] = {
            0.000059f, 1.0f-0.000004f,
            0.000103f, 1.0f-0.336048f,
            0.335973f, 1.0f-0.335903f,
            1.000023f, 1.0f-0.000013f,
            0.667979f, 1.0f-0.335851f,
            0.999958f, 1.0f-0.336064f,
            0.667979f, 1.0f-0.335851f,
            0.336024f, 1.0f-0.671877f,
            0.667969f, 1.0f-0.671889f,
            1.000023f, 1.0f-0.000013f,
            0.668104f, 1.0f-0.000013f,
            0.667979f, 1.0f-0.335851f,
            0.000059f, 1.0f-0.000004f,
            0.335973f, 1.0f-0.335903f,
            0.336098f, 1.0f-0.000071f,
            0.667979f, 1.0f-0.335851f,
            0.335973f, 1.0f-0.335903f,
            0.336024f, 1.0f-0.671877f,
            1.000004f, 1.0f-0.671847f,
            0.999958f, 1.0f-0.336064f,
            0.667979f, 1.0f-0.335851f,
            0.668104f, 1.0f-0.000013f,
            0.335973f, 1.0f-0.335903f,
            0.667979f, 1.0f-0.335851f,
            0.335973f, 1.0f-0.335903f,
            0.668104f, 1.0f-0.000013f,
            0.336098f, 1.0f-0.000071f,
            0.000103f, 1.0f-0.336048f,
            0.000004f, 1.0f-0.671870f,
            0.336024f, 1.0f-0.671877f,
            0.000103f, 1.0f-0.336048f,
            0.336024f, 1.0f-0.671877f,
            0.335973f, 1.0f-0.335903f,
            0.667969f, 1.0f-0.671889f,
            1.000004f, 1.0f-0.671847f,
            0.667979f, 1.0f-0.335851f
        };

    GLuint b_vertexPosition_modelspaceID = glGetAttribLocation(programID, "vertexPosition_modelspace");
    GLuint b_vertexUVID = glGetAttribLocation(programID, "vertexUV");

    GLuint b_vertexbuffer;
    glGenBuffers(1, &b_vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, b_vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

    GLuint b_uvbuffer;
    glGenBuffers(1, &b_uvbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, b_uvbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_uv_buffer_data), g_uv_buffer_data, GL_STATIC_DRAW);

    //
    // The bullets ends
    // /////////////////////////////////////////////////////////////////////////////////////////////////

    // /////////////////////////////////////////////////////////////////////////////////////////////////
    // The Monster

    // Get a handle for our buffers
    GLuint m_vertexPosition_modelspaceID = glGetAttribLocation(program2ID, "vertexPosition_modelspace");
    GLuint m_vertexUVID = glGetAttribLocation(program2ID, "vertexUV");
    GLuint m_vertexNormal_modelspaceID = glGetAttribLocation(program2ID, "vertexNormal_modelspace");

    // Load the texture
    GLuint m_Texture = loadBMP_custom("Dropship_D.bmp");

    // Get a handle for our "myTextureSampler" uniform
    GLuint m_TextureID  = glGetUniformLocation(program2ID, "myTextureSampler");

    // Read our .obj file
    std::vector<glm::vec3> m_vertices;
    std::vector<glm::vec2> m_uvs;
    std::vector<glm::vec3> m_normals;
    loadOBJ("Dropship.obj", m_vertices, m_uvs, m_normals);

    // Load it into a VBO
    GLuint m_vertexbuffer;
    glGenBuffers(1, &m_vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(glm::vec3), &m_vertices[0], GL_STATIC_DRAW);

    GLuint m_uvbuffer;
    glGenBuffers(1, &m_uvbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_uvbuffer);
    glBufferData(GL_ARRAY_BUFFER, m_uvs.size() * sizeof(glm::vec2), &m_uvs[0], GL_STATIC_DRAW);

    GLuint m_normalbuffer;
    glGenBuffers(1, &m_normalbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_normalbuffer);
    glBufferData(GL_ARRAY_BUFFER, m_normals.size() * sizeof(glm::vec3), &m_normals[0], GL_STATIC_DRAW);

    //
    // The monster ends
    // /////////////////////////////////////////////////////////////////////////////////////////////////

    // /////////////////////////////////////////////////////////////////////////////////////////////////
    // The Obstecles begins

    GLuint O_vertexPosition_modelspaceID = glGetAttribLocation(program2ID, "vertexPosition_modelspace");
    GLuint o_vertexNormal_modelspaceID = glGetAttribLocation(program2ID, "vertexNormal_modelspace");
    //GLuint O_vertexColorID = glGetAttribLocation(programID, "vertexColor");
    // Get a handle for our "myTextureSampler" uniform
    GLuint O_TextureID  = glGetUniformLocation(program2ID, "myTextureSampler");
    GLuint O_vertexUVID = glGetAttribLocation(program2ID, "vertexUV");

    // Load the texture
    GLuint O_Texture[5] = { loadBMP_custom("obstcale.bmp"), loadBMP_custom("AM1.bmp"), loadBMP_custom("Am2.bmp"),
                            loadBMP_custom("AM3.bmp"), loadBMP_custom("Am4.bmp") };

    // Read our .obj file
    std::vector<glm::vec3> O_vertices[5];
    std::vector<glm::vec2> O_uvs[5];
    std::vector<glm::vec3> O_normals[5];
    loadOBJ("asteroid11.obj", O_vertices[0], O_uvs[0], O_normals[0]);
    loadOBJ("asteroid23.obj", O_vertices[1], O_uvs[1], O_normals[1]);
    loadOBJ("obstcale.obj", O_vertices[2], O_uvs[2], O_normals[2]);
    loadOBJ("asteroid7.obj", O_vertices[3], O_uvs[3], O_normals[3]);
    loadOBJ("asteroid27.obj", O_vertices[4], O_uvs[4], O_normals[4]);

    // Load it into a VBO
    GLuint O_vertexbuffer[5];
    glGenBuffers(1, &O_vertexbuffer[0]);
    glBindBuffer(GL_ARRAY_BUFFER, O_vertexbuffer[0]);
    glBufferData(GL_ARRAY_BUFFER, (O_vertices)[0].size() * sizeof(glm::vec3), &O_vertices[0][0], GL_STATIC_DRAW);

    glGenBuffers(1, &O_vertexbuffer[1]);
    glBindBuffer(GL_ARRAY_BUFFER, O_vertexbuffer[1]);
    glBufferData(GL_ARRAY_BUFFER, (O_vertices[1]).size() * sizeof(glm::vec3), &O_vertices[1][0], GL_STATIC_DRAW);

    glGenBuffers(1, &O_vertexbuffer[2]);
    glBindBuffer(GL_ARRAY_BUFFER, O_vertexbuffer[2]);
    glBufferData(GL_ARRAY_BUFFER, (O_vertices[2]).size() * sizeof(glm::vec3), &O_vertices[2][0], GL_STATIC_DRAW);

    glGenBuffers(1, &O_vertexbuffer[3]);
    glBindBuffer(GL_ARRAY_BUFFER, O_vertexbuffer[3]);
    glBufferData(GL_ARRAY_BUFFER, (O_vertices[3]).size() * sizeof(glm::vec3), &O_vertices[3][0], GL_STATIC_DRAW);

    glGenBuffers(1, &O_vertexbuffer[4]);
    glBindBuffer(GL_ARRAY_BUFFER, O_vertexbuffer[4]);
    glBufferData(GL_ARRAY_BUFFER, (O_vertices[4]).size() * sizeof(glm::vec3), &O_vertices[4][0], GL_STATIC_DRAW);

    // load the UVs of astroids into buffers
    GLuint O_uvbuffer[5];
    glGenBuffers(1, &O_uvbuffer[0]);
    glBindBuffer(GL_ARRAY_BUFFER, O_uvbuffer[0]);
    glBufferData(GL_ARRAY_BUFFER, (O_uvs[0]).size() * sizeof(glm::vec2), &O_uvs[0][0], GL_STATIC_DRAW);

    glGenBuffers(1, &O_uvbuffer[1]);
    glBindBuffer(GL_ARRAY_BUFFER, O_uvbuffer[1]);
    glBufferData(GL_ARRAY_BUFFER, (O_uvs[1]).size() * sizeof(glm::vec2), &O_uvs[1][0], GL_STATIC_DRAW);

    glGenBuffers(1, &O_uvbuffer[2]);
    glBindBuffer(GL_ARRAY_BUFFER, O_uvbuffer[2]);
    glBufferData(GL_ARRAY_BUFFER, (O_uvs[2]).size() * sizeof(glm::vec2), &O_uvs[2][0], GL_STATIC_DRAW);

    glGenBuffers(1, &O_uvbuffer[3]);
    glBindBuffer(GL_ARRAY_BUFFER, O_uvbuffer[3]);
    glBufferData(GL_ARRAY_BUFFER, (O_uvs[3]).size() * sizeof(glm::vec2), &O_uvs[3][0], GL_STATIC_DRAW);

    glGenBuffers(1, &O_uvbuffer[4]);
    glBindBuffer(GL_ARRAY_BUFFER, O_uvbuffer[4]);
    glBufferData(GL_ARRAY_BUFFER, (O_uvs[4]).size() * sizeof(glm::vec2), &O_uvs[4][0], GL_STATIC_DRAW);

    GLuint o_normalbuffer[5];
    glGenBuffers(1, &o_normalbuffer[0]);
    glBindBuffer(GL_ARRAY_BUFFER, o_normalbuffer[0]);
    glBufferData(GL_ARRAY_BUFFER, (O_normals[0]).size() * sizeof(glm::vec3), &O_normals[0], GL_STATIC_DRAW);

    glGenBuffers(1, &o_normalbuffer[1]);
    glBindBuffer(GL_ARRAY_BUFFER, o_normalbuffer[1]);
    glBufferData(GL_ARRAY_BUFFER, (O_normals[1]).size() * sizeof(glm::vec3), &O_normals[1], GL_STATIC_DRAW);

    glGenBuffers(1, &o_normalbuffer[2]);
    glBindBuffer(GL_ARRAY_BUFFER, o_normalbuffer[2]);
    glBufferData(GL_ARRAY_BUFFER, (O_normals[2]).size() * sizeof(glm::vec3), &O_normals[2], GL_STATIC_DRAW);

    glGenBuffers(1, &o_normalbuffer[3]);
    glBindBuffer(GL_ARRAY_BUFFER, o_normalbuffer[3]);
    glBufferData(GL_ARRAY_BUFFER, (O_normals[3]).size() * sizeof(glm::vec3), &O_normals[3], GL_STATIC_DRAW);

    glGenBuffers(1, &o_normalbuffer[4]);
    glBindBuffer(GL_ARRAY_BUFFER, o_normalbuffer[4]);
    glBufferData(GL_ARRAY_BUFFER, (O_normals[4]).size() * sizeof(glm::vec3), &O_normals[4], GL_STATIC_DRAW);

    //
    // The Obstecles ends
    // /////////////////////////////////////////////////////////////////////////////////////////////////

    // /////////////////////////////////////////////////////////////////////////////////////////////////
    // The explosion begins

    static const GLfloat e_vertex_buffer_data[] = {
            -1.0f,-1.0f,-1.0f,
            -1.0f,-1.0f, 1.0f,
            -1.0f, 1.0f, 1.0f,
             1.0f, 1.0f,-1.0f,
            -1.0f,-1.0f,-1.0f,
            -1.0f, 1.0f,-1.0f,
             1.0f,-1.0f, 1.0f,
            -1.0f,-1.0f,-1.0f,
             1.0f,-1.0f,-1.0f,
             1.0f, 1.0f,-1.0f,
             1.0f,-1.0f,-1.0f,
            -1.0f,-1.0f,-1.0f,
            -1.0f,-1.0f,-1.0f,
            -1.0f, 1.0f, 1.0f,
            -1.0f, 1.0f,-1.0f,
             1.0f,-1.0f, 1.0f,
            -1.0f,-1.0f, 1.0f,
            -1.0f,-1.0f,-1.0f,
            -1.0f, 1.0f, 1.0f,
            -1.0f,-1.0f, 1.0f,
             1.0f,-1.0f, 1.0f,
             1.0f, 1.0f, 1.0f,
             1.0f,-1.0f,-1.0f,
             1.0f, 1.0f,-1.0f,
             1.0f,-1.0f,-1.0f,
             1.0f, 1.0f, 1.0f,
             1.0f,-1.0f, 1.0f,
             1.0f, 1.0f, 1.0f,
             1.0f, 1.0f,-1.0f,
            -1.0f, 1.0f,-1.0f,
             1.0f, 1.0f, 1.0f,
            -1.0f, 1.0f,-1.0f,
            -1.0f, 1.0f, 1.0f,
             1.0f, 1.0f, 1.0f,
            -1.0f, 1.0f, 1.0f,
             1.0f,-1.0f, 1.0f
    };

    // Two UV coordinatesfor each vertex. They were created with Blender.
    static const GLfloat e_uv_buffer_data[] = {
        0.000059f, 1.0f-0.000004f,
        0.000103f, 1.0f-0.336048f,
        0.335973f, 1.0f-0.335903f,
        1.000023f, 1.0f-0.000013f,
        0.667979f, 1.0f-0.335851f,
        0.999958f, 1.0f-0.336064f,
        0.667979f, 1.0f-0.335851f,
        0.336024f, 1.0f-0.671877f,
        0.667969f, 1.0f-0.671889f,
        1.000023f, 1.0f-0.000013f,
        0.668104f, 1.0f-0.000013f,
        0.667979f, 1.0f-0.335851f,
        0.000059f, 1.0f-0.000004f,
        0.335973f, 1.0f-0.335903f,
        0.336098f, 1.0f-0.000071f,
        0.667979f, 1.0f-0.335851f,
        0.335973f, 1.0f-0.335903f,
        0.336024f, 1.0f-0.671877f,
        1.000004f, 1.0f-0.671847f,
        0.999958f, 1.0f-0.336064f,
        0.667979f, 1.0f-0.335851f,
        0.668104f, 1.0f-0.000013f,
        0.335973f, 1.0f-0.335903f,
        0.667979f, 1.0f-0.335851f,
        0.335973f, 1.0f-0.335903f,
        0.668104f, 1.0f-0.000013f,
        0.336098f, 1.0f-0.000071f,
        0.000103f, 1.0f-0.336048f,
        0.000004f, 1.0f-0.671870f,
        0.336024f, 1.0f-0.671877f,
        0.000103f, 1.0f-0.336048f,
        0.336024f, 1.0f-0.671877f,
        0.335973f, 1.0f-0.335903f,
        0.667969f, 1.0f-0.671889f,
        1.000004f, 1.0f-0.671847f,
        0.667979f, 1.0f-0.335851f
    };

    // define our explosion
    explosion = new Explode();

    GLuint evertexPosition_modelspaceID = glGetAttribLocation(programID, "vertexPosition_modelspace");
    GLuint evertexUVID = glGetAttribLocation(programID, "vertexUV");
    // Load the texture
    GLuint eTexture = loadBMP_custom("skull2.bmp");

    // Get a handle for our "myTextureSampler" uniform
    GLuint eTexID  = glGetUniformLocation(programID, "myTextureSampler");
    GLuint e_vertexbuffer, e_uvbuffer;
    glGenBuffers(1, &e_vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, e_vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(e_vertex_buffer_data), e_vertex_buffer_data, GL_STATIC_DRAW);

    glGenBuffers(1, &e_uvbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, e_uvbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(e_uv_buffer_data), e_uv_buffer_data, GL_STATIC_DRAW);

    explosion->setIDs(evertexPosition_modelspaceID, evertexUVID, e_uvbuffer, e_vertexbuffer, 0, 0, 0, 0);
    explosion->setTexture(eTexID, eTexture);

    //
    // The explosion ends
    // /////////////////////////////////////////////////////////////////////////////////////////////////


    // /////////////////////////////////////////////////////////////////////////////////////////////////
    // The Game menu

    // Initialize our little text library with the Holstein font
    initText2D( "Holstein.DDS" );
    char text[256];
    char text1[256];
    char text2[256];
    char text3[256];
    char text4[256];
    char text5[256];
    char text6[256];
    char text7[256];
    char text8[256];
    char text9[256];
    char text10[256];
    sprintf(text,"start game" );
    sprintf(text1,">exit" );
    sprintf(text2,">start game" );
    sprintf(text3,"exit" );
    sprintf(text4, "you made it hero!");
    sprintf(text5, "str-wars");
    sprintf(text6, "Islam");
    sprintf(text7, "Moanis");
    sprintf(text8, "Omar");
    sprintf(text9, "press F1 to continue");
    sprintf(text10, "press F5 to continue");

    //
    // The Game menu ends
    // /////////////////////////////////////////////////////////////////////////////////////////////////////

    ///  -----------------------------------------* Game Loop *-----------------------------------------

    printf("main::main() entering game loop\n");

    int bulletUpdates = 0;
    float posx, posy, posz=20;
    float obsteclesUpdate = 0;
    float distance = 0; //distance covered by player
    bool running = true;
    bool gameStarted = false;
    bool cursor = true;
    int deathTime = 0;
    bool collision = false;
    bool win = false;
    bool showCredits = false;
    int monsterUpdate = 0;
    Level currentLevel;
    levelLoader.getCurrentLevel(currentLevel);


    /*sf::SoundBuffer sb;
    if (!sb.loadFromFile("0614.ogg")) printf("failed to load sound\n");
    sf::Sound sound;
    sound.setBuffer(sb);
    sound.play();*/

    do{
        // Clear the screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Use our shader
        glUseProgram(programID);

        // render the scene
        scene->render(MatrixID, Projection, View);
        //intro->render(MatrixID, ModelMatrixID, ViewMatrixID, Projection, View);

        // game loop
        if (gameStarted)
        {
            if (collision)
            {
                // render the death frame
                explosion->render(MatrixID, ModelMatrixID, ViewMatrixID, Projection, View);
                deathTime++;

                if (deathTime > 50)
                {
                    collision = false;
                    deathTime = 0;
                }
            }
            else
            {
                // update distance
                distance += 0.5;
                if (distance > currentLevel.distance)
                {
                    if (levelLoader.getCurrentLevel(currentLevel))
                    {
                        // start from begining
                        scene->update();
                        printf("level%d\n", (int) currentLevel.level);
                    }
                    else
                    {
                        // player won
                        printf("player wins\n");

                        // reset all parameters
                        monsterUpdate = 0;
                        obsteclesUpdate = 0;
                        distance = 0;
                        gameStarted = false;
                        win = true;
                        player->setPosition(vec3(0, 0, 0));

                        // delete all objects
                        removeUnessaryObjects(true);

                        // reset the scene
                        //scene->reset();

                        // draw new scene, skip all the game loop
                        goto draw_objects;
                    }
                }

                ///  -----------------------------------------* Update gameobjects state *---------------------------------
                bulletUpdates++;
                monsterUpdate++;
                obsteclesUpdate++;

                // update objects
                for (std::list<GameObject*>::iterator it = objects.begin(); it != objects.end(); it++)
                {
                    // update the objects
                    if (! ((*it)->update(window, &objects) ))
                    {
                        // object collided with another one

                        // remove the collided object
                        deletedObjects.push_back(it);
                    }
                }

                // update the player position
                if (! (player->update(window, &objects)))
                {
                    // player collided with an enemy

                    // reset the levels
                    monsterUpdate = 0;
                    obsteclesUpdate = 0;
                    distance = 0;
                    collision = true;
                    player->setPosition(vec3(0, 0, 0));
                    removeUnessaryObjects(true);
                    scene->reset();
                }

                // removing dead objects
                removeUnessaryObjects(false);

                /// add new objects
                ///
                // check to add new monster and player
                if (bulletUpdates >= currentLevel.bulltes)
                {
                    bulletUpdates -= currentLevel.bulltes;

                    if (glfwGetKey( window, GLFW_KEY_SPACE) == GLFW_PRESS )
                    {
                        GameObject * bullet = new Bullet(player->getPosition());

                        // set vertices and UVs of the bullet
                        bullet->setIDs(b_vertexPosition_modelspaceID, b_vertexUVID, b_uvbuffer, b_vertexbuffer, 0, 0, 0, 0);

                        // add to list of gameobjects
                        objects.push_back(bullet);
                    }
                }

                if (monsterUpdate > currentLevel.monsters)
                {
                    monsterUpdate -= currentLevel.monsters;

                    // Generate the random positions for the enemy planes
                    posx = (rand() % (MAX_NEGATIVE_X * 2)) - MAX_POSITIVE_X;
                    posy = (rand() % ((MAX_POSITIVE_Y+2) *2)) - MAX_POSITIVE_Y - 2;

                    GameObject* monster = new Monster(vec3(posx, posy, posz));

                    monster->setIDs(m_vertexPosition_modelspaceID, m_vertexUVID, m_uvbuffer, m_vertexbuffer, 0, 0, m_vertexNormal_modelspaceID, m_normalbuffer);
                    monster->setNormals(m_normals);
                    monster->setTexture(m_TextureID, m_Texture);
                    monster->setUVs(m_uvs);
                    monster->setVertices(m_vertices);

                    // add to the list of game objects
                    objects.push_back(monster);
                }

                // add random obstecles according to the level specified obstecles updates
                if (obsteclesUpdate > currentLevel.obstcales)
                {
                    obsteclesUpdate -= currentLevel.obstcales;

                    //printf("obstecles %d\n", (int)currentLevel.obstcales);
                    // generate random position for the obstecle
                    posx = (rand() % (MAX_POSITIVE_X * 2)) - MAX_POSITIVE_X;
                    posy = (rand() % ((MAX_POSITIVE_Y+2) *2)) - MAX_POSITIVE_Y-2;
                    int i = rand() % 5;
                    GameObject *obstecle = new Obstecle(vec3(posx, posy, 20));

                    // set the obstecle textures, vertices, normals, UVs generated at the start of game
                    obstecle->setIDs(O_vertexPosition_modelspaceID, O_vertexUVID, O_uvbuffer[i], O_vertexbuffer[i], 0, 0, o_vertexNormal_modelspaceID, o_normalbuffer[i]);
                    obstecle->setNormals(O_normals[i]);
                    obstecle->setTexture(O_TextureID, O_Texture[rand() % 5]);
                    obstecle->setUVs(O_uvs[i]);
                    obstecle->setVertices(O_vertices[i]);

                    // add to the list of objects in game
                    objects.push_back(obstecle);
                }

                ///  -----------------------------------------* Rendering *-----------------------------------------
                ///

                // use the shading shaders to draw the objects, to generate the light effects
                glUseProgram(program2ID);

                // Draw the player
                player->render(Matrix2ID, ModelMatrixID, ViewMatrixID, Projection, View);

                // Draw the objects
                for (std::list<GameObject *>::iterator it = objects.begin(); it != objects.end(); it++)
                {
                    (*it)->render(Matrix2ID, ModelMatrixID, ViewMatrixID, Projection, View);
                }
            }
        }
        else if (win)
        {
            // final scene, when player wins

            if (!showCredits)
            {
                // first celebrate that the player won
                printText2D(text4, 40, 400, 40);
                printText2D(text9, 40, 50, 20);
                if (glfwGetKey( window, GLFW_KEY_F1) == GLFW_PRESS )
                {
                    showCredits = true;
                }
            }
            else
            {
                // show credits of the game
                printText2D(text5, 40, 500, 60);
                printText2D(text6, 40, 400, 60);
                printText2D(text7, 40, 300, 60);
                printText2D(text8, 40, 200, 60);
                printText2D(text10, 40, 50, 20);

                // continue to the main game menu
                if (glfwGetKey( window, GLFW_KEY_F5) == GLFW_PRESS )
                {
                    win = false;
                    scene->reset();
                }
            }
        }
        else
        {
            /// the game menu logic
            ///
            if (cursor)
            {
                printText2D(text2, 50, 500, 60);
                printText2D(text3, 60, 100, 60);
            }
            else
            {
                printText2D(text, 50, 500, 60);
                printText2D(text1, 60, 100, 60);
            }

            // navigate through the menu
            if (glfwGetKey( window, GLFW_KEY_UP) == GLFW_PRESS )
            {
                    cursor = true;
            }
            else if (glfwGetKey( window, GLFW_KEY_DOWN) == GLFW_PRESS )
            {
                cursor = false;
            }
            else if (glfwGetKey( window, GLFW_KEY_ENTER) == GLFW_PRESS )
            {
                running = gameStarted = cursor;
                printf("game started\n");
            }
        }

        draw_objects:

        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();

    } // Check if the ESC key was pressed or the window was closed
    while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
           glfwWindowShouldClose(window) == 0 && running);

    /// Termination

    printf("main::main() terminating\n");

    // Cleanup VBO and shader
    glDeleteBuffers(1, &O_vertexbuffer[0]);
    glDeleteBuffers(1, &O_vertexbuffer[1]);
    glDeleteBuffers(1, &O_vertexbuffer[2]);
    glDeleteBuffers(1, &O_vertexbuffer[3]);
    glDeleteBuffers(1, &O_vertexbuffer[4]);
    glDeleteBuffers(1, &o_normalbuffer[0]);
    glDeleteBuffers(1, &o_normalbuffer[1]);
    glDeleteBuffers(1, &o_normalbuffer[2]);
    glDeleteBuffers(1, &o_normalbuffer[3]);
    glDeleteBuffers(1, &o_normalbuffer[4]);
    glDeleteBuffers(1, &O_uvbuffer[0]);
    glDeleteBuffers(1, &O_uvbuffer[1]);
    glDeleteBuffers(1, &O_uvbuffer[2]);
    glDeleteBuffers(1, &O_uvbuffer[3]);
    glDeleteBuffers(1, &O_uvbuffer[4]);
    glDeleteBuffers(1, &m_vertexbuffer);
    glDeleteBuffers(1, &m_uvbuffer);
    glDeleteBuffers(1, &m_normalbuffer);
    glDeleteBuffers(1, &b_vertexbuffer);
    glDeleteBuffers(1, &b_uvbuffer);
    glDeleteBuffers(1, &p_uvbuffer);
    glDeleteBuffers(1, &p_vertexbuffer);
    glDeleteBuffers(1, &p_normalbuffer);
    glDeleteProgram(programID);
    glDeleteProgram(program2ID);
    glDeleteTextures(1, &O_Texture[0]);
    glDeleteTextures(1, &O_Texture[1]);
    glDeleteTextures(1, &O_Texture[2]);
    glDeleteTextures(1, &O_Texture[3]);
    glDeleteTextures(1, &O_Texture[4]);
    glDeleteTextures(1, &m_Texture);
    glDeleteTextures(1, &p_Texture);
    glDeleteTextures(1, &eTexture);
    glDeleteTextures(1, &Tex);
    glDeleteTextures(1, &Tex1);
    glDeleteTextures(1, &Tex2);
    glDeleteTextures(1, &Tex3);

    // Delete the text's VBO, the shader and the texture
    cleanupText2D();

    // Close OpenGL window and terminate GLFW
    glfwTerminate();

    //delete objects
    delete intro;
    delete scene;
    delete player;
    delete explosion;

    // delete the obstecles
    for(std::list<GameObject *>::iterator it = objects.begin(); it != objects.end(); it++)
    {
        delete (*it);
    }

    return 0;
}

// Remove and safely delete objects that are out of the scene
// if clear is true, remove all the objects
void removeUnessaryObjects(bool clear)
{
    if (clear)
    {
        for(std::list<GameObject* >::iterator it = objects.begin(); it != objects.end(); it++)
        {
            deletedObjects.push_back(it);
        }
    }
    else
    {
        //check for dead objects, that are out of range
        for(std::list<GameObject* >::iterator it = objects.begin(); it != objects.end(); it++)
        {
            if (!((*it)->isInRange()))
            {
                deletedObjects.push_back(it);
            }
        }
    }

    //remove and safely delete dead obstecles
    for(std::list < std::list<GameObject* >::iterator >::iterator deleteIterator = deletedObjects.begin(); deleteIterator != deletedObjects.end(); deleteIterator++)
    {
        GameObject * object =  **deleteIterator;
        objects.erase(*deleteIterator);
        delete object;

    }

    deletedObjects.clear();
}
