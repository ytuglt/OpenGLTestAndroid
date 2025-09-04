//
// Created by lixiang on 2025/8/23.
//

#ifndef OPENGLTRIANGLEDEMO_GLUTILS_H
#define OPENGLTRIANGLEDEMO_GLUTILS_H

#include <GLES3/gl3.h>

#define MATH_PI 3.1415926535897932384626433832802


class GLUtils {
public:
    static GLuint LoadShader(GLenum type, const char* shaderSrc);
    static GLuint CreateProgram(const char* vShaderStr, const char* fShaderStr, GLuint &vertexShaderHandle, GLuint &fragShaderHandle);
    static void DeleteProgram(GLuint &program);
    static GLuint CreateProgramWithFeedback(const char *pVertexShaderSource, const char *pFragShaderSource,
                                            GLuint &vertexShaderHandle, GLuint &fragShaderHandle,
                                            GLchar const **varying, int varyingCount);
    static void CheckGLError(const char* pGLOperation);
};


#endif //OPENGLTRIANGLEDEMO_GLUTILS_H
