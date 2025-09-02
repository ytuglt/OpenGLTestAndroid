//
// Created by lixiang on 2025/8/23.
//

#include "OpenGLSample.h"
//#include "../util/LogUtil.h"

#define VERTEXT_POS_INDEX 0
#define TEXTURE_POS_INDX 1

OpenGLSample::OpenGLSample() {
    LOGI("OpenGLSample construct");
    m_ProgramObj = 0;
    m_VertexShader = 0;
    m_FragmentShader = 0;

    m_VaoIds[0] = GL_NONE;
    m_VboIds[0] = GL_NONE;

    m_ImageTextureId = GL_NONE;
    m_FboTextureId = GL_NONE;
    m_SamplerLoc = GL_NONE;
    m_FboId = GL_NONE;
    m_FboProgramObj = GL_NONE;
    m_FboVertexShader = GL_NONE;
    m_FboFragmentShader = GL_NONE;
    m_FboSamplerLoc = GL_NONE;
}

OpenGLSample::~OpenGLSample() {
    LOGI("OpenGLSample destruct");
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
    GLfloat vVertices[] = {
            -1.0f, -1.0f, 0.0f,
            1.0f, -1.0f, 0.0f,
            -1.0f, 1.0f, 0.0f,
            1.0f, 1.0f, 0.0f,
    };
    GLfloat vTexCoords[] = {
            0.0f, 1.0f,
            1.0f, 1.0f,
            0.0f, 0.0f,
            1.0f, 0.0f,
    };

    GLfloat vFboTexCoords[] = {
            0.0f, 0.0f,
            1.0f, 0.0f,
            0.0f, 1.0f,
            1.0f, 1.0f,
    };

    GLushort indices[] = {
            0, 1, 2,
            1, 3, 2,
    };

    char vShaderStr[] =
            "#version 300 es\n"
            "layout(location = 0) in vec3 aPosition;\n"
            "layout(location = 1) in vec2 aTexCoord;\n"
            "out vec2 vTexCoord;\n"
            "void main()\n"
            "{\n"
            "    gl_Position = vec4(aPosition, 1.0);\n"
            "    vTexCoord = aTexCoord;\n"
            "}\n";
    char fShaderStr[] =
            "#version 300 es\n"
            "precision mediump float;\n"
            "in vec2 vTexCoord;\n"
            "layout(location = 0) out vec4 outColor;\n"
            "uniform sampler2D sTextureMap;\n"
            "void main()\n"
            "{\n"
            "    outColor = texture(sTextureMap, vTexCoord);\n"
            "}\n";
    char fFboShaderStr[] =
            "#version 300 es\n"
            "precision mediump float;\n"
            "in vec2 vTexCoord;\n"
            "layout(location = 0) out vec4 outColor;\n"
            "uniform sampler2D sTextureMap;\n"
            "void main()\n"
            "{\n"
            "    vec4 tempColor = texture(sTextureMap, vTexCoord);\n"
            "    float luminance = tempColor.r * 0.299 + tempColor.g * 0.587 + tempColor.b * 0.114;\n"
            "    outColor = vec4(vec3(luminance), 1.0);\n"
            "}\n";

    m_ProgramObj = GLUtils::CreateProgram(vShaderStr, fShaderStr, m_VertexShader, m_FragmentShader);

    m_FboProgramObj = GLUtils::CreateProgram(vShaderStr, fFboShaderStr, m_FboVertexShader, m_FboFragmentShader);
    if (m_ProgramObj == GL_NONE || m_FboProgramObj == GL_NONE) {
        LOGI("OpenGLSample Init CreateProgram failed");
        return;
    }
    m_SamplerLoc = glGetUniformLocation(m_ProgramObj, "sTextureMap");
    m_FboSamplerLoc = glGetUniformLocation(m_FboProgramObj, "sTextureMap");

    // generates VBO ids and load the VBOs with data
    glGenBuffers(4, m_VboIds);
    glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vVertices), vVertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vTexCoords), vTexCoords, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[2]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vFboTexCoords), vFboTexCoords, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_VboIds[3]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    GO_CHECK_GL_ERROR();

    // generate 2 VAO
    glGenVertexArrays(2, m_VaoIds);

    // 1. normal render VAO
    glBindVertexArray(m_VaoIds[0]);
    glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[0]);
    glEnableVertexAttribArray(VERTEXT_POS_INDEX);
    glVertexAttribPointer(VERTEXT_POS_INDEX, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (const void *)0);
    glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

    glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[1]);
    glEnableVertexAttribArray(TEXTURE_POS_INDX);
    glVertexAttribPointer(TEXTURE_POS_INDX, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (const void *)0);
    glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_VboIds[3]);

    GO_CHECK_GL_ERROR();
    glBindVertexArray(GL_NONE);
    GO_CHECK_GL_ERROR();

    //2. fbo VAO
    glBindVertexArray(m_VaoIds[1]);
    GO_CHECK_GL_ERROR();

    glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[0]);
    glEnableVertexAttribArray(VERTEXT_POS_INDEX);
    glVertexAttribPointer(VERTEXT_POS_INDEX, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (const void *)0);
    glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);
    GO_CHECK_GL_ERROR();

    glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[2]);
    glEnableVertexAttribArray(TEXTURE_POS_INDX);
    glVertexAttribPointer(TEXTURE_POS_INDX, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (const void *)0);
    glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);
    GO_CHECK_GL_ERROR();

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_VboIds[3]);

    GO_CHECK_GL_ERROR();
    glBindVertexArray(GL_NONE);

    // texture create and init
    glGenTextures(1, &m_ImageTextureId);
    glBindTexture(GL_TEXTURE_2D, m_ImageTextureId);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_RenderImage.width, m_RenderImage.height,
                 0, GL_RGBA, GL_UNSIGNED_BYTE, m_RenderImage.ppPlane[0]);
    glBindTexture(GL_TEXTURE_2D, GL_NONE);
    GO_CHECK_GL_ERROR();

    if (!CreateFrameBufferObj()) {
        LOGE("OpenGLSample Init CreateFrameBufferObj failed");
        return;
    }
}

