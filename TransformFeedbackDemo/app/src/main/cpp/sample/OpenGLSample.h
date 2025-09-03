//
// Created by lixiang on 2025/8/23.
//

#ifndef OPENGL_TRIANGLE_DEMO_TRIANGLESAMPLE_H
#define OPENGL_TRIANGLE_DEMO_TRIANGLESAMPLE_H

#include <GLES3/gl3.h>
#include "../util/LogUtil.h"
#include "../util/GLUtils.h"
#include "../util/ImageDef.h"

class OpenGLSample {
public:
    OpenGLSample();
    ~OpenGLSample();

    void Init();
    void Draw(int screenWidth, int screenHeight);

    void LoadImage(NativeImage *image);

    bool CreateFrameBufferObj();

private:
    // base
    GLuint m_ProgramObj;
    GLuint m_VertexShader;
    GLuint m_FragmentShader;

    GLuint m_ImageTextureId;
    GLuint m_VaoId;
    GLuint m_VboIds[2] = {GL_NONE};
    GLint m_SamplerLoc;
    GLuint m_TransFeedbackObjId;
    GLuint m_TransFeedbackBufId;
    NativeImage m_RenderImage;
};


#endif //OPENGLTEST_TRIANGLESAMPLE_H
