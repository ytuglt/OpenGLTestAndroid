//
// Created by lixiang on 2025/8/23.
//

#ifndef OPENGL_TRIANGLE_DEMO_TRIANGLESAMPLE_H
#define OPENGL_TRIANGLE_DEMO_TRIANGLESAMPLE_H

#include <GLES3/gl3.h>
//#include "../util/LogUtil.h"

class TriangleSample {
public:
    TriangleSample();
    ~TriangleSample();

//    void Init();
//    void Draw();

private:
    GLuint m_ProgramObj;
    GLuint m_VertexShader;
    GLuint m_FragmentShader;
};


#endif //OPENGLTEST_TRIANGLESAMPLE_H
