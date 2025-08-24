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
    void Draw();

    void LoadImage(NativeImage *image);

private:
    // base
    GLuint m_ProgramObj;
    GLuint m_VertexShader;
    GLuint m_FragmentShader;

    // yuv
    GLuint m_yTextureId;
    GLuint m_uvTextureId;

    GLint m_ySamplerLoc;
    GLint m_uvSamplerLoc;

    NativeImage m_RenderImage;
};


#endif //OPENGLTEST_TRIANGLESAMPLE_H
