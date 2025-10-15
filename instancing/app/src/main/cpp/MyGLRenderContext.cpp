//
// Created by lixiang on 2025/8/23.
//

#include "MyGLRenderContext.h"
#include "util/LogUtil.h"
#include "util/ImageDef.h"

MyGLRenderContext* MyGLRenderContext::mInstance = nullptr;

MyGLRenderContext::MyGLRenderContext() {
    LOGI("MyGLRenderContext");
}

MyGLRenderContext::~MyGLRenderContext() {
    LOGI("~MyGLRenderContext");
}

void MyGLRenderContext::SetImageData(int format, int width, int height, uint8_t *pData) {
    LOGI("SetImageData format=%d, width = %d, height = %d, pData = %p", format, width, height, pData);
    NativeImage  nativeImage;
    nativeImage.format = format;
    nativeImage.width = width;
    nativeImage.height = height;
    nativeImage.ppPlane[0] = pData;

    switch (format) {
        case IMAGE_FORMAT_NV21:
        case IMAGE_FORMAT_NV12:
            nativeImage.ppPlane[1] = pData + width * height;
            break;
        default:
            break;
    }

    m_Sample.LoadImage(&nativeImage);
}

void MyGLRenderContext::SetImageDataWithIndex(int index, int format, int width, int height, uint8_t *pData)
{
    LOGI("MyGLRenderContext::SetImageDataWithIndex index=%d, format=%d, width=%d, height=%d, pData=%p", index, format, width, height, pData);
    NativeImage nativeImage;
    nativeImage.format = format;
    nativeImage.width = width;
    nativeImage.height = height;
    nativeImage.ppPlane[0] = pData;

    switch (format)
    {
        case IMAGE_FORMAT_NV12:
        case IMAGE_FORMAT_NV21:
            nativeImage.ppPlane[1] = nativeImage.ppPlane[0] + width * height;
            break;
        case IMAGE_FORMAT_I420:
            nativeImage.ppPlane[1] = nativeImage.ppPlane[0] + width * height;
            nativeImage.ppPlane[2] = nativeImage.ppPlane[1] + width * height / 4;
            break;
        default:
            break;
    }

    m_Sample.LoadMultiImageWithIndex(index, &nativeImage);
}

void MyGLRenderContext::OnSurfaceCreated() {
    LOGI("OnSurfaceCreated");
    glClearColor(1.0f,1.0f,1.0f, 1.0f);
    m_Sample.Init();
}

void MyGLRenderContext::OnSurfaceChanged(int width, int height) {
    LOGI("OnSurfaceChanged width = %d, height = %d", width, height);\
    glViewport(0, 0, width, height);
    m_ScreenWidth = width;
    m_ScreenHeight = height;
}

void MyGLRenderContext::OnDrawFrame() {
//    LOGI("OnDrawFrame");
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    m_Sample.Draw(m_ScreenWidth, m_ScreenHeight);
}

void MyGLRenderContext::UpdateTransformMatrix(float rotateX, float rotateY, float scaleX, float scaleY)
{
    m_Sample.UpdateTransformMatrix(rotateX, rotateY, scaleX, scaleY);
}

MyGLRenderContext *MyGLRenderContext::GetInstance() {
    if (mInstance == nullptr) {
        mInstance = new MyGLRenderContext();
    }
    return mInstance;
}

void MyGLRenderContext::DestroyInstance() {
    if (mInstance != nullptr) {
        delete mInstance;
        mInstance = nullptr;
    }
}

