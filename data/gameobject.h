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

	/**
	* especify how the object is drawn to the screen. 
	**/
    virtual void render(GLuint & programID,GLuint &MatrixID,
                        mat4 Projection, mat4  View) = 0;
	
	/**
	* update the object state.
	**/
	virtual void update() = 0;

	/**
	* Checks collision with another GameObject.
	**/
    virtual bool checkCollision(GameObject *) = 0;

    //virtual void makeSoundEffect() = 0;

    /**
     * @brief getObjectLane
     * @return
     */
    Lane getObjectLane() const;

    GameObjectType getObjectType() const;

    vec3 getPosition() const;
    void setPosition(vec3);

    Lane getLane() const;
    std::vector< glm::vec3 > getVertices() const;
    std::vector< glm::vec2 > getUVs() const;
    std::vector< glm::vec3 > getNormals() const;

    ///TODO: make it private
//private:
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

