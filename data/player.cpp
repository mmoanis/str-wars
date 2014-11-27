#include "player.h"

Player::Player(Lane lane, vec3 position): GameObject(lane, position, PLAYER)
{
}

bool Player::checkCollision(GameObject *)
{
    return false;
}

void Player::render()
{

}

void Player::update()
{

}
