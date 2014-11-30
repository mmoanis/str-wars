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
    glDeleteTextures(1, &textureID);
}

void Player::setup(GLuint programID)
{
    vertexPosition_modelspaceID = glGetAttribLocation(programID, "vertexPosition_modelspace");
    //GLuint vertexColorID = glGetAttribLocation(programID, "vertexColor");
    vertexUVID = glGetAttribLocation(programID, "vertexUV");

    texture = loadBMP_custom("models/Wraith Raider Starship/wr2.bmp");
    // Get a handle for our "myTextureSampler" uniform
    textureID  = glGetUniformLocation(programID, "myTextureSampler");

    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, _vertices.size() * sizeof(glm::vec3), &_vertices[0], GL_STATIC_DRAW);

    glGenBuffers(1, &uvbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
    glBufferData(GL_ARRAY_BUFFER, _uvs.size() * sizeof(glm::vec2), &_uvs[0], GL_STATIC_DRAW);

    RotationMatrix = eulerAngleYXZ(0.0f, 0.0f, 0.0f);//yaw, pitch and roll. Measured in radians
    ScalingMatrix = scale(mat4(), vec3(0.005f, 0.005f, 0.005f));
}

void Player::render(const GLuint &MatrixID, const mat4 &Projection, const mat4 &View)
{
    //make the matrices for the transformation
    TranslationMatrix = translate(mat4(), vec3((int)_position.x, 0.0f,(int)_position.z));

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

void Player::update()
{
    //get user input
    if (glfwGetKey( GLFW_KEY_LEFT) == GLFW_PRESS ) //left arrow is pressed, so move the traingle left
    {
        if (_position.x < MAX_POSITIVE_X)
        {
            _position.x++;
        }
    }
    else if (glfwGetKey( GLFW_KEY_RIGHT) == GLFW_PRESS )
    {
        if ( _position.x > MAX_NEGATIVE_X )
            _position.x--;
    }
    else if (glfwGetKey( GLFW_KEY_UP) == GLFW_PRESS )
    {
        if ( _position.z <= MAX_POSITIVE_Z )
            _position.z++;
    }
    else if (glfwGetKey( GLFW_KEY_DOWN) == GLFW_PRESS )
    {
        if (_position.z >= MAX_NEGATIVE_Z)
            _position.z--;
    }
}
