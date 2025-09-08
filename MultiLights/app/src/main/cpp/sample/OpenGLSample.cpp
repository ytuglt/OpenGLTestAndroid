//
// Created by lixiang on 2025/8/23.
//

#include "OpenGLSample.h"
//#include "../util/LogUtil.h"
#include "../glm/gtc/matrix_transform.hpp"

glm::vec3 transPositions[] = {
        glm::vec3( 0.0f,  0.0f,  0.0f),
        glm::vec3( 2.0f,  2.0f, -1.0f),
        glm::vec3(-1.5f, -2.2f, -1.5f),
        glm::vec3(-1.8f, -2.0f,  1.3f),
        glm::vec3( 1.4f, -1.4f, -1.5f),
        glm::vec3(-1.7f,  2.0f, -1.5f),
        glm::vec3( 1.3f, -2.0f,  2.5f),
        glm::vec3( 0.5f,  1.3f, -0.1f),
        glm::vec3( 1.5f,  2.2f,  1.5f),
        glm::vec3(-1.3f,  1.0f, -1.5f),
        glm::vec3(-1.3f,  0.0f, -1.5f),
        glm::vec3( 0.0f, -1.3f, -0.5f),
        glm::vec3( 0.0f, -1.5f,  1.5f),
};

OpenGLSample::OpenGLSample() {
    LOGI("OpenGLSample construct");
    m_ProgramObj = 0;
    m_VertexShader = 0;
    m_FragmentShader = 0;

    m_SamplerLoc = GL_NONE;
    m_MVPMatLoc = GL_NONE;
    m_ModelMatrixLoc = GL_NONE;

    m_TextureId = GL_NONE;
    m_VaoId = GL_NONE;

    m_AngleX = 0;
    m_AngleY = 0;


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

        out vec3 normal;
        out vec3 fragPos;
        out vec2 v_texCoord;

        uniform mat4 u_MVPMatrix;
        uniform mat4 u_ModelMatrix;

        void main()
        {
            gl_Position = u_MVPMatrix * a_position;
            fragPos = vec3(u_ModelMatrix * a_position);
            normal = mat3(transpose(inverse(u_ModelMatrix))) * a_normal;
            v_texCoord = a_texCoord;
        }
    )";

    char fShaderStr[] = R"(
        #version 300 es
        precision mediump float;

        struct Light {
            vec3 position;
            vec3 direction;
            vec3 color;
            float cutOff;
            float outerCutOff;

            float constant;
            float linear;
            float quadratic;
        };

        in vec3 normal;
        in vec3 fragPos;
        in vec2 v_texCoord;

        layout(location = 0) out vec4 outColor;
        uniform sampler2D s_TextureMap;

        uniform vec3 viewPos;
        uniform Light light;

        void main()
        {
            vec4 objectColor = texture(s_TextureMap, v_texCoord);

            vec3 lightDir = normalize(light.position - fragPos);

            // check if lighting is inside the spotlight cone
            float theta = dot(lightDir, normalize(-light.direction));

            float epsilon = light.cutOff - light.outerCutOff;
            float intensity = clamp((theta - light.outerCutOff) / epsilon,0.0, 1.0);

            // ambient
            float ambientStrength = 0.25;
            vec3 ambient = ambientStrength * light.color;

            // diffuse
            vec3 norm = normalize(normal);
            float diff = max(dot(norm, lightDir), 0.0);
            vec3 diffuse = diff * light.color;

            // specular
            vec3 viewDir = normalize(viewPos - fragPos);
            vec3 reflectDir = reflect(-lightDir, norm);
            float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
            vec3 specular = spec * light.color;

            // attenuation
            float distance = length(light.position - fragPos);
            float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

            // ambient  *= attenuation;  // Also remove attenuation from ambient, because if we move too far, the light in spotlight would then be darker than outside (since outside spotlight we have ambient lighting).
            diffuse  *= attenuation;
			specular *= attenuation;

			diffuse *= intensity;
			specular*= intensity;

			vec3 finalColor = (ambient + diffuse + specular) * vec3(objectColor);
			outColor = vec4(finalColor, 1.0f);
        }
    )";

    m_ProgramObj = GLUtils::CreateProgram(vShaderStr, fShaderStr, m_VertexShader, m_FragmentShader);
    if (m_ProgramObj)
    {
        m_SamplerLoc = glGetUniformLocation(m_ProgramObj, "s_TextureMap");
        GO_CHECK_GL_ERROR();
        m_MVPMatLoc = glGetUniformLocation(m_ProgramObj, "u_MVPMatrix");
        GO_CHECK_GL_ERROR();
        m_ModelMatrixLoc = glGetUniformLocation(m_ProgramObj, "u_ModelMatrix");
        GO_CHECK_GL_ERROR();
        m_ViewPosLoc = glGetUniformLocation(m_ProgramObj, "viewPos");
        GO_CHECK_GL_ERROR();
    }
    else
    {
        LOGE("MultiLightsSample::Init create program fail");
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

    //upload RGBA image data
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_TextureId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_RenderImage.width, m_RenderImage.height, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, m_RenderImage.ppPlane[0]);
    glBindTexture(GL_TEXTURE_2D, GL_NONE);
}

