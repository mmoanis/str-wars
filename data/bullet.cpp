#include "bullet.h"

Bullet::Bullet(Lane lane, vec3 position): GameObject(lane, position, BULLET)
{
}

bool Bullet::checkCollision(GameObject *)
{
    //check
    return false;
}

void Bullet::render(RenderData * renderData)
{

}

void Bullet::update()
{
    //update bullet position
    vec3 temp = getPosition();
    temp.z++;
    this->setPosition(temp);
}
