//
// Created by lixiang on 2025/8/23.
//

#include "OpenGLSample.h"
//#include "../util/LogUtil.h"

#define VERTEX_POS_INDX  0
#define TEXTURE_POS_INDX 1

OpenGLSample::OpenGLSample() {
    LOGI("OpenGLSample construct");
    m_ProgramObj = 0;
    m_VertexShader = 0;
    m_FragmentShader = 0;

    m_VaoId = GL_NONE;
    m_ImageTextureId = GL_NONE;
    m_SamplerLoc = GL_NONE;
    m_TransFeedbackBufId = GL_NONE;
    m_TransFeedbackObjId = GL_NONE;
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
            -1.0f, -0.5f, 0.0f,
            1.0f, -0.5f, 0.0f,
            -1.0f,  0.5f, 0.0f,

            1.0f, -0.5f, 0.0f,
            1.0f,  0.5f, 0.0f,
            -1.0f,  0.5f, 0.0f,
    };
    GLfloat vTexCoors[] = {
            0.0f, 1.0f,
            1.0f, 1.0f,
            0.0f, 0.0f,

            1.0f, 1.0f,
            1.0f, 0.0f,
            0.0f, 0.0f,
    };

    char vShaderStr[] =
            "#version 300 es                            \n"
            "layout(location = 0) in vec4 a_position;   \n"
            "layout(location = 1) in vec2 a_texCoord;   \n"
            "out vec2 v_texCoord;                       \n"
            "out vec3 outPos;                           \n"
            "out vec2 outTex;                           \n"
            "void main()                                \n"
            "{                                          \n"
            "   gl_Position = a_position;               \n"
            "   v_texCoord = a_texCoord;                \n"
            "   outPos = vec3(a_position)*30.0;          \n"
            "   outTex = a_texCoord * 3.0;              \n"
            "}                                          \n";

    // 片段着色器脚本，简单纹理映射
    char fShaderStr[] =
            "#version 300 es\n"
            "precision mediump float;\n"
            "in vec2 v_texCoord;\n"
            "layout(location = 0) out vec4 outColor;\n"
            "uniform sampler2D s_TextureMap;\n"
            "void main()\n"
            "{\n"
            "    outColor = texture(s_TextureMap, v_texCoord);\n"
            "}";

    GLchar const * varyings[] = {"outPos", "outTex"};

    m_ProgramObj = GLUtils::CreateProgramWithFeedback(vShaderStr, fShaderStr,
                                                      m_VertexShader, m_FragmentShader,
                                                      varyings,
                                                      sizeof(varyings) / sizeof(varyings[0]));

    if (m_ProgramObj == GL_NONE )
    {
        LOGE("TransformFeedbackSample::Init m_ProgramObj == GL_NONE");
        return;
    }
    m_SamplerLoc = glGetUniformLocation(m_ProgramObj, "s_TextureMap");

    // 生成 VBO ，加载顶点数据和索引数据
    // Generate VBO Ids and load the VBOs with data
    glGenBuffers(2, m_VboIds);
    glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vVertices), vVertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vTexCoors), vTexCoors, GL_STATIC_DRAW);

    GO_CHECK_GL_ERROR();

    // Generate VAO Ids
    glGenVertexArrays(1, &m_VaoId);
    // Normal rendering VAO
    glBindVertexArray(m_VaoId);

    glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[0]);
    glEnableVertexAttribArray(VERTEX_POS_INDX);
    glVertexAttribPointer(VERTEX_POS_INDX, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (const void *)0);
    glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

    glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[1]);
    glEnableVertexAttribArray(TEXTURE_POS_INDX);
    glVertexAttribPointer(TEXTURE_POS_INDX, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (const void *)0);
    glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

    GO_CHECK_GL_ERROR();
    glBindVertexArray(GL_NONE);

    glGenBuffers(1, &m_TransFeedbackBufId);
    glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, m_TransFeedbackBufId);
    glBufferData(GL_TRANSFORM_FEEDBACK_BUFFER, (3+2) * 6 * sizeof(GLfloat), NULL, GL_STATIC_READ);
    glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, GL_NONE);

    glGenTransformFeedbacks(1, &m_TransFeedbackObjId);
    glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, m_TransFeedbackObjId);
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, m_TransFeedbackBufId);
    glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);
    glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, 0);

    // 创建并初始化图像纹理
    glGenTextures(1, &m_ImageTextureId);
    glBindTexture(GL_TEXTURE_2D, m_ImageTextureId);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_RenderImage.width, m_RenderImage.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_RenderImage.ppPlane[0]);
    glBindTexture(GL_TEXTURE_2D, GL_NONE);
    GO_CHECK_GL_ERROR();
}

void OpenGLSample::Draw(int screenWidth, int screenHeight) {
    // FBO render
    LOGI("OpenGLSample Draw screenWidth = %d, screenHeight = %d , image width = %d, image height = %d",
         screenWidth, screenHeight, m_RenderImage.width, m_RenderImage.height);
    // Do normal rendering
    glViewport(0, 0, screenWidth, screenHeight);
    glUseProgram(m_ProgramObj);
    glBindVertexArray(m_VaoId);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_ImageTextureId);
    glUniform1i(m_SamplerLoc, 0);

    glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, m_TransFeedbackObjId);
    glBeginTransformFeedback(GL_TRIANGLES);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glEndTransformFeedback();
    glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);

    glBindTexture(GL_TEXTURE_2D, GL_NONE);
    glBindVertexArray(GL_NONE);

    // Read feedback buffer
    glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, m_TransFeedbackBufId);
    void *rawData = glMapBufferRange(GL_TRANSFORM_FEEDBACK_BUFFER, 0, (3+2) * 6 * sizeof(GLfloat), GL_MAP_READ_BIT);

    float *p = (float*)rawData;
    for (int i=0; i<6; i++) {
        LOGI("read feedback buffer outPos[%d] = [%f, %f, %f], outTex[%d] = [%f, %f]",i, p[i*5], p[i*5+1], p[i*5+2], i, p[i*5+3], p[i*5+4]);
    }

    glUnmapBuffer(GL_TRANSFORM_FEEDBACK_BUFFER);
    glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, 0);
}