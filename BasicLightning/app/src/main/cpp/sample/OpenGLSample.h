//
// Created by lixiang on 2025/8/23.
//

#ifndef OPENGL_TRIANGLE_DEMO_TRIANGLESAMPLE_H
#define OPENGL_TRIANGLE_DEMO_TRIANGLESAMPLE_H

#include <GLES3/gl3.h>
#include "../util/LogUtil.h"
#include "../util/GLUtils.h"
#include "../util/ImageDef.h"
#include "../glm/detail/type_mat.hpp"
#include "../glm/detail/type_mat4x4.hpp"

class OpenGLSample {
public:
    OpenGLSample();
    ~OpenGLSample();

    void Init();
    void Draw(int screenWidth, int screenHeight);

    void LoadImage(NativeImage *image);

    virtual void Destroy();

    virtual void UpdateTransformMatrix(float rotateX, float rotateY, float scaleX, float scaleY);

    void UpdateMVPMatrix(glm::mat4 &mvpMatrix, int angleX, int angleY, float ratio);


private:
    // base
    GLuint m_ProgramObj;
    GLuint m_VertexShader;
    GLuint m_FragmentShader;

    int m_AngleX;
    int m_AngleY;
    float m_ScaleX;
    float m_ScaleY;

    GLuint m_TextureId;
    GLuint m_SamplerLoc;

    GLuint m_VaoId;
    GLuint m_VboIds[1];
    NativeImage m_RenderImage;

    GLint m_MVPMatLoc;
    glm::mat4 m_MVPMatrix;

    GLint m_ModelMatrixLoc;
    glm::mat4 m_ModelMatrix;

    GLint m_LightPosLoc;
    GLint m_LightColorLoc;
    GLint m_ViewPosLoc;

};


#endif //OPENGLTEST_TRIANGLESAMPLE_H
