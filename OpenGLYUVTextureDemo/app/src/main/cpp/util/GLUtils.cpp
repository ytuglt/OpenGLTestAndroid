//
// Created by lixiang on 2025/8/23.
//

#include "GLUtils.h"
#include "LogUtil.h"
#include <stdlib.h>

GLuint GLUtils::LoadShader(GLenum shaderType, const char *shaderSrc) {
    GLuint shader = 0;

    shader = glCreateShader(shaderType);
    if (!shader) {
        return shader;
    }

    glShaderSource(shader, 1, &shaderSrc, NULL);
    glCompileShader(shader);

    GLint compiled = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if (!compiled) {
        GLint infoLen = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
        if (infoLen) {
            char *buf = (char *) malloc((size_t) infoLen);
            if (buf) {
                glGetShaderInfoLog(shader, infoLen, NULL, buf);
                LOGI("GLUtils::LoadShader shader compile failed, shaderType : %d\n, infoLog: %s\n",
                     shaderType, buf);
                free(buf);
            }
            glDeleteShader(shader);
            shader = 0;
        }
    }
    return shader;
}

GLuint GLUtils::CreateProgram(const char *vShaderStr, const char *fShaderStr,
                              GLuint &vertexShaderHandle, GLuint &fragShaderHandle) {
    GLuint program = 0;

    vertexShaderHandle = LoadShader(GL_VERTEX_SHADER, vShaderStr);
    if (!vertexShaderHandle) {
        return program;
    }

    fragShaderHandle = LoadShader(GL_FRAGMENT_SHADER, fShaderStr);
    if (!fragShaderHandle) {
        return program;
    }

    program = glCreateProgram();
    if (!program) {
        return program;
    }
    glAttachShader(program, vertexShaderHandle);
    CheckGLError("glAttachShader");
    glAttachShader(program, fragShaderHandle);
    CheckGLError("glAttachShader");

    glLinkProgram(program);
    GLint linkStatus = GL_FALSE;
    glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);

    glDetachShader(program, vertexShaderHandle);
    glDeleteShader(vertexShaderHandle);
    vertexShaderHandle = 0;
    glDetachShader(program, fragShaderHandle);
    glDeleteShader(fragShaderHandle);
    fragShaderHandle = 0;

    if (linkStatus != GL_TRUE) {
        GLint bufLength = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufLength);
        if (bufLength) {
            char *buf = (char *) malloc((size_t) bufLength);
            if (buf) {
                glGetProgramInfoLog(program, bufLength, NULL, buf);
                LOGI("GLUtils::CreateProgram program link failed, infoLog: %s\n", buf);
                free(buf);
            }
        }
        glDeleteProgram(program);
        program = 0;
    }

    LOGI("GLUtils::CreateProgram program link success, program: %d", program);
    return program;
}

void GLUtils::CheckGLError(const char *pGLOperation) {
    for (GLint error = glGetError(); error; error = glGetError()) {
        LOGE("GLUtils::CheckGLError GL Operation %s(), glError: (0x%x)\n", pGLOperation, error);
    }
}

void GLUtils::DeleteProgram(GLuint &program) {
    LOGI("GLUtils::DeleteProgram program: %d", program);
    if (program) {
        glUseProgram(0);
        glDeleteProgram(program);
        program = 0;
    }
}


