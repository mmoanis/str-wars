#include "gameobject.h"

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

Lane GameObject::getObjectLane() const
{
    return _lane;
}

GameObjectType GameObject::getObjectType() const
{
    return _objectType;
}

vec3 GameObject::getPosition() const
{
    return _position;
}

void GameObject::setPosition(vec3 position)
{
    this->_position = position;
}

Lane GameObject::getLane() const
{
    return this->_lane;
}

std::vector< glm::vec2 > GameObject::getUVs() const
{
    return this->_uvs;
}

std::vector< glm::vec3 > GameObject::getVertices() const
{
    return this->_vertices;
}

std::vector< glm::vec3 > GameObject::getNormals() const
{
    return this->_normals;
}
