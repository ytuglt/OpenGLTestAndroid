//
// Created by lixiang on 2025/8/23.
//

#include "TextureMapSample.h"
//#include "../util/LogUtil.h"

TextureMapSample::TextureMapSample() {
    LOGI("TextureMapSample construct");
    m_ProgramObj = 0;
    m_VertexShader = 0;
    m_FragmentShader = 0;

    m_TextureId = 0;
    m_samplerLoc = 0;

    m_RenderImage.width = 0;
    m_RenderImage.height = 0;
    m_RenderImage.ppPlane[0] = nullptr;
}

TextureMapSample::~TextureMapSample() {
    LOGI("TextureMapSample destruct");

    if (m_ProgramObj) {
        glDeleteProgram(m_ProgramObj);
    }

    if (m_TextureId) {
        glDeleteTextures(1, &m_TextureId);
    }

    if (m_RenderImage.ppPlane[0]) {
        free(m_RenderImage.ppPlane[0]);
        m_RenderImage.ppPlane[0] = nullptr;
    }
}

void TextureMapSample::LoadImage(NativeImage *image) {
    LOGI("TextureMapSample LoadImage image = %p ", image->ppPlane[0]);
    m_RenderImage.width = image->width;
    m_RenderImage.height = image->height;
    m_RenderImage.format = image->format;
    NativeImageUtil::CopyNativeImage(image, &m_RenderImage);
}

void TextureMapSample::Init() {
    //create RGBA texture
//    glGenTextures(1, &m_TextureId);
//    glBindTexture(GL_TEXTURE_2D, m_TextureId);
//    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//    glBindTexture(GL_TEXTURE_2D, GL_NONE);

    // 创建纹理
    glGenTextures(1, &m_TextureId);
    glBindTexture(GL_TEXTURE_2D, m_TextureId);

    // 设置纹理参数
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, GL_NONE);

    char vShaderStr[] =
            "#version 300 es                            \n"
            "layout(location = 0) in vec4 a_position;   \n"
            "layout(location = 1) in vec2 a_texCoord;   \n"
            "out vec2 v_texCoord;                       \n"
            "void main()                                \n"
            "{                                          \n"
            "   gl_Position = a_position;               \n"
            "   v_texCoord = a_texCoord;                \n"
            "}                                          \n";

    char fShaderStr[] =
            "#version 300 es                                     \n"
            "precision mediump float;                            \n"
            "in vec2 v_texCoord;                                 \n"
            "layout(location = 0) out vec4 outColor;             \n"
            "uniform sampler2D s_TextureMap;                     \n"
            "void main()                                         \n"
            "{                                                   \n"
            "  outColor = texture(s_TextureMap, v_texCoord);     \n"
            "}                                                   \n";
//            "#version 300 es\n"
//            "precision mediump float;\n"
//            "in vec2 v_texCoord; \n"
//            "layout(location = 0) out vec4 outColor; \n"
//            "uniform sampler2D s_TextureMap; \n"
//            "void main()\n"
//            "{\n"
//            "   outColor = texture(s_TextureMap, v_texCoord);\n"
////            "   outColor = vec4(1.0, 0.0, 0.0, 1.0); \n"
//            "}\n";


    m_ProgramObj = GLUtils::CreateProgram(vShaderStr, fShaderStr, m_VertexShader, m_FragmentShader);
    if (m_ProgramObj) {
        m_samplerLoc = glGetUniformLocation(m_ProgramObj, "s_TextureMap");
    } else {
        LOGE("TextureMapSample Init failed");
    }
}

void TextureMapSample::Draw() {
    LOGI("TextureMapSample Draw");

    if (m_ProgramObj == GL_NONE || m_TextureId == GL_NONE) {
        LOGE("TextureMapSample Draw failed");
        return;
    }

    glClearColor(1.0, 0.65, 0.0, 1.0);
    glClear(GL_STENCIL_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(m_ProgramObj);

    GLfloat verticesCoords[] = {
            -1.0f, 0.5f, 0.0f,
            -1.0f, -0.5f, 0.0f,
            1.0f, -0.5f, 0.0f,
            1.0f, 0.5f, 0.0f
    };
    GLfloat texCoords[] = {
            0.0f, 0.0f,
            0.0f, 1.0f,
            1.0f, 1.0f,
            1.0f, 0.0f
    };
    GLushort indices[] = { 0, 1, 2, 0, 2, 3 };

    // upload RGBA image data
    if (m_RenderImage.ppPlane[0]) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_TextureId);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
                     m_RenderImage.width, m_RenderImage.height, 0, GL_RGBA,
                     GL_UNSIGNED_BYTE, m_RenderImage.ppPlane[0]);
        glBindTexture(GL_TEXTURE_2D, GL_NONE);
        LOGI("glTexImage2D renderImage width = %d, height = %d , pplane = %p", m_RenderImage.width, m_RenderImage.height, m_RenderImage.ppPlane[0]);
    }

    glUseProgram(m_ProgramObj);

    //load the vertex position
    glVertexAttribPointer(0, 3, GL_FLOAT,
                          GL_FALSE, 3 * sizeof(GLfloat), verticesCoords);
    // load the texture coordinate
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE,
                          2 * sizeof(GLfloat), texCoords);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    // Bind the RGBA map
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_TextureId);

    // set the RGBA map sampler to texture unit to 0
    glUniform1i(m_samplerLoc, 0);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, indices);

//    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, verticesCoords);
//    glEnableVertexAttribArray(0);
//
//    glDrawArrays(GL_TRIANGLES, 0, 3);
}
