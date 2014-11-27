#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include "DEFS.h"
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
    virtual void render(RenderData *) = 0;
	
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

private:
    //define the lane which the object in
    Lane _lane;

    //type of game object
    GameObjectType _objectType;

    //texture id
	GLuint _textureID;

    //position of the object
    vec3 _position;
};

#endif // GAMEOBJECT_H

