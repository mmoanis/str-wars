#include "player.h"

bool Player::checkCollision(GameObject *)
{
    return false;
}

Player::Player(Lane lane, vec3 position): GameObject(lane, position, PLAYER)
{
    printf("Player::Player() at x=%d y=%d z=%d\n", (int)position.x, (int)position.y, (int)position.z);
}

void Player::render(RenderData * renderData)
{
    vec3 position = getPosition();
    //printf("Player::render() at x=%d y=%d z=%d\n", (int)position.x, (int)position.y, (int)position.z);
    glm::mat4 RotationMatrix = eulerAngleYXZ(0.0f, 0.0f, 0.5f);//yaw, pitch and roll. Measured in radians
    //glm::mat4 TranslationMatrix = translate(mat4(), vec3(position.x, 0.0f, position.z));
    glm::mat4 ScalingMatrix = scale(mat4(), vec3(1.0f, 1.0f, 1.0f));
    glm::mat4 Model = translate(mat4(), position)*RotationMatrix * ScalingMatrix;//order of multiplication is important (try different values above and different order of multiplication)

    glm::mat4 MVP        = renderData->Projection * renderData->View * Model;
    GLuint vertexColorID = glGetAttribLocation(renderData->programID, "vertexColor");
    // Send our transformation to the currently bound shader,
    // in the "MVP" uniform
    // For each object you render, since the MVP will be different (at least the Model part)
    glUniformMatrix4fv(renderData->MatrixID, 1, GL_FALSE, &MVP[0][0]);

    // 1rst attribute buffer : vertices
    glEnableVertexAttribArray(renderData->vertexPosition_modelspaceID);
    glBindBuffer(GL_ARRAY_BUFFER, renderData->vertexbuffer);
    glVertexAttribPointer(
    renderData->vertexPosition_modelspaceID, // The attribute we want to configure
    3,                           // size
    GL_FLOAT,                    // type
                GL_FALSE,                    // normalized? not
    0,                           // stride
    (void*)0                     // array buffer offset
    );

    // 2nd attribute buffer : colors
    glEnableVertexAttribArray(vertexColorID);
    glBindBuffer(GL_ARRAY_BUFFER, renderData->colorbuffer);
    glVertexAttribPointer(
    vertexColorID,               // The attribute we want to configure
    3,                           // size
    GL_FLOAT,                    // type
    GL_FALSE,                    // normalized?
    0,                           // stride
    (void*)0                     // array buffer offset
    );
    // Draw the triangleS !
    glDrawArrays(GL_TRIANGLES, 0, 12*3); // 12*3 indices starting at 0 -> 12 triangles
}

//////////////////////////////////////////////////////////////////
/// \brief Player::update
///updates the player state at each clock tick (60 frame)
void Player::update()
{
    vec3 position = getPosition();

    //get user input
    if (glfwGetKey( GLFW_KEY_LEFT) ==GLFW_PRESS ) //left arrow is pressed, so move the traingle left
    {
        position.x++;
    }
    else if (glfwGetKey( GLFW_KEY_RIGHT) ==GLFW_PRESS )
    {
        position.x--;
    }
    else if (glfwGetKey( GLFW_KEY_UP) ==GLFW_PRESS )
    {
        position.z--;
    }
    else if (glfwGetKey( GLFW_KEY_DOWN) ==GLFW_PRESS )
    {
         position.z++;
    }

    //printf("Player::update() x=%d y=%d z=%d", position.x, position.y, position.z);
    //apply changes
    setPosition(position);
}
