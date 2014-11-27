#include "engine.h"

#define UPDATES_PER_SECOND 60



Engine::Engine()
{
    this->player = new Player(CENTER, vec3(0, 0, 0));
}

Engine::~Engine()
{
    delete player;
}

/////////////////////////////
/// \brief Engine::render
///render game objects
void Engine::render()
{

    player->render();

    for(std::vector<Bullet *>::iterator it = bullets.begin(); it != bullets.end(); it++)
    {
        (*it)->render();
    }

    glDisableVertexAttribArray(vertexPosition_modelspaceID);
    glDisableVertexAttribArray(vertexColorID);

    // Swap buffers
    glfwSwapBuffers();
}
/////////////////////////////
/// \brief Engine::run
///run game logic
void Engine::run()
{
    static const GLfloat g_color_buffer_data[] = {
                0.583f,  0.771f,  0.014f,
                0.583f,  0.771f,  0.014f,
                0.583f,  0.771f,  0.014f,
                1.0f,  0.17f,  0.014f,
                1.0f,  0.17f,  0.014f,
                1.0f,  0.17f,  0.014f,
                0.583f,  0.771f,  1.0f,
                0.583f,  0.771f,  1.0f,
                0.583f,  0.771f,  1.0f,
                0.014f, 0.583f,  0.771f,
                0.014f, 0.583f,  0.771f,
                0.014f, 0.583f,  0.771f,
                1.0f,  0.0f,  0.0f,
                1.0f,  0.0f,  0.0f,
                1.0f,  0.0f,  0.0f,
                0.483f,  0.596f,  0.789f,
                0.559f,  0.861f,  0.639f,
                0.195f,  0.548f,  0.859f,
                0.997f,  0.513f,  0.064f,
                0.945f,  0.719f,  0.592f,
                0.543f,  0.021f,  0.978f,
                0.279f,  0.317f,  0.505f,
                0.167f,  0.620f,  0.077f,
                0.347f,  0.857f,  0.137f,
            1.0f,  0.184f,  0.576f,
            1.0f,  0.184f,  0.576f,
            1.0f,  0.184f,  0.576f,
            0.014f,  0.184f,  0.576f,
            0.014f,  0.184f,  0.576f,
            0.014f,  0.184f,  0.576f,
            0.014f,  1.0f,  0.576f,
            0.014f,  1.0f,  0.576f,
            0.014f,  1.0f,  0.576f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
    };

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

    if (setupGL(g_color_buffer_data, g_vertex_buffer_data))
    {

        double lastTime = glfwGetTime(), deltaTime=0.0f;
        while (glfwGetKey( GLFW_KEY_ESC ) != GLFW_PRESS &&
               glfwGetWindowParam( GLFW_OPENED ))
            //gameloop
        {

            deltaTime += (glfwGetTime() - lastTime) *  UPDATES_PER_SECOND;
            lastTime = glfwGetTime();
            //get user action and execute it
            if (deltaTime >= 1.0f)
            {
                update();
                deltaTime--;
            }

            //render to screen
            render();

        }
    }

    // Cleanup VBO and shader
    glDeleteBuffers(1, &vertexbuffer);
    glDeleteBuffers(1, &colorbuffer);
    glDeleteProgram(programID);

    // Close OpenGL window and terminate GLFW
    glfwTerminate();
}
/////////////////////////////
/// \brief Engine::setupGL
///set window
bool Engine::setupGL(const GLfloat g_color_buffer_data[], const GLfloat g_vertex_buffer_data[])
{
    // Initialise GLFW
    if( !glfwInit() )
    {
        fprintf( stderr, "Failed to initialize GLFW\n" );
        return false;
    }

    glfwOpenWindowHint(GLFW_FSAA_SAMPLES, 4);
    glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, 2);
    glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, 1);

    // Open a window and create its OpenGL context
    if( !glfwOpenWindow( 1024, 768, 0,0,0,0, 32,0, GLFW_WINDOW ) )
    {
        fprintf( stderr, "Failed to open GLFW window.\n" );
        glfwTerminate();
        return false;
    }

    // Initialize GLEW
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        return false;
    }

    glfwSetWindowTitle( "1" );

    // Ensure we can capture the escape key being pressed below
    glfwEnable( GLFW_STICKY_KEYS );
    // Dark blue background
    glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    // Accept fragment if it closer to the camera than the former one
    glDepthFunc(GL_LESS);

    // Create and compile our GLSL program from the shaders
    programID = LoadShaders( "TransformVertexShader.vertexshader", "ColorFragmentShader.fragmentshader" );
    // Get a handle for our "MVP" uniform
    // Model matrix gives the coordinates and transformation of objects in global coordinates
    // returns an integer that represents the location of a specific uniform variable
    // Get a handle for our "MVP" uniform.
    // Only at initialisation time.
    MatrixID = glGetUniformLocation(programID, "MVP");

    // Get a handle for our buffers
    vertexPosition_modelspaceID = glGetAttribLocation(programID, "vertexPosition_modelspace");
    vertexColorID = glGetAttribLocation(programID, "vertexColor");

    glGenBuffers(1, &vertexbuffer);//generate one buffer and let vertex point at it
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 36, g_vertex_buffer_data, GL_STATIC_DRAW);

    glGenBuffers(1, &colorbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 36, g_color_buffer_data, GL_STATIC_DRAW);

    // Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
    Projection = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 100.0f);
    // Camera matrix
    View       = glm::lookAt(
                 glm::vec3(0,10,-10), //position
                 glm::vec3(0,0,0), // and looks at the origin
                 glm::vec3(0,1,0)  // Head is up
                 );
    return true;
}
/////////////////////////////
/// \brief Engine::update
/// update gameobjects states
void Engine::update()
{
    player->update();

    for(std::vector<Bullet *>::iterator it = bullets.begin(); it != bullets.end(); it++)
    {
        (*it)->update();
    }
}