void OpenGLSample::Draw(int screenWidth, int screenHeight) {
    if (m_ProgramObj == GL_NONE || m_TextureId == GL_NONE) return;
    glEnable(GL_DEPTH_TEST);

    float ratio = (float)screenWidth / screenHeight;

    // Use the program object
    glUseProgram(m_ProgramObj);

    glBindVertexArray(m_VaoId);

    glUniform3f(m_ViewPosLoc,     0.0f, 0.0f, 3.0f);

    // Bind the RGBA map
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_TextureId);
    glUniform1i(m_SamplerLoc, 0);

    // 设置光源的位置、颜色和方向
    glUniform3f(glGetUniformLocation(m_ProgramObj, "light.position"), 0.0f, 0.0f, 3.0f);
    glUniform3f(glGetUniformLocation(m_ProgramObj, "light.color"), 1.0f, 1.0f, 1.0f);
    glUniform3f(glGetUniformLocation(m_ProgramObj, "light.direction"), 0.0f, 0.0f, -1.0f);

    // 用于计算边缘的过度，cutOff 表示内切光角，outerCutOff 表示外切光角
    glUniform1f(glGetUniformLocation(m_ProgramObj, "light.cutOff"), glm::cos(glm::radians(10.5f)));
    glUniform1f(glGetUniformLocation(m_ProgramObj, "light.outerCutOff"), glm::cos(glm::radians(11.5f)));

    // 衰减系数,常数项 constant，一次项 linear 和二次项 quadratic。
    glUniform1f(glGetUniformLocation(m_ProgramObj, "light.constant"),  1.0f);
    glUniform1f(glGetUniformLocation(m_ProgramObj, "light.linear"),    0.09);
    glUniform1f(glGetUniformLocation(m_ProgramObj, "light.quadratic"), 0.032);

    // 绘制多个立方体，不同的位移和旋转角度
    for(int i = 0; i < sizeof(transPositions)/ sizeof(transPositions[0]); i++)
    {
        UpdateMatrix(m_MVPMatrix, m_ModelMatrix, m_AngleX + 10, m_AngleY + 10, 0.4, transPositions[i], ratio);
        glUniformMatrix4fv(m_MVPMatLoc, 1, GL_FALSE, &m_MVPMatrix[0][0]);
        glUniformMatrix4fv(m_ModelMatrixLoc, 1, GL_FALSE, &m_ModelMatrix[0][0]);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
}

/**
 * @param angleX 绕X轴旋转度数
 * @param angleY 绕Y轴旋转度数
 * @param ratio 宽高比
 * */
void OpenGLSample::UpdateMVPMatrix(glm::mat4 &mvpMatrix, int angleX, int angleY, float ratio) {
    LOGI("OpenGLSample UpdateMVPMatrix, angleX = %d, angleY = %d, ratio = %f", angleX, angleY, ratio);
}

void OpenGLSample::UpdateMatrix(glm::mat4 &mvpMatrix, glm::mat4 &modelMatrix, int angleXRotate, int angleYRotate, float scale, glm::vec3 transVec3, float ratio)
{
    LOGE("MultiLightsSample::UpdateMatrix angleX = %d, angleY = %d, ratio = %f", angleXRotate,
            angleYRotate, ratio);
    angleXRotate = angleXRotate % 360;
    angleYRotate = angleYRotate % 360;

    //转化为弧度角
    float radiansX = static_cast<float>(MATH_PI / 180.0f * angleXRotate);
    float radiansY = static_cast<float>(MATH_PI / 180.0f * angleYRotate);


    // Projection matrix
    //glm::mat4 Projection = glm::ortho(-ratio, ratio, -1.0f, 1.0f, 0.0f, 100.0f);
    //glm::mat4 Projection = glm::frustum(-ratio, ratio, -1.0f, 1.0f, 4.0f, 100.0f);
    glm::mat4 Projection = glm::perspective(45.0f, ratio, 0.1f, 100.f);

    // View matrix
    glm::mat4 View = glm::lookAt(
            glm::vec3(0, 0, 3), // Camera is at (0,0,1), in World Space
            glm::vec3(0, 0, 0), // and looks at the origin
            glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
    );

    // Model matrix
    glm::mat4 Model = glm::mat4(1.0f);
    Model = glm::scale(Model, glm::vec3(scale, scale, scale));
    Model = glm::rotate(Model, radiansX, glm::vec3(1.0f, 0.0f, 0.0f));
    Model = glm::rotate(Model, radiansY, glm::vec3(0.0f, 1.0f, 0.0f));
    Model = glm::translate(Model, transVec3);

    modelMatrix = Model;

    mvpMatrix = Projection * View * Model;
}

void OpenGLSample::UpdateTransformMatrix(float rotateX, float rotateY, float scaleX, float scaleY)
{
    m_AngleX = static_cast<int>(rotateX);
    m_AngleY = static_cast<int>(rotateY);
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
