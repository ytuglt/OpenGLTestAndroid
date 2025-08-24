//
// Created by lixiang on 2025/8/23.
//

#ifndef OPENGLTRIANGLEDEMO_GLUTILS_H
#define OPENGLTRIANGLEDEMO_GLUTILS_H

#include <GLES3/gl3.h>


class GLUtils {
public:
    static GLuint LoadShader(GLenum type, const char* shaderSrc);
    static GLuint CreateProgram(const char* vShaderStr, const char* fShaderStr, GLuint &vertexShaderHandle, GLuint &fragShaderHandle);
    static void DeleteProgram(GLuint &program);
    static void CheckGLError(const char* pGLOperation);
};


#endif //OPENGLTRIANGLEDEMO_GLUTILS_H
