#include "bullet.h"

Bullet::Bullet(Lane lane, vec3 position): GameObject(lane, position, BULLET)
{
}

bool Bullet::checkCollision(GameObject *)
{
    //check
    return false;
}

void Bullet::render(GLuint & programID,GLuint &MatrixID, mat4 Projection, mat4  View)
{

}

void Bullet::update()
{
    //update bullet position
    vec3 temp = getPosition();
    temp.z++;
    this->setPosition(temp);
}
