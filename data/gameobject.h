#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <list>
#include <vector>
#include "DEFS.h"
#include <math.h>

// Include GLFW
#include <glfw3.h>

using namespace glm;

class GameObject
{
public:
    GameObject(vec3 position, GameObjectType type);
    virtual ~GameObject();

    //especify how the object is drawn to the screen
    virtual void render(const GLuint &MatrixID, const mat4 &Projection, const mat4 &View) = 0;
	
    //update the object state
    virtual bool update(GLFWwindow* window, std::list<GameObject *> *);

    //Checks collision with another GameObject
    virtual bool checkCollision(GameObject *);

    //virtual void makeSoundEffect() = 0;

    //gets the type of the object
    GameObjectType getObjectType() const;

    //get the position of the object
    vec3 getPosition() const;

    //set the position of the object
    void setPosition(vec3);

    //get the vertices of the object
    std::vector< glm::vec3 > getVertices() const;
    void setVertices(const std::vector< glm::vec3 > &v);

    //get the UVs of the object
    std::vector< glm::vec2 > getUVs() const;
    void setUVs(const std::vector< glm::vec2 > &v);

    //get the normals of the object
    std::vector< glm::vec3 > getNormals() const;
    void setNormals(const std::vector< glm::vec3 > &v);

    //set the handlers for the texture of the object
    void setTexture(const GLuint &tid, const GLuint &t);

    //set the handler of vertices, colors and UVs of the object
    void setIDs(const GLuint &vertexPosition_modelspaceID, const GLuint &vertexUVID, const GLuint &uvbuffer,
                const GLuint &vertexbuffer, const GLuint &vertexColorID, const GLuint &colorbuffer);

    //get the collider of the game object
    Collider getCollider() const;

    //check wether the bullet is still in screen
    bool isInRange() const;

protected:

    //bullet is still alive
    bool inRange;

    //type of game object
    GameObjectType _objectType;

    //position of the object
    vec3 _position;

    ///model parameters

    //vertices
    std::vector< glm::vec3 > _vertices;

    // uvs
    std::vector< glm::vec2 > _uvs;

    // normals
    std::vector< glm::vec3 > _normals;

    //vertex position handle
    GLuint vertexPosition_modelspaceID;

    //UV handle
    GLuint vertexUVID;

    //uv buffer
    GLuint uvbuffer;

    //texture handle
    GLuint texture;

    //texture
    GLuint textureID;

    GLuint vertexbuffer;

    GLuint vertexColorID;

    GLuint colorbuffer;

    ///transformation matrices
    glm::mat4 RotationMatrix;

    glm::mat4 TranslationMatrix;

    glm::mat4 ScalingMatrix;

    //angle of rotation around x-axis
    float anglex;

    //angle of rotation around z-axis
    float anglez;

    //angle of rotation around y-axis
    float angley;

    float scalex;

    float scaley;

    float scalez;

    //collider object
    Collider collider;
};

#endif // GAMEOBJECT_H

