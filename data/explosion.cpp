#include "explosion.h"

Explode::Explode() : GameObject(vec3(0, 0, 0), EXPLODE)
{
    anglex = 2.0;
    anglez = 0.0;
    angley = 0.0;
    scalex = 19.0f;
    scaley = 13.0f;
    scalez = 11.0f;
    ScalingMatrix = scale(mat4(), vec3(scalex,scaley,scalez));
    TranslationMatrix = translate(mat4(), vec3(0.0, -8.0,25.0f));
    RotationMatrix = eulerAngleYXZ(angley, anglex, anglez);
}

Explode::~Explode()
{

}

void Explode::render(const GLuint &MatrixID, const GLuint &ModelMatrixID, const GLuint &ViewMatrixID, const mat4 &Projection, const mat4 &View)
{
    MVP = Projection * View*TranslationMatrix*ScalingMatrix*RotationMatrix ;
    glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
    glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &(mat4(1.0))[0][0]);
    glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &View[0][0]);

    // Bind our texture in Texture Unit 0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    // Set our "myTextureSampler" sampler to user Texture Unit 0
    glUniform1i(textureID, 0);//Specify the value of a uniform variable for the current program object

    // 1rst attribute buffer : vertices
    glEnableVertexAttribArray(vertexPosition_modelspaceID);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glVertexAttribPointer(
        vertexPosition_modelspaceID,  // The attribute we want to configure
        3,                            // size
        GL_FLOAT,                     // type
        GL_FALSE,                     // normalized?
        0,                            // stride
        (void*)0                      // array buffer offset
    );

    // 2nd attribute buffer : UVs
    glEnableVertexAttribArray(vertexUVID);
    glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
    glVertexAttribPointer(
        vertexUVID,                   // The attribute we want to configure
        2,                            // size : U+V => 2
        GL_FLOAT,                     // type
        GL_FALSE,                     // normalized?
        0,                            // stride
        (void*)0                      // array buffer offset
    );

    // Draw the triangles !
    glDrawArrays(GL_TRIANGLES, 0, 12*3); // 12*3 indices starting at 0 -> 12 triangles

    glDisableVertexAttribArray(vertexPosition_modelspaceID);
    glDisableVertexAttribArray(vertexUVID);

}
