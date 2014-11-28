#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include "DEFS.h"
#include "common/objloader.hpp"
using namespace glm;

class GameObject
{
public:
    GameObject(Lane lane, vec3 position, GameObjectType type);
    GameObject();
    virtual ~GameObject();

    //setup and initialize object
    virtual void setup(GLuint programID) = 0;

    //especify how the object is drawn to the screen
    virtual void render(const GLuint &MatrixID, const mat4 &Projection, const mat4 &View) = 0;
	
    //update the object state
	virtual void update() = 0;

    //Checks collision with another GameObject
    virtual bool checkCollision(GameObject *) = 0;

    //virtual void makeSoundEffect() = 0;

    //gets the type of the object
    GameObjectType getObjectType() const;

    //get the position of the object
    vec3 getPosition() const;

    //set the position of the object
    void setPosition(vec3);

    //get the lane the object currently occupies
    Lane getLane() const;

    //get the vertices of the object
    std::vector< glm::vec3 > getVertices() const;

    //get the UVs of the object
    std::vector< glm::vec2 > getUVs() const;

    //get the normals of the object
    std::vector< glm::vec3 > getNormals() const;

protected:
    //define the lane which the object in
    Lane _lane;

    //type of game object
    GameObjectType _objectType;

    //texture id
	GLuint _textureID;

    //position of the object
    vec3 _position;

    ///model parameters

    //vertices
    std::vector< glm::vec3 > _vertices;

    std::vector< glm::vec2 > _uvs;

    std::vector< glm::vec3 > _normals;
};

#endif // GAMEOBJECT_H

