//
// Created by lixiang on 2025/8/23.
//

#include "OpenGLSample.h"
//#include "../util/LogUtil.h"
#include "../glm/gtc/matrix_transform.hpp"

#define VERTEX_POS_INDX  0
#define TEXTURE_POS_INDX 1

OpenGLSample::OpenGLSample() {
    LOGI("OpenGLSample construct");
    m_ProgramObj = 0;
    m_VertexShader = 0;
    m_FragmentShader = 0;

    m_SamplerLoc = GL_NONE;
    m_MVPMatLoc = GL_NONE;
    m_ModelMatrixLoc = GL_NONE;
    m_LightPosLoc = GL_NONE;

    m_TextureId = GL_NONE;
    m_VaoId = GL_NONE;

    m_AngleX = 0;
    m_AngleY = 0;

    m_ScaleX = 1.0f;
    m_ScaleY = 1.0f;

    m_ModelMatrix = glm::mat4(0.0f);
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
    if (m_ProgramObj) return;

    //create RGBA texture
    glGenTextures(1, &m_TextureId);
    glBindTexture(GL_TEXTURE_2D, m_TextureId);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, GL_NONE);

    char vShaderStr[] = R"(
        #version 300 es
        precision mediump float;

        layout(location = 0) in vec4 a_position;
        layout(location = 1) in vec2 a_texCoord;
        layout(location = 2) in vec3 a_normal;

        uniform mat4 u_MVPMatrix;
        uniform mat4 u_ModelMatrix;
        uniform vec3 lightPos;
        uniform vec3 lightColor;
        uniform vec3 viewPos;

        out vec2 v_texCoord;
        out vec3 ambient;
        out vec3 diffuse;
        out vec3 specular;

        void main() {
            gl_Position = u_MVPMatrix * a_position;
            vec3 fragPos = vec3(u_ModelMatrix * a_position);

            // Ambient
            float ambientStrength = 0.1;
            ambient = ambientStrength * lightColor;

            // Diffuse
            float diffuseStrength = 0.5;
            vec3 unitNormal = normalize(vec3(u_ModelMatrix * vec4(a_normal, 1.0)));
            vec3 lightDir = normalize(lightPos - fragPos);
            float diff = max(dot(unitNormal, lightDir), 0.0);
            diffuse = diffuseStrength * diff * lightColor;

            // specular
            float specularStrength = 0.9;
            vec3 viewDir = normalize(viewPos - fragPos);
            vec3 reflectDir = reflect(-lightDir, unitNormal);
            float spec = pow(max(dot(unitNormal, reflectDir), 0.0), 16.0);
            specular = specularStrength * spec * lightColor;

            v_texCoord = a_texCoord;
        }
    )";

    char fShaderStr[] = R"(
        #version 300 es
        precision mediump float;

        layout(location = 0) out vec4 outColor;

        in vec2 v_texCoord;
        in vec3 ambient;
        in vec3 diffuse;
        in vec3 specular;

        uniform sampler2D s_TextureMap;

        void main() {
            vec4 objectColor = texture(s_TextureMap, v_texCoord);
            vec3 finalColor = (ambient + diffuse + specular) * vec3(objectColor);
            outColor = vec4(finalColor, 1.0);
        }
    )";

    m_ProgramObj = GLUtils::CreateProgram(vShaderStr, fShaderStr, m_VertexShader, m_FragmentShader);
    if (m_ProgramObj)
    {
        m_SamplerLoc = glGetUniformLocation(m_ProgramObj, "s_TextureMap");
        m_MVPMatLoc = glGetUniformLocation(m_ProgramObj, "u_MVPMatrix");
        m_ModelMatrixLoc = glGetUniformLocation(m_ProgramObj, "u_ModelMatrix");
        m_LightPosLoc = glGetUniformLocation(m_ProgramObj, "lightPos");
        m_LightColorLoc = glGetUniformLocation(m_ProgramObj, "lightColor");
        m_ViewPosLoc = glGetUniformLocation(m_ProgramObj, "viewPos");
    } else {
        LOGE("OpenGLSample Init failed");
        return;
    }

    GLfloat vertices[] = {
            //position            //texture coord  //normal
            -0.5f, -0.5f, -0.5f,   0.0f, 0.0f,      0.0f,  0.0f, -1.0f,
            0.5f, -0.5f, -0.5f,   1.0f, 0.0f,      0.0f,  0.0f, -1.0f,
            0.5f,  0.5f, -0.5f,   1.0f, 1.0f,      0.0f,  0.0f, -1.0f,
            0.5f,  0.5f, -0.5f,   1.0f, 1.0f,      0.0f,  0.0f, -1.0f,
            -0.5f,  0.5f, -0.5f,   0.0f, 1.0f,      0.0f,  0.0f, -1.0f,
            -0.5f, -0.5f, -0.5f,   0.0f, 0.0f,      0.0f,  0.0f, -1.0f,

            -0.5f, -0.5f, 0.5f,    0.0f, 0.0f,      0.0f,  0.0f,  1.0f,
            0.5f, -0.5f, 0.5f,    1.0f, 0.0f,      0.0f,  0.0f,  1.0f,
            0.5f,  0.5f, 0.5f,    1.0f, 1.0f,      0.0f,  0.0f,  1.0f,
            0.5f,  0.5f, 0.5f,    1.0f, 1.0f,      0.0f,  0.0f,  1.0f,
            -0.5f,  0.5f, 0.5f,    0.0f, 1.0f,      0.0f,  0.0f,  1.0f,
            -0.5f, -0.5f, 0.5f,    0.0f, 0.0f,      0.0f,  0.0f,  1.0f,

            -0.5f,  0.5f,  0.5f,   1.0f, 0.0f,     -1.0f,  0.0f,  0.0f,
            -0.5f,  0.5f, -0.5f,   1.0f, 1.0f,     -1.0f,  0.0f,  0.0f,
            -0.5f, -0.5f, -0.5f,   0.0f, 1.0f,     -1.0f,  0.0f,  0.0f,
            -0.5f, -0.5f, -0.5f,   0.0f, 1.0f,     -1.0f,  0.0f,  0.0f,
            -0.5f, -0.5f,  0.5f,   0.0f, 0.0f,     -1.0f,  0.0f,  0.0f,
            -0.5f,  0.5f,  0.5f,   1.0f, 0.0f,     -1.0f,  0.0f,  0.0f,

            0.5f,  0.5f,  0.5f,   1.0f, 0.0f,      1.0f,  0.0f,  0.0f,
            0.5f,  0.5f, -0.5f,   1.0f, 1.0f,      1.0f,  0.0f,  0.0f,
            0.5f, -0.5f, -0.5f,   0.0f, 1.0f,      1.0f,  0.0f,  0.0f,
            0.5f, -0.5f, -0.5f,   0.0f, 1.0f,      1.0f,  0.0f,  0.0f,
            0.5f, -0.5f,  0.5f,   0.0f, 0.0f,      1.0f,  0.0f,  0.0f,
            0.5f,  0.5f,  0.5f,   1.0f, 0.0f,      1.0f,  0.0f,  0.0f,

            -0.5f, -0.5f, -0.5f,   0.0f, 1.0f,      0.0f, -1.0f,  0.0f,
            0.5f, -0.5f, -0.5f,   1.0f, 1.0f,      0.0f, -1.0f,  0.0f,
            0.5f, -0.5f,  0.5f,   1.0f, 0.0f,      0.0f, -1.0f,  0.0f,
            0.5f, -0.5f,  0.5f,   1.0f, 0.0f,      0.0f, -1.0f,  0.0f,
            -0.5f, -0.5f,  0.5f,   0.0f, 0.0f,      0.0f, -1.0f,  0.0f,
            -0.5f, -0.5f, -0.5f,   0.0f, 1.0f,      0.0f, -1.0f,  0.0f,

            -0.5f, 0.5f, -0.5f,    0.0f, 1.0f,      0.0f,  1.0f,  0.0f,
            0.5f, 0.5f, -0.5f,    1.0f, 1.0f,      0.0f,  1.0f,  0.0f,
            0.5f, 0.5f,  0.5f,    1.0f, 0.0f,      0.0f,  1.0f,  0.0f,
            0.5f, 0.5f,  0.5f,    1.0f, 0.0f,      0.0f,  1.0f,  0.0f,
            -0.5f, 0.5f,  0.5f,    0.0f, 0.0f,      0.0f,  1.0f,  0.0f,
            -0.5f, 0.5f, -0.5f,    0.0f, 1.0f,      0.0f,  1.0f,  0.0f,
    };

    // Generate VBO Ids and load the VBOs with data
    glGenBuffers(1, m_VboIds);
    glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Generate VAO Id
    glGenVertexArrays(1, &m_VaoId);
    glBindVertexArray(m_VaoId);

    glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[0]);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (const void *) 0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (const void *) (3* sizeof(GLfloat)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (const void *) (5* sizeof(GLfloat)));
    glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

    glBindVertexArray(GL_NONE);
}

