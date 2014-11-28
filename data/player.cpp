#include "player.h"

bool Player::checkCollision(GameObject *)
{
    return false;
}

Player::Player(Lane lane, vec3 position): GameObject(lane, position, PLAYER)
{
    printf("Player::Player() at x=%d y=%d z=%d\n", (int)position.x, (int)position.y, (int)position.z);
    // Read our .obj file
    loadOBJ("models/Wraith Raider Starship/WraithRaiderStarship.obj", _vertices, _uvs, _normals);
}

Player::~Player()
{
    //delete the handles
    glDeleteBuffers(1, &vertexbuffer);
    glDeleteBuffers(1, &uvbuffer);

    //delete remaining bullets
    for(std::vector<Bullet *>::iterator it = bullets.begin(); it != bullets.end(); it++)
    {
        delete (*it);
    }
}

void Player::setup(GLuint programID)
{
    vertexPosition_modelspaceID = glGetAttribLocation(programID, "vertexPosition_modelspace");
    //GLuint vertexColorID = glGetAttribLocation(programID, "vertexColor");
    vertexUVID = glGetAttribLocation(programID, "vertexUV");

    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, _vertices.size() * sizeof(glm::vec3), &_vertices[0], GL_STATIC_DRAW);

    glGenBuffers(1, &uvbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
    glBufferData(GL_ARRAY_BUFFER, _uvs.size() * sizeof(glm::vec2), &_uvs[0], GL_STATIC_DRAW);
}

void Player::render(GLuint & programID,GLuint &MatrixID, mat4 Projection, mat4  View)
{
    //make the matrices for the transformation
    glm::mat4 RotationMatrix = eulerAngleYXZ(0.0f, 0.0f, 0.0f);//yaw, pitch and roll. Measured in radians
    glm::mat4 TranslationMatrix = translate(mat4(), vec3((int)_position.x, 0.0f,(int)_position.z));
    glm::mat4 ScalingMatrix = scale(mat4(), vec3(0.005f, 0.005f, 0.005f));
    glm::mat4 Model = TranslationMatrix* RotationMatrix* ScalingMatrix;//order of multiplication is important (try different values above and different order of multiplication)

    //make the MVP matix
    glm::mat4 MVP        = Projection * View * Model;

    // Send our transformation to the currently bound shader,
    // in the "MVP" uniform
    // For each object you render, since the MVP will be different (at least the Model part)
    glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

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

    //render player bullets
    //draw any bullets fired
    for(std::vector<Bullet *>::iterator it = bullets.begin(); it != bullets.end(); it++)
    {
        (*it)->render(programID, MatrixID, Projection, View);
    }
}

void Player::update()
{
    vec3 position = getPosition();

    //get user input
    if (glfwGetKey( GLFW_KEY_LEFT) ==GLFW_PRESS ) //left arrow is pressed, so move the traingle left
    {
        if (position.x < MAX_POSITIVE_X)
        {
            position.x++;
        }
    }
    else if (glfwGetKey( GLFW_KEY_RIGHT) ==GLFW_PRESS )
    {
        if ( position.x > MAX_NEGATIVE_X )
            position.x--;
    }
    else if (glfwGetKey( GLFW_KEY_UP) ==GLFW_PRESS )
    {
        if ( position.z <= MAX_POSITIVE_Z )
            position.z++;
    }
    else if (glfwGetKey( GLFW_KEY_DOWN) ==GLFW_PRESS )
    {
        if (position.z >= MAX_NEGATIVE_Z)
            position.z--;
    }

    if (glfwGetKey( GLFW_KEY_DOWN) == GLFW_KEY_SPACE )
    {
         //fire bullets
        //bullets.push_back(new Bullet(getLane(), _position));
    }

    //apply changes
    setPosition(position);

    //update bullets
    for(std::vector<Bullet *>::iterator it = bullets.begin(); it != bullets.end(); it++)
    {
        (*it)->update();
    }
}
