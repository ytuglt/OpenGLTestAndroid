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
#include <vector>
#include <map>

#define RENDER_IMG_NUM 3

class OpenGLSample {
public:
    OpenGLSample();
    ~OpenGLSample();

    void Init();
    void Draw(int screenWidth, int screenHeight);

    void LoadImage(NativeImage *image);
    virtual void LoadMultiImageWithIndex(int index, NativeImage *pImage);

    virtual void Destroy();

    virtual void UpdateTransformMatrix(float rotateX, float rotateY, float scaleX, float scaleY);


    virtual void UpdateMatrix(glm::mat4 &mvpMatrix, glm::mat4 &modelMatrix, int angleXRotate, int angleYRotate, float scale, glm::vec3 transVec3, float ratio);

private:
    // base
    GLuint m_ProgramObj;
    GLuint m_VertexShader;
    GLuint m_FragmentShader;

    GLuint m_TextureId;
    GLint m_SamplerLoc;
    GLint m_MVPMatLoc;
    GLint m_ModelMatrixLoc;
    GLint m_ViewPosLoc;

    GLuint m_VaoId;
    GLuint m_VboIds[2];
    NativeImage m_RenderImage;
    glm::mat4 m_MVPMatrix;
    glm::mat4 m_ModelMatrix;

    int m_AngleX;
    int m_AngleY;

    float m_ScaleX;
    float m_ScaleY;
};


#endif //OPENGLTEST_TRIANGLESAMPLE_H
