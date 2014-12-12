#include "monster.h"

int Monster::monsterCount = 0;

Monster::Monster(Lane lane, vec3 position): GameObject(lane, position, PLAYER)
{
    monsterCount++;
    angley = 3.14;
    anglex = 0;
    anglez = 0;
    printf("Monster::Monster() created monster #%d at x=%d y=%d z=%d\n", monsterCount, (int)position.x, (int)position.y, (int)position.z);
    RotationMatrix = eulerAngleYXZ(angley, anglex, anglez);//yaw, pitch and roll. Measured in radians
    //ScalingMatrix = scale(mat4(), vec3(0.005f, 0.005f, 0.005f));
    ScalingMatrix = scale(mat4(), vec3(0.05f, 0.05f, 0.05f));
    inRange = true;
}

// Destructor
Monster::~Monster()
{
    printf("Monster::~Monster() destructed monster #%d\n", monsterCount);
}

bool Monster::checkCollision(GameObject *)
{
    // TODO: implement collision detection
    return false;
}
// Release handlers IDs
void Monster::releaseResources()
{
    //delete the handles
    glDeleteBuffers(1, &vertexbuffer);
    glDeleteBuffers(1, &uvbuffer);
}

// Release the textures
void Monster::releaseTexture()
{
    //delete the handles
    glDeleteTextures(1, &textureID);
}

// Initialize object state
// @Deprecated: use constructor instead
void Monster::setup()
{
}
// Draws the Monster on the screen
void Monster::render(const GLuint &MatrixID, const mat4 &Projection, const mat4 &View)
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

// Update the player states
void Monster::update(GLFWwindow* window)
{
    //get user input
    if (_position.z >= MAX_NEGATIVE_Z)
    {
        _position.z-= 0.5;
    }
    else
        inRange = false;
}

// Checks if monster is in range
bool Monster::isInRange() const
{
    return this->inRange;
}
