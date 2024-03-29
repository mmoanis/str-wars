#include "bullet.h"

// Number of bullets
int Bullet::bulletCount = 0;

// Constructor
Bullet::Bullet(vec3 position): GameObject(position, BULLET)
{
    bulletCount++;
    printf("Bullet::Bullet() created bullet #%d at x=%d y=%d z=%d\n", bulletCount, (int)position.x, (int)position.y, (int)position.z);
    inRange = true;
    anglez = 0;
    anglex = 0;
    angley = 0;
    scalex = 0.02f;
    scaley = 0.02f;
    scalez = 0.02f;
    RotationMatrix = eulerAngleYXZ(angley, anglex,anglez);
    ScalingMatrix = scale(mat4(), vec3(scalex, scaley, scalez));

    // set the collider
    this->collider.sizex = this->collider.sizey = this->collider.sizez = 0.02f;
}

// Destructor
Bullet::~Bullet()
{
    printf("Bullet::~Bullet() destructed bullet #%d at x=%d y=%d z=%d\n", bulletCount, (int)_position.x, (int)_position.y, (int)_position.z);
}

void Bullet::render(const GLuint &MatrixID, const GLuint &ModelMatrixID, const GLuint &ViewMatrixID, const mat4 &Projection, const mat4 &View)
{
    ///debug
    //printf("Bullet::render()\n");

    //make transformation matrices
    TranslationMatrix = translate(mat4(), _position);
    glm::mat4 Model = TranslationMatrix * RotationMatrix * ScalingMatrix ;//* translate(mat4(), vec3(_position.x * -1, _position.y * -1, _position.z * -1));

    //make the MVP matrix
    glm::mat4 MVP        = Projection * View * Model;

    // Send our transformation to the currently bound shader,
    // in the "MVP" uniform
    // For each model you render, since the MVP will be different (at least the Model part)
    glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
    glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &(mat4(1.0))[0][0]);
    glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &View[0][0]);

    // 1rst attribute buffer : vertices
    glEnableVertexAttribArray(vertexPosition_modelspaceID);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glVertexAttribPointer(
            vertexPosition_modelspaceID, // The attribute we want to configure
            3,                           // size
            GL_FLOAT,                    // type
            GL_FALSE,                    // normalized?
            0,                           // stride
            (void*)0                     // array buffer offset
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
    glDrawArrays(GL_TRIANGLES, 0, 12*3); // 12*3 indices starting at 0 -> 12 triangles

    // Disable vertix IDs
    glDisableVertexAttribArray(vertexPosition_modelspaceID);
    glDisableVertexAttribArray(vertexColorID);
}

// Update the position of the bullet to move into positive Z
bool Bullet::update(GLFWwindow*, std::list<GameObject *> *)
{
    //update bullet position
    if (_position.z < BULLET_BARRIER)
    {
        _position.z+=0.2f;
    }
    else
        inRange = false;    //mark dead

    anglex++;
    RotationMatrix = eulerAngleYXZ(angley, anglex,anglez);

    return true;
}