void OpenGLSample::Draw(int screenWidth, int screenHeight) {
    // FBO render
    LOGI("OpenGLSample Draw screenWidth = %d, screenHeight = %d , image width = %d, image height = %d",
         screenWidth, screenHeight, m_RenderImage.width, m_RenderImage.height);

    if(m_ProgramObj == GL_NONE || m_TextureId == GL_NONE) return;
    glEnable(GL_DEPTH_TEST);


    UpdateMVPMatrix(m_MVPMatrix, m_AngleX, m_AngleY, (float)screenWidth / screenHeight);

    //upload RGBA image data
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_TextureId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_RenderImage.width, m_RenderImage.height, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, m_RenderImage.ppPlane[0]);
    glBindTexture(GL_TEXTURE_2D, GL_NONE);

    // use the program object
    glUseProgram(m_ProgramObj);

    glBindVertexArray(m_VaoId);

    glUniformMatrix4fv(m_MVPMatLoc, 1, GL_FALSE, &m_MVPMatrix[0][0]);
    glUniformMatrix4fv(m_ModelMatrixLoc, 1, GL_FALSE, &m_ModelMatrix[0][0]);

    glUniform3f(m_LightColorLoc,  1.0f, 1.0f, 1.0f);
    glUniform3f(m_LightPosLoc,    -2.0f, 0.0f, 2.0f);
    glUniform3f(m_ViewPosLoc,     -3.0f, 0.0f, 3.0f);

    // Bind the RGBA map
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_TextureId);
    glUniform1i(m_SamplerLoc, 0);

