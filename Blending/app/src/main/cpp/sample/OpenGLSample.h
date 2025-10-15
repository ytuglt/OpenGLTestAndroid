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


    void UpdateMatrix(glm::mat4 &mvpMatrix, int angleXRotate, int angleYRotate, float scale, glm::vec3 transVec3, float ratio);


private:
    // base
    GLuint m_ProgramObj;
    GLuint m_VertexShader;
    GLuint m_FragmentShader;

    GLuint m_TextureIds[RENDER_IMG_NUM];
    GLint m_SamplerLoc;
    GLint m_MVPMatLoc;

    GLuint m_VaoIds[3];
    GLuint m_VboIds[3];
    NativeImage m_RenderImages[RENDER_IMG_NUM];
    glm::mat4 m_MVPMatrix;

    std::vector<glm::vec3> windowsTrans;
    std::map<GLfloat, glm::vec3> sorted;

    int m_AngleX;
    int m_AngleY;
};


#endif //OPENGLTEST_TRIANGLESAMPLE_H
