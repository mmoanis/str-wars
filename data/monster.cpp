#include "monster.h"

int Monster::monsterCount = 0;

Monster::Monster(vec3 position): GameObject(position, MONSTER)
{
    monsterCount++;
    angley = 3.14f;
    anglex = 0;
    anglez = 0;
    scalex = scaley = scalez = 0.02f;
    printf("Monster::Monster() created monster #%d at x=%d y=%d z=%d\n", monsterCount, (int)position.x, (int)position.y, (int)position.z);
    RotationMatrix = eulerAngleYXZ(angley, anglex, anglez);//yaw, pitch and roll. Measured in radians
    ScalingMatrix = scale(mat4(), vec3(scalex, scaley, scalez));
    inRange = true;

    // set the collider
    this->collider.sizex = 0.45f;
    this->collider.sizey = 0.50f;
    this->collider.sizez = 0.40f;
}

// Destructor
Monster::~Monster()
{
    printf("Monster::~Monster() destructed monster #%d\n", monsterCount);
}

bool Monster::checkCollision(GameObject *other)
{
    Collider collider = other->getCollider();
    vec3 position = other ->getPosition();

    // uncomment in debuging
    //printf("\n%f %f %f\n", (float)_position.x, (float)position.x, (float)collider.sizex + (float)this->collider.sizex);
    //printf("%f %f %f\n", (float)_position.y, (float)position.y, (float)collider.sizey + this->collider.sizey);
    //printf("%f %f %f\n", (float)_position.z, (float)position.z, (float)collider.sizez + this->collider.sizez);
    bool x = int (abs(_position.x - position.x) * 100) <= (int) ((this->collider.sizex + collider.sizex) * 100);
    bool y = int (abs(_position.y - position.y) * 100) <= int ((this->collider.sizey + collider.sizey) * 100);
    bool z = int (abs(_position.z - position.z) * 100) <= int ((this->collider.sizez + collider.sizez) * 100);

    return x && y && z;
}

// Draws the Monster on the screen
void Monster::render(const GLuint &MatrixID, const GLuint &ModelMatrixID, const GLuint &ViewMatrixID, const mat4 &Projection, const mat4 &View)
{
    //make the matrices for the transformation
    TranslationMatrix = translate(mat4(), vec3(_position.x, _position.y, _position.z));

    glm::mat4 Model = TranslationMatrix* RotationMatrix* ScalingMatrix;//*translate(mat4(), vec3(_position.x * -1, _position.y * -1, _position.z * -1));//order of multiplication is important (try different values above and different order of multiplication)

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
    glDrawArrays(GL_TRIANGLES, 0, _vertices.size()); // 12*3 indices starting at 0 -> 12 triangles

    glDisableVertexAttribArray(vertexPosition_modelspaceID);
    glDisableVertexAttribArray(vertexUVID);
    glDisableVertexAttribArray(vertexNormal_modelspaceID);
}

// Update the player states
bool Monster::update(GLFWwindow*, std::list<GameObject *> * gameObjects)
{
    //get user input
    if (_position.z >= MAX_NEGATIVE_Z)
    {
        _position.z-= 0.09f;
        //printf("updated %f\n",(float) _position.z);
    }
    else
        inRange = false;

    //printf("%d\n", gameObjects->size());
    for (std::list<GameObject *>::iterator it = gameObjects->begin(); it != gameObjects->end(); it++)
    {
        if ((*it)->getObjectType() == BULLET && checkCollision((*it)))
        {
            (*it) -> setIsInRange(false);
            vec3 pos = (*it)->getPosition();
            printf("**** x:%d y:%d z:%d ***\n", (int)_position.x, (int)_position.y, (int)_position.z);
            printf("-------------**** %d x:%d y:%d z:%d ***-------------\n", (*it)->getObjectType(), (int)pos.x, (int)pos.y, (int)pos.z);
            return false;
        }
    }

    return true;
}
