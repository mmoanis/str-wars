#include "engine.h"

Engine::Engine()
{
    this->player = new Player(CENTER, vec3(0, 0, 0));
    updateSensitivity = 0;
}

Engine::~Engine()
{
    delete player;

    //delete remaining bullets
    for(std::vector<Bullet *>::iterator it = bullets.begin(); it != bullets.end(); it++)
    {
        delete (*it);
    }
}

//render objects to screen
void Engine::render()
{

    // Clear the screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Use our shader
    glUseProgram(programID);

    //draw player
    player->render(MatrixID, Projection, View);

    //render player bullets
    //draw any bullets fired
    for(std::vector<Bullet *>::iterator it = bullets.begin(); it != bullets.end(); it++)
    {
        (*it)->render(MatrixID, Projection, View);
    }

    // Swap buffers
    glfwSwapBuffers(window);
}

//run game logic
void Engine::run()
{
    if (setupGL())
    {
        //initialize gameobjects
        player->setup(programID);

        double lastTime = glfwGetTime(), deltaTime=0.0f;

        //gameloop
        do
        {

            deltaTime += (glfwGetTime() - lastTime) *  UPDATES_PER_SECOND;
            lastTime = glfwGetTime();
            //get user action and execute it
            if (deltaTime >= 3.0f)
            {
                update();
                deltaTime-= 3.0f;
            }

            //render to screen
            render();
        }
        // Check if the ESC key was pressed or the window was closed
        while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
               glfwWindowShouldClose(window) == 0 );
    }

    // Cleanup VBO and shader
    glDeleteProgram(programID);

    // Close OpenGL window and terminate GLFW
    glfwTerminate();
}

//setup gl and open window
bool Engine::setupGL()
{
    // Initialise GLFW
    if( !glfwInit() )
    {
        fprintf( stderr, "Failed to initialize GLFW\n" );
        return false;
    }

    // Open a window and create its OpenGL context
    window = glfwCreateWindow( 1024, 768, "Tutorial 07 - Model Loading", NULL, NULL);
    if( window == NULL ){
        fprintf( stderr, "Failed to open GLFW window.\n" );
        glfwTerminate();
        return false;
    }
    glfwMakeContextCurrent(window);

    // Initialize GLEW
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        return false;
    }

    glfwSetWindowTitle(window, "str-wars" );

    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    //glfwSetCursorPos(window, 1024/2, 768/2);

    // Dark blue background
    glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    // Accept fragment if it closer to the camera than the former one
    glDepthFunc(GL_LESS);

    // Cull triangles which normal is not towards the camera
    glEnable(GL_CULL_FACE);

    // Create and compile our GLSL program from the shaders
    programID = LoadShaders( "TransformVertexShader.vertexshader", "ColorFragmentShader.fragmentshader" );

    // Get a handle for our "MVP" uniform
    // Model matrix gives the coordinates and transformation of objects in global coordinates
    // returns an integer that represents the location of a specific uniform variable
    // Get a handle for our "MVP" uniform.
    // Only at initialisation time.
    MatrixID = glGetUniformLocation(programID, "MVP");

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

//update gameobjects status
void Engine::update()
{
    //update player status
    player->update(window);

    updateSensitivity++;


    if (updateSensitivity > 10)
    {
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS )
        {
             //fire bullets
            Bullet * bullet = new Bullet(player->getLane(), player->getPosition());
            bullet->setup(programID);
            bullets.push_back(bullet);
        }
        updateSensitivity -= 10;
    }
    //check for dead bullets
    std::vector < std::vector<Bullet *>::iterator > deletedBullets;
    for(std::vector<Bullet *>::iterator it = bullets.begin(); it != bullets.end(); it++)
    {
        if (!((*it)->isInRange()))
            deletedBullets.push_back(it);
    }

    for(std::vector < std::vector<Bullet *>::iterator >::iterator deleteIterator = deletedBullets.begin(); deleteIterator != deletedBullets.end(); deleteIterator++)
    {
        //remove dead bullets
        bullets.erase(*deleteIterator);
    }

    //update bullets
    for(std::vector<Bullet *>::iterator it = bullets.begin(); it != bullets.end(); it++)
    {
        (*it)->update(window);
    }

    printf("Engine::update() number of bullets alive:%d\n", bullets.size());
}
