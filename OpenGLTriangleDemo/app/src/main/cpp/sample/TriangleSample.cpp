//
// Created by lixiang on 2025/8/23.
//

#include "TriangleSample.h"
//#include "../util/LogUtil.h"

TriangleSample::TriangleSample() {
    LOGI("TriangleSample construct");
    m_ProgramObj = 0;
    m_VertexShader = 0;
    m_FragmentShader = 0;
}

TriangleSample::~TriangleSample() {
    LOGI("TriangleSample destruct");
    if (m_ProgramObj) {
        glDeleteProgram(m_ProgramObj);
    }
}

void TriangleSample::Init() {
    char vShaderStr[] =
            "#version 300 es  \n"
            "layout(location = 0) in vec4 vPosition; \n"
            "void main()  \n"
            "{ \n"
            "   gl_Position = vPosition; \n"
            "} \n";
    char fShaderStr[] =
            "#version 300 es\n"
            "precision mediump float;\n"
            "out vec4 fragColor;\n"
            "void main()\n"
            "{\n"
            "   fragColor = vec4(1.0, 0.0, 0.0, 1.0);\n"
            "}\n";
    m_ProgramObj = GLUtils::CreateProgram(vShaderStr, fShaderStr, m_VertexShader, m_FragmentShader);
}

void TriangleSample::Draw() {
    LOGI("TriangleSample Draw");
    GLfloat vVertices[] = {
            0.0f, 0.5f, 0.0f,
            -0.5f, -0.5f, 0.0f,
            0.5f, -0.5f, 0.0f
    };

    if (m_ProgramObj == 0) {
        LOGE("TriangleSample Draw failed , program is 0");
        return;
    }

    glUseProgram(m_ProgramObj);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, vVertices);
    glEnableVertexAttribArray(0);

    glDrawArrays(GL_TRIANGLES, 0, 3);
}
