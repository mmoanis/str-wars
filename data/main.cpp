// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <math.h>

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
                 glm::vec3(0,2,-10), //position
                 glm::vec3(0,0,0), // and looks at the origin
                 glm::vec3(0,1,0)  // Head is up
                 );

    printf("main::main() finished initialization\n");

    /// ----------------------------------------* GameObjects *------------------------------------------

    // /////////////////////////////////////////////////////////////////////////////////////////////////
    // The Scene
    printf("main::main(): making scene\n");

    Scene * scene = new Scene();
    GLuint vertexPosition_modelspaceID = glGetAttribLocation(programID, "vertexPosition_modelspace");
    GLuint vertexUVID = glGetAttribLocation(programID, "vertexUV");
   // Load the texture
    GLuint Tex = loadBMP_custom("download.bmp");//Try these images: colors.bmp, parts.bmp

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
    Player * player = new Player(CENTER, vec3(0, 0, 0));

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

    // One color for each vertex. They were generated randomly.
    static const GLfloat g_color_buffer_data[] = {
            0.583f,  0.771f,  0.014f,
            0.609f,  0.115f,  0.436f,
            0.327f,  0.483f,  0.844f,
            0.822f,  0.569f,  0.201f,
            0.435f,  0.602f,  0.223f,
            0.310f,  0.747f,  0.185f,
            0.597f,  0.770f,  0.761f,
            0.559f,  0.436f,  0.730f,
            0.359f,  0.583f,  0.152f,
            0.483f,  0.596f,  0.789f,
            0.559f,  0.861f,  0.639f,
            0.195f,  0.548f,  0.859f,
            0.014f,  0.184f,  0.576f,
            0.771f,  0.328f,  0.970f,
            0.406f,  0.615f,  0.116f,
            0.676f,  0.977f,  0.133f,
            0.971f,  0.572f,  0.833f,
            0.140f,  0.616f,  0.489f,
            0.997f,  0.513f,  0.064f,
            0.945f,  0.719f,  0.592f,
            0.543f,  0.021f,  0.978f,
            0.279f,  0.317f,  0.505f,
            0.167f,  0.620f,  0.077f,
            0.347f,  0.857f,  0.137f,
            0.055f,  0.953f,  0.042f,
            0.714f,  0.505f,  0.345f,
            0.783f,  0.290f,  0.734f,
            0.722f,  0.645f,  0.174f,
            0.302f,  0.455f,  0.848f,
            0.225f,  0.587f,  0.040f,
            0.517f,  0.713f,  0.338f,
            0.053f,  0.959f,  0.120f,
            0.393f,  0.621f,  0.362f,
            0.673f,  0.211f,  0.457f,
            0.820f,  0.883f,  0.371f,
            0.982f,  0.099f,  0.879f
    };

    GLuint b_vertexPosition_modelspaceID = glGetAttribLocation(programID, "vertexPosition_modelspace");
    GLuint b_vertexColorID = glGetAttribLocation(programID, "vertexColor");

    GLuint b_vertexbuffer;
    glGenBuffers(1, &b_vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, b_vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

    GLuint b_colorbuffer;
    glGenBuffers(1, &b_colorbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, b_colorbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_color_buffer_data), g_color_buffer_data, GL_STATIC_DRAW);

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
    GLuint O_Texture = loadBMP_custom("obstcale.bmp");

    // Read our .obj file
    std::vector<glm::vec3> O_vertices;
    std::vector<glm::vec2> O_uvs;
    std::vector<glm::vec3> O_normals; // Won't be used at the moment.
    loadOBJ("obstcale.obj", O_vertices, O_uvs, O_normals);

    // Load it into a VBO
    GLuint O_vertexbuffer;
    glGenBuffers(1, &O_vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, O_vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, O_vertices.size() * sizeof(glm::vec3), &O_vertices[0], GL_STATIC_DRAW);

    GLuint O_uvbuffer;
    glGenBuffers(1, &O_uvbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, O_uvbuffer);
    glBufferData(GL_ARRAY_BUFFER, O_uvs.size() * sizeof(glm::vec2), &O_uvs[0], GL_STATIC_DRAW);


    std::vector<Obstecle *> obstecles;

    //
    // The Obstecles ends
    // /////////////////////////////////////////////////////////////////////////////////////////////////

    ///  -----------------------------------------* Game Loop *-----------------------------------------

    printf("main::main() entering game loop\n");

    double lastTime = glfwGetTime(), deltaTime=0.0f;
    int bulletUpdates = 0;
    bool addMonster = true;
    float posx, posy, posz=20;
    int numberOfObstecles = 0;
    do{

        // Clear the screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Use our shader
        glUseProgram(programID);

        deltaTime += (glfwGetTime() - lastTime) *  UPDATES_PER_SECOND;
        lastTime = glfwGetTime();

        ///  -----------------------------------------* Update gameobjects state *---------------------------------
        //get user action and execute it
        if (deltaTime >= 3.0f)
        {
            player->update(window);

            deltaTime-= 3.0f;
            bulletUpdates++;

            // update bullets
            for (std::vector<Bullet *>::iterator it = bullets.begin(); it != bullets.end(); it++)
            {
                (*it)->update(window);
            }

            // update monsters
            for (std::vector<Monster *>::iterator it = monsters.begin(); it != monsters.end(); it++)
            {
                (*it)->update(window);
            }

            // update obstecles
            for (std::vector<Obstecle *>::iterator it = obstecles.begin(); it != obstecles.end(); it++)
            {
                (*it)->update(window);
            }

            // check to add new monster and player
            if (bulletUpdates >= 5)
            {
                bulletUpdates -= 5;

                if (glfwGetKey( window, GLFW_KEY_SPACE) == GLFW_PRESS )
                {
                    Bullet * bullet = new Bullet(player->getLane(), player->getPosition());
                    bullet->setIDs(b_vertexPosition_modelspaceID, 0, 0, b_vertexbuffer, b_vertexColorID, b_colorbuffer);

                    bullets.push_back(bullet);
                }
            }

            // Generate the random positions for the enemy planes
            posx = (rand() % (MAX_NEGATIVE_X * 2)) - MAX_POSITIVE_X;
            posy = (rand() % (MAX_POSITIVE_Y+2)) - 3;   ///TODO: fix the y-axis random number

            if (addMonster)
            {
                Monster * monster = new Monster(CENTER, vec3(posx, posy, posz));
                monster->setIDs(m_vertexPosition_modelspaceID, m_vertexUVID, m_uvbuffer, m_vertexbuffer, 0, 0);
                monster->setNormals(m_normals);
                monster->setTexture(m_TextureID, m_Texture);
                monster->setUVs(m_uvs);
                monster->setVertices(m_vertices);

                monsters.push_back(monster);
            }

            //numberOfObstecles = rand() % 3;


            if (numberOfObstecles < 2)
            {
                posx = (rand() % (MAX_NEGATIVE_X * 2)) - MAX_POSITIVE_X;
                posy = (rand() % (MAX_POSITIVE_Y+2)) - 3;   ///TODO: fix the y-axis random number

                Obstecle * obstecle = new Obstecle(CENTER, vec3(posx, posy, posz));
                obstecle->setIDs(O_vertexPosition_modelspaceID, O_vertexUVID, O_uvbuffer, O_vertexbuffer, 0, 0);
                obstecle->setNormals(O_normals);
                obstecle->setTexture(O_TextureID, O_Texture);
                obstecle->setUVs(O_uvs);
                obstecle->setVertices(O_vertices);

                obstecles.push_back(obstecle);

                numberOfObstecles ++;
            }
        }

        ///  -----------------------------------------* Rendering *-----------------------------------------

        // Draw the objects
        player->render(MatrixID, Projection, View);
        scene->render(MatrixID, Projection, View);

        //check for dead bullets
        std::vector < std::vector<Monster *>::iterator > deletedMonsters;
        for(std::vector<Monster *>::iterator it = monsters.begin(); it != monsters.end(); it++)
        {
            if (!((*it)->isInRange()))
            {
                delete (*it);
                deletedMonsters.push_back(it);
                addMonster = true;
            }
            else
                addMonster = false;
        }

        //remove dead monsters
        for(std::vector < std::vector<Monster *>::iterator >::iterator deleteIterator = deletedMonsters.begin(); deleteIterator != deletedMonsters.end(); deleteIterator++)
        {
            monsters.erase(*deleteIterator);
        }

        // Draw the bullets
        for (std::vector<Monster *>::iterator it = monsters.begin(); it != monsters.end(); it++)
        {
            (*it)->render(MatrixID, Projection, View);
        }

        //check for dead bullets
        std::vector < std::vector<Bullet *>::iterator > deletedBullets;
        for(std::vector<Bullet *>::iterator it = bullets.begin(); it != bullets.end(); it++)
        {
            if (!((*it)->isInRange()))
            {
                delete (*it);
                deletedBullets.push_back(it);
            }
        }

        //remove dead bullets
        for(std::vector < std::vector<Bullet *>::iterator >::iterator deleteIterator = deletedBullets.begin(); deleteIterator != deletedBullets.end(); deleteIterator++)
        {
            bullets.erase(*deleteIterator);
        }

        // Draw the bullets
        for (std::vector<Bullet *>::iterator it = bullets.begin(); it != bullets.end(); it++)
        {
            (*it)->render(MatrixID, Projection, View);
        }

        //check for dead obstecles
        std::vector < std::vector<Obstecle *>::iterator > deletedObstecles;

        for(std::vector<Obstecle *>::iterator it = obstecles.begin(); it != obstecles.end(); it++)
        {
            if (!((*it)->isInRange()))
            {
                delete (*it);
                deletedObstecles.push_back(it);
                numberOfObstecles--;
            }
        }

        //remove dead obstecles
        for(std::vector < std::vector<Obstecle *>::iterator >::iterator deleteIterator = deletedObstecles.begin(); deleteIterator != deletedObstecles.end(); deleteIterator++)
        {
            obstecles.erase(*deleteIterator);
        }

        // Draw the obstecles
        for(std::vector<Obstecle *>::iterator it = obstecles.begin(); it != obstecles.end(); it++)
        {
            (*it)->render(MatrixID, Projection, View);
        }

        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();

    } // Check if the ESC key was pressed or the window was closed
    while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
           glfwWindowShouldClose(window) == 0 );

    /// Termination

    printf("main::main() terminating\n");

    // Cleanup VBO and shader
    player->releaseResources();
    for (std::vector<Bullet *>::iterator it = bullets.begin(); it != bullets.end(); it++)
    {
        (*it)->releaseResources();
    }
    for (std::vector<Monster *>::iterator it = monsters.begin(); it != monsters.end(); it++)
    {
        (*it)->releaseResources();
    }
    for (std::vector<Obstecle *>::iterator it = obstecles.begin(); it != obstecles.end(); it++)
    {
        (*it)->releaseResources();
    }
    glDeleteProgram(programID);
    player->releaseTexture();
    for (std::vector<Monster *>::iterator it = monsters.begin(); it != monsters.end(); it++)
    {
        (*it)->releaseTexture();
    }
    for (std::vector<Obstecle *>::iterator it = obstecles.begin(); it != obstecles.end(); it++)
    {
        (*it)->releaseTexture();
    }

    // Close OpenGL window and terminate GLFW
    glfwTerminate();

    //delete objects
    delete scene;
    delete player;

    for (std::vector<Bullet *>::iterator it = bullets.begin(); it != bullets.end(); it++)
    {
        delete (*it);
    }
    for (std::vector<Monster *>::iterator it = monsters.begin(); it != monsters.end(); it++)
    {
        delete (*it);
    }
    for (std::vector<Obstecle *>::iterator it = obstecles.begin(); it != obstecles.end(); it++)
    {
        delete (*it);
    }

    return 0;
}
