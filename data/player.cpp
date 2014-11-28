#include "player.h"

bool Player::checkCollision(GameObject *)
{
    return false;
}

Player::Player(Lane lane, vec3 position): GameObject(lane, position, PLAYER)
{
    printf("Player::Player() at x=%d y=%d z=%d\n", (int)position.x, (int)position.y, (int)position.z);
    // Read our .obj file
    bool res = loadOBJ("models/Wraith Raider Starship/WraithRaiderStarship.obj", _vertices, _uvs, _normals);
}

Player::~Player()
{
    glDeleteBuffers(1, &vertexbuffer);
    glDeleteBuffers(1, &uvbuffer);
    for(std::vector<Bullet *>::iterator it = bullets.begin(); it != bullets.end(); it++)
    {
        delete (*it);
    }
}

void Player::render(GLuint & programID,GLuint &MatrixID,
                    mat4 Projection, mat4  View)
{
    vertexPosition_modelspaceID = glGetAttribLocation(programID, "vertexPosition_modelspace");
    //GLuint vertexColorID = glGetAttribLocation(programID, "vertexColor");
    vertexUVID = glGetAttribLocation(programID, "vertexUV");



    vec3 position = getPosition();
    //printf("Player::render() at x=%d y=%d z=%d\n", (int)position.x, (int)position.y, (int)position.z);
    glm::mat4 RotationMatrix = eulerAngleYXZ(0.0f, 0.0f, 0.0f);//yaw, pitch and roll. Measured in radians
    glm::mat4 TranslationMatrix = translate(mat4(), vec3((int)position.x,0.0f,(int)position.z));
    glm::mat4 ScalingMatrix = scale(mat4(), vec3(0.025f, 0.025f, 0.025f));
    glm::mat4 Model = TranslationMatrix* RotationMatrix* ScalingMatrix;//order of multiplication is important (try different values above and different order of multiplication)

    glm::mat4 MVP        = Projection * View * Model;

    // Send our transformation to the currently bound shader,
    // in the "MVP" uniform
    // For each object you render, since the MVP will be different (at least the Model part)
    glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, _vertices.size() * sizeof(glm::vec3), &_vertices[0], GL_STATIC_DRAW);


    glGenBuffers(1, &uvbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
    glBufferData(GL_ARRAY_BUFFER, _uvs.size() * sizeof(glm::vec2), &_uvs[0], GL_STATIC_DRAW);

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
    if (glfwGetKey( GLFW_KEY_LEFT) ==GLFW_PRESS && position.x <= 7 ) //left arrow is pressed, so move the traingle left
    {
        position.x++;
    }
    else if (glfwGetKey( GLFW_KEY_RIGHT) ==GLFW_PRESS &&  position.x >= -7 )
    {
        position.x--;
    }
    else if (glfwGetKey( GLFW_KEY_UP) ==GLFW_PRESS && position.z <= 7 )
    {
        position.z++;
    }
    else if (glfwGetKey( GLFW_KEY_DOWN) ==GLFW_PRESS && position.z >= -7 )
    {
         position.z--;
    }

    if (glfwGetKey( GLFW_KEY_DOWN) == GLFW_KEY_SPACE )
    {
         //fire bullets
        bullets.push_back(new Bullet(getLane(), _position));
    }

    //apply changes
    setPosition(position);

    //update bullets
    for(std::vector<Bullet *>::iterator it = bullets.begin(); it != bullets.end(); it++)
    {
        (*it)->update();
    }
}
