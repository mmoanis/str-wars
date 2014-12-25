// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <math.h>
#include <memory>

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
#include <common/controls.hpp>
#include <common/objloader.hpp>
#include <common/text2D.hpp>

#include "player.h"
#include "bullet.h"
#include "scene.h"
#include "monster.h"
#include "obstecle.h"

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
                 glm::vec3(0,2,-5), //position
                 glm::vec3(0,0,0), // and looks at the origin
                 glm::vec3(0,1,0)  // Head is up
                 );

    printf("main::main() finished initialization\n");

    /// ----------------------------------------* GameObjects *------------------------------------------

    // /////////////////////////////////////////////////////////////////////////////////////////////////
    // The Scene
    printf("main::main() making scene\n");

    Scene * scene = new Scene();
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
    // The player
    printf("main::main(): making player\n");

    // Define our player
    Player * player = new Player(vec3(0, 0, 0));

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

    std::vector<Bullet *> bullets;

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

    std::vector<Monster *> monsters;

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

    std::vector<Obstecle *> obstecles;

    //
    // The Obstecles ends
    // /////////////////////////////////////////////////////////////////////////////////////////////////

    // /////////////////////////////////////////////////////////////////////////////////////////////////
    // The Game menu

    // Initialize our little text library with the Holstein font
    initText2D( "Holstein.DDS" );

    double xpos, ypos;
    char text[256];

    char text1[256];

    //
    // The Game menu ends
    // /////////////////////////////////////////////////////////////////////////////////////////////////////

    ///  -----------------------------------------* Game Loop *-----------------------------------------

    printf("main::main() entering game loop\n");

    double lastTime = glfwGetTime(), deltaTime=0.0f;
    int bulletUpdates = 0;
    bool addMonster = true;
    float posx, posy, posz=20;
    int numberOfObstecles = 2;
    float obsteclesUpdate = 0;
    float distance = 0; //distance covered by player
    bool running = true;
    bool gameStarted = false;
    bool cursor = true;

    std::vector < std::vector<Obstecle* >::iterator > deletedObstecles;
    std::vector < std::vector<Bullet* >::iterator > deletedBullets;
    std::vector < std::vector<Monster* >::iterator > deletedMonsters;

    std::vector < GameObject * > * gameObjects = new std::vector < GameObject * > ();

    do{

        // Clear the screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Use our shader
        glUseProgram(programID);



        if (gameStarted)
        {

            deltaTime += (glfwGetTime() - lastTime) *  UPDATES_PER_SECOND;
            lastTime = glfwGetTime();

            ///  -----------------------------------------* Update gameobjects state *---------------------------------
            //get user action and execute it
            if (deltaTime >= 1.0f)
            {
                deltaTime-= 1.0f;
                bulletUpdates++;

                // update bullets
                for (std::vector<Bullet *> ::iterator it = bullets.begin(); it != bullets.end(); it++)
                {
                    gameObjects->push_back((GameObject *) (*it));
                }

                // update monsters
                for (std::vector<Monster*>::iterator it = monsters.begin(); it != monsters.end(); it++)
                {
                    gameObjects->push_back((GameObject *) (*it));
                }

                // update obstecles
                for (std::vector<Obstecle*>::iterator it = obstecles.begin(); it != obstecles.end(); it++)
                {
                    gameObjects->push_back((GameObject *) (*it));
                }

                if (! (player->update(window, gameObjects)))
                {
                    running = false;
                    while(1);
                }

                // update bullets
                for (std::vector<Bullet *> ::iterator it = bullets.begin(); it != bullets.end(); it++)
                {
                    (*it)->update(window, NULL);
                }

                // update monsters
                for (std::vector<Monster*>::iterator it = monsters.begin(); it != monsters.end(); it++)
                {
                    if (! ((*it)->update(window, gameObjects)) )
                    {
                        deletedMonsters.push_back(it);
                        addMonster = true;
                    }
                }

                // update obstecles
                for (std::vector<Obstecle*>::iterator it = obstecles.begin(); it != obstecles.end(); it++)
                {
                    (*it)->update(window, gameObjects);
                }

                gameObjects->clear();

                /// removing dead* objects
                ///
                ///
                ///


                //check for dead bullets
                for(std::vector<Bullet* >::iterator it = bullets.begin(); it != bullets.end(); it++)
                {
                    if (!((*it)->isInRange()))
                    {
                        deletedBullets.push_back(it);
                    }
                }

                //check for dead monsters
                for(std::vector<Monster* >::iterator it = monsters.begin(); it != monsters.end(); it++)
                {
                    if (!((*it)->isInRange()))
                    {
                        deletedMonsters.push_back(it);
                        addMonster = true;
                    }
                    else
                        addMonster = false;
                }

                //check for dead obstecles
                for(std::vector<Obstecle*>::iterator it = obstecles.begin(); it != obstecles.end(); it++)
                {
                    if (!((*it)->isInRange()))
                    {
                        deletedObstecles.push_back(it);
                        numberOfObstecles--;
                    }
                }

                //remove dead bullets
                for(std::vector < std::vector<Bullet* >::iterator >::iterator deleteIterator = deletedBullets.begin(); deleteIterator != deletedBullets.end(); deleteIterator++)
                {
                    Bullet * bullet = **deleteIterator;
                    bullets.erase(*deleteIterator);
                    delete bullet;
                }

                //remove dead monsters
                for(std::vector < std::vector<Monster* >::iterator >::iterator deleteIterator = deletedMonsters.begin(); deleteIterator != deletedMonsters.end(); deleteIterator++)
                {
                    Monster * monster = **deleteIterator;
                    monsters.erase(*deleteIterator);
                    delete monster;
                }

                //remove dead obstecles
                for(std::vector < std::vector<Obstecle* >::iterator >::iterator deleteIterator = deletedObstecles.begin(); deleteIterator != deletedObstecles.end(); deleteIterator++)
                {
                    Obstecle * obstecle = **deleteIterator;
                    obstecles.erase(*deleteIterator);
                    delete obstecle;
                }

                deletedObstecles.clear();
                deletedBullets.clear();
                deletedMonsters.clear();


                /// add new objects
                ///
                // check to add new monster and player
                if (bulletUpdates >= 20)
                {
                    bulletUpdates -= 20;

                    if (glfwGetKey( window, GLFW_KEY_SPACE) == GLFW_PRESS )
                    {
                        Bullet * bullet = new Bullet(player->getPosition());
                        bullet->setIDs(b_vertexPosition_modelspaceID, 0, 0, b_vertexbuffer, 0, 0);

                        bullets.push_back(bullet);
                    }
                }

                // Generate the random positions for the enemy planes
                posx = (rand() % (MAX_NEGATIVE_X * 2)) - MAX_POSITIVE_X;
                posy = (rand() % ((MAX_POSITIVE_Y+2) *2)) - MAX_POSITIVE_Y - 2;

                if (addMonster)
                {
                    Monster* monster = new Monster(vec3(posx, posy, posz));
                    monster->setIDs(m_vertexPosition_modelspaceID, m_vertexUVID, m_uvbuffer, m_vertexbuffer, 0, 0);
                    monster->setNormals(m_normals);
                    monster->setTexture(m_TextureID, m_Texture);
                    monster->setUVs(m_uvs);
                    monster->setVertices(m_vertices);

                    monsters.push_back(monster);
                }

                obsteclesUpdate++;

                if (obsteclesUpdate > 80)
                {
                    ///TODO: fix the y-axis random number
                    posx = (rand() % (MAX_POSITIVE_X * 2)) - MAX_POSITIVE_X;
                    posy = (rand() % ((MAX_POSITIVE_Y+2) *2)) - MAX_POSITIVE_Y-2;
                    int i = rand() % 5;
                    Obstecle *obstecle = new Obstecle(vec3(posx, posy, 40));
                    obstecle->setIDs(O_vertexPosition_modelspaceID, O_vertexUVID, O_uvbuffer[i], O_vertexbuffer[i], 0, 0);
                    obstecle->setNormals(O_normals[i]);
                    obstecle->setTexture(O_TextureID, O_Texture[rand() % 5]);
                    obstecle->setUVs(O_uvs[i]);
                    obstecle->setVertices(O_vertices[i]);

                    obstecles.push_back(obstecle);

                    numberOfObstecles ++;

                    obsteclesUpdate -= 80;
                }

                ///  -----------------------------------------* Rendering *-----------------------------------------

                // Draw the objects
                player->render(MatrixID, Projection, View);
                scene->render(MatrixID, Projection, View);

                // Draw the monsters
                for (std::vector<Monster *>::iterator it = monsters.begin(); it != monsters.end(); it++)
                {
                    (*it)->render(MatrixID, Projection, View);
                }

                // Draw the bullets
                for (std::vector<Bullet *>::iterator it = bullets.begin(); it != bullets.end(); it++)
                {
                    (*it)->render(MatrixID, Projection, View);
                }

                // Draw the obstecles
                for(std::vector<Obstecle *>::iterator it = obstecles.begin(); it != obstecles.end(); it++)
                {
                    (*it)->render(MatrixID, Projection, View);
                }
            }

        }
        else
        {
            // game menu
            if (cursor)
            {
                sprintf(text,">start game" );
            sprintf(text1,"exit" );
            }
            else
            {
                sprintf(text,"start game" );
            sprintf(text1,">exit" );
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
                // Delete the text's VBO, the shader and the texture
                cleanupText2D();
            }

            printText2D(text, 50, 500, 60);
            printText2D(text1, 60, 100, 60);
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



    // Close OpenGL window and terminate GLFW
    glfwTerminate();

    //delete objects

    delete gameObjects;
    delete scene;
    delete player;

    // delete the monsters
    for (std::vector<Monster *>::iterator it = monsters.begin(); it != monsters.end(); it++)
    {
        delete (*it);
    }

    // delete the bullets
    for (std::vector<Bullet *>::iterator it = bullets.begin(); it != bullets.end(); it++)
    {
        delete (*it);
    }

    // delete the obstecles
    for(std::vector<Obstecle *>::iterator it = obstecles.begin(); it != obstecles.end(); it++)
    {
        delete (*it);
    }

    return 0;
}

