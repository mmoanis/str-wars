// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <list>
#include <vector>
#include <math.h>
#include <algorithm>

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

Scene * scene;
Player * player;
Explode * explosion;

std::list < std::list<GameObject* >::iterator > deletedObjects;
std::list < GameObject * > objects;


void removeUnessaryObjects(bool clear);

void delay(int milli);


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
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    // Dark blue background
    glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    // Accept fragment if it closer to the camera than the former one
    glDepthFunc(GL_LESS);

    // Cull triangles which normal is not towards the camera
    glEnable(GL_CULL_FACE);

    // Create and compile our GLSL program from the shaders
    GLuint programID = LoadShaders( "TransformVertexShader.vertexshader", "TextureFragmentShader.fragmentshader" );

    // Get a handle for our "MVP" uniform
    // Model matrix gives the coordinates and transformation of objects in global coordinates
    // returns an integer that represents the location of a specific uniform variable
    // Get a handle for our "MVP" uniform.
    // Only at initialisation time.
    GLuint MatrixID = glGetUniformLocation(programID, "MVP");

    // Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
    glm::mat4 Projection = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 100.0f);
    // Camera matrix
    glm::mat4 View       = glm::lookAt(
                 glm::vec3(0, 1,-10), //position
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
    GLuint p_vertexPosition_modelspaceID = glGetAttribLocation(programID, "vertexPosition_modelspace");
    GLuint p_vertexUVID = glGetAttribLocation(programID, "vertexUV");

    // Load the texture
    GLuint p_Texture = loadBMP_custom("planeMonster.bmp");

    // Get a handle for our "myTextureSampler" uniform
    GLuint p_TextureID  = glGetUniformLocation(programID, "myTextureSampler");

    // Read our .obj file
    std::vector<glm::vec3> p_vertices;
    std::vector<glm::vec2> p_uvs;
    std::vector<glm::vec3> p_normals; // Won't be used at the moment.
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

    player->setIDs(p_vertexPosition_modelspaceID, p_vertexUVID, p_uvbuffer, p_vertexbuffer, 0, 0);
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

    GLuint b_vertexPosition_modelspaceID = glGetAttribLocation(programID, "vertexPosition_modelspace");

    GLuint b_vertexbuffer;
    glGenBuffers(1, &b_vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, b_vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

    //
    // The bullets ends
    // /////////////////////////////////////////////////////////////////////////////////////////////////

    // /////////////////////////////////////////////////////////////////////////////////////////////////
    // The Monster

    // Get a handle for our buffers
    GLuint m_vertexPosition_modelspaceID = glGetAttribLocation(programID, "vertexPosition_modelspace");
    GLuint m_vertexUVID = glGetAttribLocation(programID, "vertexUV");

    // Load the texture
    GLuint m_Texture = loadBMP_custom("Dropship_D.bmp");

    // Get a handle for our "myTextureSampler" uniform
    GLuint m_TextureID  = glGetUniformLocation(programID, "myTextureSampler");

    // Read our .obj file
    std::vector<glm::vec3> m_vertices;
    std::vector<glm::vec2> m_uvs;
    std::vector<glm::vec3> m_normals; // Won't be used at the moment.
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

    //
    // The monster ends
    // /////////////////////////////////////////////////////////////////////////////////////////////////

    // /////////////////////////////////////////////////////////////////////////////////////////////////
    // The Obstecles begins

    GLuint O_vertexPosition_modelspaceID = glGetAttribLocation(programID, "vertexPosition_modelspace");
    //GLuint O_vertexColorID = glGetAttribLocation(programID, "vertexColor");
    // Get a handle for our "myTextureSampler" uniform
    GLuint O_TextureID  = glGetUniformLocation(programID, "myTextureSampler");
    GLuint O_vertexUVID = glGetAttribLocation(programID, "vertexUV");

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

    explosion->setIDs(evertexPosition_modelspaceID, evertexUVID, e_uvbuffer, e_vertexbuffer, 0, 0);
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
    sprintf(text,"start game" );
    sprintf(text1,">exit" );
    sprintf(text2,">start game" );
    sprintf(text3,"exit" );

    //
    // The Game menu ends
    // /////////////////////////////////////////////////////////////////////////////////////////////////////

    ///  -----------------------------------------* Game Loop *-----------------------------------------

    printf("main::main() entering game loop\n");

    double lastTime = glfwGetTime(), deltaTime=0.0f;
    int bulletUpdates = 0;
    float posx, posy, posz=20;
    float obsteclesUpdate = 0;
    float distance = 0; //distance covered by player
    bool running = true;
    bool gameStarted = false;
    bool cursor = true;
    int deathTime = 0;
    bool collision = false;
    int monsterUpdate = 0;
    Level currentLevel;
    levelLoader.getCurrentLevel(currentLevel);


    //printf("%d\n", (int)currentLevel.obstcales);

    do{
        // Clear the screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Use our shader
        glUseProgram(programID);

        // render the scene
        scene->render(MatrixID, Projection, View);

        // game loop
        if (gameStarted)
        {
            // update frame time
            deltaTime += (glfwGetTime() - lastTime) *  UPDATES_PER_SECOND;
            lastTime = glfwGetTime();

            if (collision)
            {
                // render the death frame
                explosion->render(MatrixID, Projection, View);

                if (deltaTime >= 0.5f)
                {
                    deltaTime -= 0.5f;

                    deathTime++;

                    if (deathTime > 50)
                    {
                        collision = false;
                        deathTime = 0;
                    }
                }
            }
            else
            {
                // update distance
                distance += 0.5;
                //printf("distance covered: %d next level at %d\n", (int)distance, (int)currentLevel.distance);
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
                        running = false;
                    }
                }

                ///  -----------------------------------------* Update gameobjects state *---------------------------------
                //get user action and execute it
                if (deltaTime >= 0.5f)
                {
                    deltaTime-= 0.5f;

                    bulletUpdates++;
                    monsterUpdate++;
                    obsteclesUpdate++;

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
                            bullet->setIDs(b_vertexPosition_modelspaceID, 0, 0, b_vertexbuffer, 0, 0);

                            objects.push_back(bullet);
                        }
                    }

                    if (monsterUpdate > currentLevel.monsters)
                    {
                        // Generate the random positions for the enemy planes
                        posx = (rand() % (MAX_NEGATIVE_X * 2)) - MAX_POSITIVE_X;
                        posy = (rand() % ((MAX_POSITIVE_Y+2) *2)) - MAX_POSITIVE_Y - 2;

                        monsterUpdate -= currentLevel.monsters;
                        GameObject* monster = new Monster(vec3(posx, posy, posz));
                        monster->setIDs(m_vertexPosition_modelspaceID, m_vertexUVID, m_uvbuffer, m_vertexbuffer, 0, 0);
                        monster->setNormals(m_normals);
                        monster->setTexture(m_TextureID, m_Texture);
                        monster->setUVs(m_uvs);
                        monster->setVertices(m_vertices);

                        objects.push_back(monster);
                    }

                    if (obsteclesUpdate > currentLevel.obstcales)
                    {
                        //printf("obstecles %d\n", (int)currentLevel.obstcales);
                        // generate random position for the obstecle
                        posx = (rand() % (MAX_POSITIVE_X * 2)) - MAX_POSITIVE_X;
                        posy = (rand() % ((MAX_POSITIVE_Y+2) *2)) - MAX_POSITIVE_Y-2;
                        int i = rand() % 5;
                        GameObject *obstecle = new Obstecle(vec3(posx, posy, 20));
                        obstecle->setIDs(O_vertexPosition_modelspaceID, O_vertexUVID, O_uvbuffer[i], O_vertexbuffer[i], 0, 0);
                        obstecle->setNormals(O_normals[i]);
                        obstecle->setTexture(O_TextureID, O_Texture[rand() % 5]);
                        obstecle->setUVs(O_uvs[i]);
                        obstecle->setVertices(O_vertices[i]);

                        objects.push_back(obstecle);

                        obsteclesUpdate -= currentLevel.obstcales;
                    }

                    ///  -----------------------------------------* Rendering *-----------------------------------------

                    // Draw the player
                    player->render(MatrixID, Projection, View);

                    // Draw the objects
                    for (std::list<GameObject *>::iterator it = objects.begin(); it != objects.end(); it++)
                    {
                        (*it)->render(MatrixID, Projection, View);
                    }
                }
            }
        }
        else
        {
            // game menu
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

            }
        }

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
    glDeleteBuffers(1, &O_uvbuffer[0]);
    glDeleteBuffers(1, &O_uvbuffer[1]);
    glDeleteBuffers(1, &O_uvbuffer[2]);
    glDeleteBuffers(1, &O_uvbuffer[3]);
    glDeleteBuffers(1, &O_uvbuffer[4]);
    glDeleteBuffers(1, &m_vertexbuffer);
    glDeleteBuffers(1, &m_uvbuffer);
    glDeleteBuffers(1, &b_vertexbuffer);
    glDeleteBuffers(1, &p_uvbuffer);
    glDeleteBuffers(1, &p_vertexbuffer);
    glDeleteProgram(programID);
    glDeleteTextures(1, &O_Texture[0]);
    glDeleteTextures(1, &O_Texture[1]);
    glDeleteTextures(1, &O_Texture[2]);
    glDeleteTextures(1, &O_Texture[3]);
    glDeleteTextures(1, &O_Texture[4]);
    glDeleteTextures(1, &m_Texture);
    glDeleteTextures(1, &p_Texture);
    glDeleteTextures(1, &eTexture);

    // Delete the text's VBO, the shader and the texture
    cleanupText2D();

    // Close OpenGL window and terminate GLFW
    glfwTerminate();

    //delete objects
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


void removeUnessaryObjects(bool clear)
{
    if (clear)
    {
        for(std::list<GameObject* >::iterator it = objects.begin(); it != objects.end(); it++)
        {
            printf("%d", (*it)->getObjectType());
            deletedObjects.push_back(it);
        }
    }
    else
    {
        //check for dead bullets
        for(std::list<GameObject* >::iterator it = objects.begin(); it != objects.end(); it++)
        {
            if (!((*it)->isInRange()))
            {
                deletedObjects.push_back(it);
            }
        }
    }

    //remove dead obstecles
    for(std::list < std::list<GameObject* >::iterator >::iterator deleteIterator = deletedObjects.begin(); deleteIterator != deletedObjects.end(); deleteIterator++)
    {
        GameObject * object =  **deleteIterator;
        objects.erase(*deleteIterator);
        delete object;

    }

    deletedObjects.clear();
}


void delay(int milli)
{
    for (int i=0; i<milli; i++)
    {
        for (int j=0; j<milli; j++);
    }
}
