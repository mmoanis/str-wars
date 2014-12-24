#include "gameobject.h"

// Constructor
GameObject::GameObject(vec3 position, GameObjectType type)
{
    this->_objectType = type;
    this->_position = position;
}

// Default Constructor
// @Deprecated
GameObject::GameObject()
{

}

// Virtual Destructor
GameObject::~GameObject()
{

}

// Gets the type of object
GameObjectType GameObject::getObjectType() const
{
    return _objectType;
}

// Gets the position of object
vec3 GameObject::getPosition() const
{
    return _position;
}

// Sets the position of object
void GameObject::setPosition(vec3 position)
{
    this->_position = position;
}

// Checks collision with another game object
bool GameObject::checkCollision(GameObject * other)
{
    return false;
}

// Gets the UVs matrix
std::vector< glm::vec2 > GameObject::getUVs() const
{
    return this->_uvs;
}

// Sets the UVs
void GameObject::setUVs(const std::vector<vec2> &v)
{
    this->_uvs = v;
}

// Gets the vertices
std::vector< glm::vec3 > GameObject::getVertices() const
{
    return this->_vertices;
}

// Sets the vertices
void GameObject::setVertices(const std::vector<vec3> &v)
{
    this->_vertices = v;
}

// Gets the normals
std::vector< glm::vec3 > GameObject::getNormals() const
{
    return this->_normals;
}

// Sets the normal
void GameObject::setNormals(const std::vector<vec3> &v)
{
    this->_normals = v;
}

// Sets the texture
void GameObject::setTexture(const GLuint &tid, const GLuint &t)
{
    this->texture = t;
    this->textureID = tid;
}

// Sets handler IDS
void GameObject::setIDs(const GLuint &vertexPosition_modelspaceID, const GLuint &vertexUVID, const GLuint &uvbuffer,
                        const GLuint &vertexbuffer, const GLuint &vertexColorID, const GLuint &colorbuffer)
{
    this->vertexPosition_modelspaceID = vertexPosition_modelspaceID;
    this->vertexUVID = vertexUVID;
    this->uvbuffer = uvbuffer;
    this->vertexbuffer = vertexbuffer;
    this->vertexColorID = vertexColorID;
    this->colorbuffer = colorbuffer;
}

// Gets the collider of the object
Collider GameObject::getCollider() const
{
    return this->collider;
}
