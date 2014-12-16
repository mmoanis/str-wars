#include "player.h"

// Constructor
Player::Player(Lane lane, vec3 position): GameObject(lane, position, PLAYER)
{
    printf("Player::Player() at x=%d y=%d z=%d\n", (int)position.x, (int)position.y, (int)position.z);

    _health = 100;
    anglex = 0.0;
    anglez = 0;
    angley = 3.14;
    scalex = scaley = scalez = 0.002;

    RotationMatrix = eulerAngleYXZ(angley, anglex, anglez);//yaw, pitch and roll. Measured in radians
    //ScalingMatrix = scale(mat4(), vec3(0.05f, 0.05f, 0.05f));
    ScalingMatrix = scale(mat4(), vec3(scalex, scaley, scalez));
}

// Destructor
Player::~Player()
{
    printf("Player::~Player() destructed\n");
}

// Check collision with other objects
bool Player::checkCollision(GameObject *)
{
    // TODO: implement collision detection
    return false;
}

// Initialize object state
// @Deprecated: use constructor instead
void Player::setup()
{
}

// Draws the player on the screen
void Player::render(const GLuint &MatrixID, const mat4 &Projection, const mat4 &View)
{
    //make the matrices for the transformation
    TranslationMatrix = translate(mat4(), vec3((int)_position.x, (int)_position.y,(int)_position.z));

    glm::mat4 Model = TranslationMatrix* RotationMatrix* ScalingMatrix;//order of multiplication is important (try different values above and different order of multiplication)

    //make the MVP matix
    glm::mat4 MVP        = Projection * View * Model;

    // Send our transformation to the currently bound shader,
    // in the "MVP" uniform
    // For each object you render, since the MVP will be different (at least the Model part)
    glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

    // Bind our texture in Texture Unit 0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    // Set our "myTextureSampler" sampler to user Texture Unit 0
    glUniform1i(textureID, 0);

    // 1rst attribute buffer : vertices
    glEnableVertexAttribArray(vertexPosition_modelspaceID);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glVertexAttribPointer(
        vertexPosition_modelspaceID,  // The attribute we want to configure
        3,                            // size
        GL_FLOAT,                     // type
        GL_FALSE,                     // normalized?
        0,                            // stride
        (void*)0                      // array buffer offset
    );

    // 2nd attribute buffer : UVs
    glEnableVertexAttribArray(vertexUVID);
    glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
    glVertexAttribPointer(
        vertexUVID,                   // The attribute we want to configure
        2,                            // size : U+V => 2
        GL_FLOAT,                     // type
        GL_FALSE,                     // normalized?
        0,                            // stride
        (void*)0                      // array buffer offset
    );

    // Draw the triangleS !
    glDrawArrays(GL_TRIANGLES, 0, _vertices.size()); // 12*3 indices starting at 0 -> 12 triangles

    glDisableVertexAttribArray(vertexPosition_modelspaceID);
    glDisableVertexAttribArray(vertexUVID);
}

// Update the player states
void Player::update(GLFWwindow* window)
{
    //get user input
    if (glfwGetKey( window, GLFW_KEY_LEFT) == GLFW_PRESS ) //left arrow is pressed, so move the traingle left
    {
        if (_position.x < MAX_POSITIVE_X)
        {
            _position.x++;
            anglez += 0.1;
        }
    }
    else if (glfwGetKey( window, GLFW_KEY_RIGHT) == GLFW_PRESS )
    {
        if ( _position.x > MAX_NEGATIVE_X )
        {
            _position.x--;
            anglez -= 0.1;
        }
    }
    else if (glfwGetKey( window, GLFW_KEY_W) == GLFW_PRESS )
    {
        if ( _position.z <= MAX_POSITIVE_Z )
        {
            _position.z++;
            if (anglex < 0.5)
                anglex += 0.05;
        }
    }
    else if (glfwGetKey( window, GLFW_KEY_S) == GLFW_PRESS )
    {
        if (_position.z >= MAX_NEGATIVE_Z)
        {
            _position.z--;
            if ( anglex > 0)
                anglex -= 0.1;
        }
    }
    else if (glfwGetKey( window, GLFW_KEY_UP) == GLFW_PRESS )
    {
        if ( _position.y <= MAX_POSITIVE_Y )
        {
            _position.y++;
        }
    }
    else if (glfwGetKey( window, GLFW_KEY_DOWN) == GLFW_PRESS )
    {
        if (_position.y >= MAX_NEGATIVE_Y)
        {
            _position.y--;
        }
    }
    RotationMatrix = eulerAngleYXZ(angley, anglex, anglez);//yaw, pitch and roll. Measured in radians
}
