#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <vector>
#include "DEFS.h"

// Include GLFW
#include <glfw3.h>

using namespace glm;

class GameObject
{
public:
    GameObject(Lane lane, vec3 position, GameObjectType type);
    GameObject();
    virtual ~GameObject();

    //setup and initialize object
    virtual void setup() = 0;

    //especify how the object is drawn to the screen
    virtual void render(const GLuint &MatrixID, const mat4 &Projection, const mat4 &View) = 0;
	
    //update the object state
    virtual void update(GLFWwindow* window) = 0;

    //Checks collision with another GameObject
    virtual bool checkCollision(GameObject *) = 0;

    //release handlers
    virtual void releaseResources() = 0;

    //release texture
    virtual void releaseTexture();

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
    void setVertices(std::vector< glm::vec3 > v);

    //get the UVs of the object
    std::vector< glm::vec2 > getUVs() const;
    void setUVs(std::vector< glm::vec2 > v);

    //get the normals of the object
    std::vector< glm::vec3 > getNormals() const;
    void setNormals(std::vector< glm::vec3 > v);

    void setTexture(GLuint tid, GLuint t);

    void setIDs(GLuint vertexPosition_modelspaceID, GLuint vertexUVID, GLuint uvbuffer,
                GLuint vertexbuffer, GLuint vertexColorID, GLuint colorbuffer);

protected:
    //define the lane which the object in
    Lane _lane;

    //type of game object
    GameObjectType _objectType;

    //position of the object
    vec3 _position;

    ///model parameters

    //vertices
    std::vector< glm::vec3 > _vertices;

    std::vector< glm::vec2 > _uvs;

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

};

#endif // GAMEOBJECT_H