//    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (const void *)0);

    glDrawArrays(GL_TRIANGLES, 0, 36);
    GO_CHECK_GL_ERROR();
}

/**
 * @param angleX 绕X轴旋转度数
 * @param angleY 绕Y轴旋转度数
 * @param ratio 宽高比
 * */
void OpenGLSample::UpdateMVPMatrix(glm::mat4 &mvpMatrix, int angleX, int angleY, float ratio) {
    LOGI("OpenGLSample UpdateMVPMatrix, angleX = %d, angleY = %d, ratio = %f", angleX, angleY, ratio);

    angleX = angleX % 360;
    angleY = angleY % 360;

    // to radians
    float radiansX = static_cast<float>(MATH_PI / 180.0f * angleX);
    float radiansY = static_cast<float>(MATH_PI / 180.0f * angleY);

    // projection matrix
    glm::mat4 Projection = glm::perspective(glm::radians(45.0f), ratio, 0.1f, 100.0f);

    // view matrix
    glm::mat4 View = glm::lookAt(
            glm::vec3(0, 0, 4), // Camera position
            glm::vec3(0, 0, 0), // looks at origin
            glm::vec3(0, 1, 0) // up direction
            );

    // model matrix
    glm::mat4 Model = glm::mat4(1.0f);
    Model = glm::scale(Model, glm::vec3(m_ScaleX, m_ScaleY, 1.0f));
    Model = glm::rotate(Model, radiansX, glm::vec3(1.0f, 0.0f, 0.0f));
    Model = glm::rotate(Model, radiansY, glm::vec3(0.0f, 1.0f, 0.0f));
    Model = glm::translate(Model, glm::vec3(0.0f, 0.0f, 0.0f));

    m_ModelMatrix = Model;

    mvpMatrix = Projection * View * Model;
}

void OpenGLSample::UpdateTransformMatrix(float rotateX, float rotateY, float scaleX, float scaleY)
{
    m_AngleX = static_cast<int>(rotateX);
    m_AngleY = static_cast<int>(rotateY);
    m_ScaleX = scaleX;
    m_ScaleY = scaleY;
}

void OpenGLSample::Destroy()
{
    if (m_ProgramObj)
    {
        glDeleteProgram(m_ProgramObj);
        glDeleteBuffers(3, m_VboIds);
        glDeleteVertexArrays(1, &m_VaoId);
        glDeleteTextures(1, &m_TextureId);
    }
}
