//
// Created by lixiang on 2025/8/23.
//

#ifndef OPENGLTEST_MYGLRENDERCONTEXT_H
#define OPENGLTEST_MYGLRENDERCONTEXT_H

#include <GLES3/gl3.h>
#include "sample/OpenGLSample.h"

class MyGLRenderContext {
private:
    MyGLRenderContext();
    ~MyGLRenderContext();

public:
    void SetImageData(int format, int width, int height, uint8_t *pData);
    void SetImageDataWithIndex(int index, int format, int width, int height, uint8_t *pData);
    void OnSurfaceCreated();
    void OnSurfaceChanged(int width, int height);
    void OnDrawFrame();
    void UpdateTransformMatrix(float rotateX, float rotateY, float scaleX, float scaleY);

    static MyGLRenderContext* GetInstance();
    static void DestroyInstance();

private:
    static MyGLRenderContext* mInstance;
    OpenGLSample m_Sample;
    int m_ScreenWidth;
    int m_ScreenHeight;
};


#endif //OPENGLTEST_MYGLRENDERCONTEXT_H
