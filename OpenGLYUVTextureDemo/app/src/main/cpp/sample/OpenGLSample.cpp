//
// Created by lixiang on 2025/8/23.
//

#include "OpenGLSample.h"
//#include "../util/LogUtil.h"

OpenGLSample::OpenGLSample() {
    LOGI("OpenGLSample construct");
    m_ProgramObj = 0;
    m_VertexShader = 0;
    m_FragmentShader = 0;

    m_yTextureId = GL_NONE;
    m_uvTextureId = GL_NONE;

    m_ySamplerLoc = GL_NONE;
    m_uvSamplerLoc = GL_NONE;
}

OpenGLSample::~OpenGLSample() {
    LOGI("OpenGLSample destruct");

    if (m_ProgramObj) {
        glDeleteProgram(m_ProgramObj);
    }

    NativeImageUtil::FreeNativeImage(&m_RenderImage);
}

void OpenGLSample::LoadImage(NativeImage *image) {
    LOGI("OpenGLSample LoadImage image = %p ", image->ppPlane[0]);
    m_RenderImage.width = image->width;
    m_RenderImage.height = image->height;
    m_RenderImage.format = image->format;
    NativeImageUtil::CopyNativeImage(image, &m_RenderImage);
}

void OpenGLSample::Init() {
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
            "uniform sampler2D y_texture;                     \n"
            "uniform sampler2D uv_texture;                     \n"
            "void main()                                         \n"
            "{                                                   \n"
            "  vec3 yuv;    \n"
            "   yuv.x = texture(y_texture, v_texCoord).r - 0.063; \n"
            "   yuv.z = texture(uv_texture, v_texCoord).a - 0.502; \n"
            "   yuv.y = texture(uv_texture, v_texCoord).r - 0.502; \n"
            "   highp vec3 rgb = mat3(1.164, 1.164, 1.164, \n"
            "                        0.0, -0.392, 2.017, \n"
            "                        1.596, -0.813, 0.0) * yuv; \n"
            "   outColor = vec4(rgb, 1.0); \n"
            "}                                                   \n";
    // load the shaders and get a linked program object
    m_ProgramObj = GLUtils::CreateProgram(vShaderStr, fShaderStr, m_VertexShader, m_FragmentShader);

    // get the sampler location
    m_ySamplerLoc = glGetUniformLocation(m_ProgramObj, "y_texture");
    m_uvSamplerLoc = glGetUniformLocation(m_ProgramObj, "uv_texture");

    // create textures
    GLuint textureIds[2] = {0};
    glGenTextures(2, textureIds);

    m_yTextureId = textureIds[0];
    m_uvTextureId = textureIds[1];
}

void OpenGLSample::Draw() {
    LOGI("OpenGLSample Draw");
    if(m_ProgramObj == GL_NONE || m_yTextureId == GL_NONE || m_uvTextureId == GL_NONE) return;

    glClearColor(1.0, 0.65, 0.0, 1.0);
    glClear(GL_STENCIL_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(m_ProgramObj);

    GLfloat verticesCoords[] = {
            -1.0f, 0.78f, 0.0f,
            -1.0f, -0.78f, 0.0f,
            1.0f, -0.78f, 0.0f,
            1.0f, 0.78f, 0.0f
    };
    GLfloat texCoords[] = {
            0.0f, 0.0f,
            0.0f, 1.0f,
            1.0f, 1.0f,
            1.0f, 0.0f
    };
    GLushort indices[] = { 0, 1, 2, 0, 2, 3 };

    // upload y plane data
    glBindTexture(GL_TEXTURE_2D, m_yTextureId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, m_RenderImage.width, m_RenderImage.height,0,
                 GL_LUMINANCE,  GL_UNSIGNED_BYTE, m_RenderImage.ppPlane[0]);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, GL_NONE);

    // upload uv plane data
    glBindTexture(GL_TEXTURE_2D, m_uvTextureId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE_ALPHA, m_RenderImage.width >> 1, m_RenderImage.height >> 1,0,
                 GL_LUMINANCE_ALPHA,  GL_UNSIGNED_BYTE, m_RenderImage.ppPlane[1]);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, GL_NONE);

    glUseProgram(m_ProgramObj);

    //load the vertex position
    glVertexAttribPointer(0, 3, GL_FLOAT,
                          GL_FALSE, 3 * sizeof(GLfloat), verticesCoords);
    // load the texture coordinate
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE,
                          2 * sizeof(GLfloat), texCoords);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    // Bind the y plane map
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_yTextureId);

    // Set the Y plane sampler to texture unit to 0
    glUniform1i(m_ySamplerLoc, 0);

    // Bind the uv plane map
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_uvTextureId);

    // Set the UV plane sampler to texture unit to 1
    glUniform1i(m_uvSamplerLoc, 1);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, indices);

//    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, verticesCoords);
//    glEnableVertexAttribArray(0);
//
//    glDrawArrays(GL_TRIANGLES, 0, 3);
}
