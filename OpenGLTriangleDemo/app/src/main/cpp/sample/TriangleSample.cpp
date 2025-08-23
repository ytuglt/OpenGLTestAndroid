//
// Created by lixiang on 2025/8/23.
//

#include "TriangleSample.h"
#include "../util/LogUtil.h"

TriangleSample::TriangleSample() {
    m_ProgramObj = 0;
    m_VertexShader = 0;
    m_FragmentShader = 0;
}

TriangleSample::~TriangleSample() {
    if (m_ProgramObj) {
        glDeleteProgram(m_ProgramObj);
    }
}