void OpenGLSample::Draw(int screenWidth, int screenHeight) {
    // FBO render
    LOGI("OpenGLSample Draw screenWidth = %d, screenHeight = %d , image width = %d, image height = %d",
         screenWidth, screenHeight, m_RenderImage.width, m_RenderImage.height);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glViewport(0, 0, m_RenderImage.width, m_RenderImage.height);
    GO_CHECK_GL_ERROR();
    
    // do fbo off screen rendering - use image dimensions for FBO viewport
    glBindFramebuffer(GL_FRAMEBUFFER, m_FboId);
    GO_CHECK_GL_ERROR();
    glUseProgram(m_FboProgramObj);
    GO_CHECK_GL_ERROR();
    glBindVertexArray(m_VaoIds[1]);
    GO_CHECK_GL_ERROR();
    glActiveTexture(GL_TEXTURE0);
    GO_CHECK_GL_ERROR();
    glBindTexture(GL_TEXTURE_2D, m_ImageTextureId);
    GO_CHECK_GL_ERROR();
    glUniform1i(m_FboSamplerLoc, 0);
    GO_CHECK_GL_ERROR();
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, (const void *)0);
    GO_CHECK_GL_ERROR();
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // do normal rendering - use screen dimensions for final render
    glViewport(0, 0, screenWidth, screenHeight);
    glUseProgram(m_ProgramObj);
    GO_CHECK_GL_ERROR();
    glBindVertexArray(m_VaoIds[0]);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_FboTextureId);
    glUniform1i(m_SamplerLoc, 0);
    GO_CHECK_GL_ERROR();
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, (const void *)0);
    GO_CHECK_GL_ERROR();
    glBindTexture(GL_TEXTURE_2D, GL_NONE);
    glBindVertexArray(GL_NONE);
}

bool OpenGLSample::CreateFrameBufferObj() {
    // create and init fbo texture
    glGenTextures(1, &m_FboTextureId);
    glBindTexture(GL_TEXTURE_2D, m_FboTextureId);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, GL_NONE);
    GO_CHECK_GL_ERROR();
    // create and init FBO
    glGenFramebuffers(1, &m_FboId);
    glBindFramebuffer(GL_FRAMEBUFFER, m_FboId);
    glBindTexture(GL_TEXTURE_2D, m_FboTextureId);
    GO_CHECK_GL_ERROR();
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_FboTextureId, 0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_RenderImage.width, m_RenderImage.height,
                 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    GO_CHECK_GL_ERROR();
    LOGE("OpenGLSample CreateFrameBufferObj glTexImage2D frameBuffer status = %d",
         glCheckFramebufferStatus(GL_FRAMEBUFFER));
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        LOGE("OpenGLSample CreateFrameBufferObj failed");
        return false;
    }
    glBindTexture(GL_TEXTURE_2D, GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE);
    return true;
}
