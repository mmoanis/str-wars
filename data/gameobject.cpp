#include "gameobject.h"

/////////////////////////////////////////////////////////////////////
GameObject::GameObject(Lane lane, vec3 position, GameObjectType type)
{
    this->_lane = lane;
    this->_objectType = type;
    this->_position = position;
}
GameObject::GameObject()
{

}

GameObject::~GameObject()
{

}

/////////////////////////////////////////////////////////////////////
Lane GameObject::getObjectLane() const
{
    return _lane;
}
/////////////////////////////////////////////////////////////////////
GameObjectType GameObject::getObjectType() const
{
    return _objectType;
}
/////////////////////////////////////////////////////////////////////
vec3 GameObject::getPosition() const
{
    return _position;
}
/////////////////////////////////////////////////////////////////////
void GameObject::setPosition(vec3 position)
{
    this->_position = position;
}
