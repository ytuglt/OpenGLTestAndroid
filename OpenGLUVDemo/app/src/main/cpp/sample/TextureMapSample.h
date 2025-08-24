//
// Created by lixiang on 2025/8/23.
//

#ifndef OPENGL_TRIANGLE_DEMO_TRIANGLESAMPLE_H
#define OPENGL_TRIANGLE_DEMO_TRIANGLESAMPLE_H

#include <GLES3/gl3.h>
#include "../util/LogUtil.h"
#include "../util/GLUtils.h"
#include "../util/ImageDef.h"

class TextureMapSample {
public:
    TextureMapSample();
    ~TextureMapSample();

    void Init();
    void Draw();

    void LoadImage(NativeImage *image);

private:
    GLuint m_ProgramObj;
    GLuint m_VertexShader;
    GLuint m_FragmentShader;

    NativeImage m_RenderImage;
    GLuint m_TextureId;
    GLint m_samplerLoc;
};


#endif //OPENGLTEST_TRIANGLESAMPLE_H
