#include "monster.h"

int Monster::monsterCount = 0;

Monster::Monster(vec3 position): GameObject(position, MONSTER)
{
    monsterCount++;
    angley = 3.14;
    anglex = 0;
    anglez = 0;
    scalex = scaley = scalez = 0.02;
    printf("Monster::Monster() created monster #%d at x=%d y=%d z=%d\n", monsterCount, (int)position.x, (int)position.y, (int)position.z);
    RotationMatrix = eulerAngleYXZ(angley, anglex, anglez);//yaw, pitch and roll. Measured in radians
    ScalingMatrix = scale(mat4(), vec3(scalex, scaley, scalez));
    inRange = true;

    // set the collider
    this->collider.sizex = this->collider.sizey = this->collider.sizez = 0.35;
}

// Destructor
Monster::~Monster()
{
    printf("Monster::~Monster() destructed monster #%d\n", monsterCount);
}

bool Monster::checkCollision(GameObject *other)
{
    //Collider collider = other ->getCollider();
    vec3 position = other ->getPosition();

    if ( (abs(_position.x - position.x)) <= this->collider.sizex)
    {
        if ( (abs(_position.y - position.y)) <= this->collider.sizey)
        {
            if ( (abs(_position.z - position.z)) <= this->collider.sizez)
            {
                return true;
            }
        }
    }
    return false;
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
bool Monster::update(GLFWwindow*, std::vector<GameObject *> * gameObjects)
{
    //get user input
    if (_position.z >= MAX_NEGATIVE_Z)
    {
        _position.z-= 0.09;
    }
    else
        inRange = false;

    //printf("%d\n", gameObjects->size());
    for (std::vector<GameObject *>::iterator it = gameObjects->begin(); it != gameObjects->end(); it++)
    {
        if ((*it)->getObjectType() == BULLET && checkCollision((*it)))
        {
            vec3 pos = (*it)->getPosition();
            printf("**** x:%d y:%d z:%d ***\n", (int)_position.x, (int)_position.y, (int)_position.z);
            printf("-------------**** %d x:%d y:%d z:%d ***-------------\n", (*it)->getObjectType(), (int)pos.x, (int)pos.y, (int)pos.z);
            return false;
        }
    }

    return true;
}

// Checks if monster is in range
bool Monster::isInRange() const
{
    return this->inRange;
}
