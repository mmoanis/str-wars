#include "player.h"

// Constructor
Player::Player(vec3 position): GameObject(position, PLAYER)
{
    printf("Player::Player() at x=%d y=%d z=%d\n", (int)position.x, (int)position.y, (int)position.z);

    //_health = 100;
    anglex = 0;
    anglez = 0;
    angley = 3.14f;
    inRange = true;
    scalex = scaley = scalez = 0.002f;

    RotationMatrix = eulerAngleYXZ(angley, anglex, anglez);//yaw, pitch and roll. Measured in radians
    ScalingMatrix = scale(mat4(), vec3(scalex, scaley, scalez));

    // set the collider
    this->collider.sizex = 0.38f;
    this->collider.sizey = 0.05f;
    this->collider.sizez = 0.3f

            ;
}

// Destructor
Player::~Player()
{
    printf("Player::~Player() destructed\n");
}

// Check collision with other objects
bool Player::checkCollision(GameObject * other)
{
    Collider collider = other->getCollider();
    vec3 position = other ->getPosition();

    // uncomment in debuging
    printf("\n%f %f %f\n", (float)_position.x, (float)position.x, (float)collider.sizex + (float)this->collider.sizex);
    printf("%f %f %f\n", (float)_position.y, (float)position.y, (float)collider.sizey + this->collider.sizey);
    printf("%f %f %f\n", (float)_position.z, (float)position.z, (float)collider.sizez + this->collider.sizez);
    bool x = int (abs(_position.x - position.x) * 100) <= (int) ((this->collider.sizex + collider.sizex) * 100);
    bool y = int (abs(_position.y - position.y) * 100) <= int ((this->collider.sizey + collider.sizey) * 100);
    bool z = int (abs(_position.z - position.z) * 100) <= int ((this->collider.sizez + collider.sizez) * 100);
    //bool sz = (position.z - _position.z > 0);
    //bool signed_z = int ((position.z - _position.z) * 100) <= int ((this->collider.sizez + collider.sizez) * 100);

    return x && y && z;//(z || (signed_z&&sz));
}

// Draws the player on the screen
void Player::render(const GLuint &MatrixID, const GLuint &ModelMatrixID, const GLuint &ViewMatrixID, const mat4 &Projection, const mat4 &View)
{
    //make the matrices for the transformation
    TranslationMatrix = translate(mat4(), vec3(_position.x, _position.y,_position.z));
    glm::mat4 Model = TranslationMatrix* RotationMatrix* ScalingMatrix;

    //make the MVP matix
    glm::mat4 MVP        = Projection * View * Model;

    // Send our transformation to the currently bound shader,
    // in the "MVP" uniform
    // For each object you render, since the MVP will be different (at least the Model part)
    glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
    glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &Model[0][0]);
    glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &View[0][0]);

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

    // 3rd attribute buffer : normals
    glEnableVertexAttribArray(vertexNormal_modelspaceID);
    glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
    glVertexAttribPointer(
        vertexNormal_modelspaceID,    // The attribute we want to configure
        3,                            // size
        GL_FLOAT,                     // type
        GL_FALSE,                     // normalized?
        0,                            // stride
        (void*)0                      // array buffer offset
    );

    // Draw the triangleS !
    glDrawArrays(GL_TRIANGLES, 0, _vertices.size());

    glDisableVertexAttribArray(vertexPosition_modelspaceID);
    glDisableVertexAttribArray(vertexUVID);
    glDisableVertexAttribArray(vertexNormal_modelspaceID);
}

// Update the player states
bool Player::update(GLFWwindow* window, std::list<GameObject *> *gameObjects)
{
    //get user input
    if (glfwGetKey( window, GLFW_KEY_LEFT) == GLFW_PRESS ) //left arrow is pressed, so move the traingle left
    {
        if (_position.x < MAX_POSITIVE_X)
        {
            _position.x+=0.3f;
            //if (anglez <= 0)
              //  anglez += 0.1f;
        }
    }
    else if (glfwGetKey( window, GLFW_KEY_RIGHT) == GLFW_PRESS )
    {
        if ( _position.x > MAX_NEGATIVE_X )
        {
            _position.x-=0.3f;
            //if (anglez >= 0)
              //  anglez -= 0.1f;
        }
    }
    else if (glfwGetKey( window, GLFW_KEY_W) == GLFW_PRESS )
    {
        if ( _position.z <= MAX_POSITIVE_Z )
        {
            _position.z+=0.3f;
            //if (anglex < 0.5f)
              //  anglex += 0.005f;
        }
    }
    else if (glfwGetKey( window, GLFW_KEY_S) == GLFW_PRESS )
    {
        if (_position.z >= MAX_NEGATIVE_Z)
        {
            _position.z-=0.3f;
            //if ( anglex > 0)
              //  anglex -= 0.005f;
        }
    }
    else if (glfwGetKey( window, GLFW_KEY_UP) == GLFW_PRESS )
    {
        if ( _position.y <= MAX_POSITIVE_Y )
        {
            _position.y+=0.3f;
        }
    }
    else if (glfwGetKey( window, GLFW_KEY_DOWN) == GLFW_PRESS )
    {
        if (_position.y >= MAX_NEGATIVE_Y)
        {
            _position.y-=0.3f;
        }
    }

    for (std::list<GameObject *>::iterator it = gameObjects->begin(); it != gameObjects->end(); it++)
    {
        if (((*it)->getObjectType() == MONSTER || (*it)->getObjectType() == OBSTECL) && checkCollision((*it)))
        {
            vec3 pos = (*it)->getPosition();
            printf("**** x:%f y:%f z:%f ***\n", (float)_position.x, (float)_position.y, (float)_position.z);
            printf("-------------**** x:%f y:%f z:%f ***-------------\n", (float)pos.x, (float)pos.y, (float)pos.z);
            return false;
        }
    }
    RotationMatrix = eulerAngleYXZ(angley, anglex, anglez);//yaw, pitch and roll. Measured in radians
    return true;
}
