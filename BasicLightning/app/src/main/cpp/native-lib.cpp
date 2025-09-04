#include <jni.h>
#include <string>
#include "util/LogUtil.h"
#include "MyGLRenderContext.h"
#include "render/EGLRender.h"

#define NATIVE_RENDER_CLASS_NAME "com/example/basiclightning/MyNativeRender"
#define NATIVE_BG_RENDER_CLASS_NAME "com/example/basiclightning/egl/NativeEglRender"

#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT void JNICALL native_onInit(JNIEnv *env, jobject instance) {
    LOGI("native_onInit");
    MyGLRenderContext::GetInstance();
}

JNIEXPORT void JNICALL native_OnUnInit(JNIEnv *env, jobject instance) {
    LOGI("native_OnUnInit");
    MyGLRenderContext::DestroyInstance();
}

JNIEXPORT void JNICALL native_SetImageData(JNIEnv *env, jobject instance, jint format, jint width, jint height, jbyteArray imageData) {
    LOGI("native_SetImageData");
    int len = env->GetArrayLength (imageData);
    uint8_t* buf = new uint8_t[len];
    env->GetByteArrayRegion(imageData, 0, len, reinterpret_cast<jbyte*>(buf));
    MyGLRenderContext::GetInstance()->SetImageData(format, width, height, buf);
    delete[] buf;
    env->DeleteLocalRef(imageData);
}

JNIEXPORT void JNICALL native_OnSurfaceCreated(JNIEnv *env, jobject instance) {
    LOGI("native_OnSurfaceCreated");
    MyGLRenderContext::GetInstance()->OnSurfaceCreated();
}

JNIEXPORT void JNICALL native_OnSurfaceChanged(JNIEnv *env, jobject instance, jint width, jint height) {
    LOGI("native_OnSurfaceChanged");
    MyGLRenderContext::GetInstance()->OnSurfaceChanged(width, height);
}

JNIEXPORT void JNICALL native_OnDrawFrame(JNIEnv *env, jobject instance) {
//    LOGI("native_OnDrawFrame");
    MyGLRenderContext::GetInstance()->OnDrawFrame();
}

/*
 * Class:     com_byteflow_app_MyNativeRender
 * Method:    native_UpdateTransformMatrix
 * Signature: (FFFF)V
 */
JNIEXPORT void JNICALL native_UpdateTransformMatrix(JNIEnv *env, jobject instance, jfloat rotateX, jfloat rotateY, jfloat scaleX, jfloat scaleY)
{
    MyGLRenderContext::GetInstance()->UpdateTransformMatrix(rotateX, rotateY, scaleX, scaleY);
}


// -----------------------------------------------------------------------------------------
/*
 * Class:     com_byteflow_app_egl_NativeBgRender
 * Method:    native_EglRenderInit
 * Signature: ()V
 */
JNIEXPORT void JNICALL native_EglRenderInit(JNIEnv *env, jobject instance)
{
    EGLRender::GetInstance()->Init();

}

/*
 * Class:     com_byteflow_app_egl_NativeBgRender
 * Method:    native_EglRenderSetImageData
 * Signature: ([BII)V
 */
JNIEXPORT void JNICALL native_EglRenderSetImageData(JNIEnv *env, jobject instance, jbyteArray data, jint width, jint height)
{
    int len = env->GetArrayLength (data);
    uint8_t* buf = new uint8_t[len];
    env->GetByteArrayRegion(data, 0, len, reinterpret_cast<jbyte*>(buf));
    EGLRender::GetInstance()->SetImageData(buf, width, height);
    delete[] buf;
    env->DeleteLocalRef(data);


}

/*
 * Class:     com_byteflow_app_egl_NativeBgRender
 * Method:    native_EglRenderSetIntParams
 * Signature: (II)V
 */
JNIEXPORT void JNICALL native_EglRenderSetIntParams(JNIEnv *env, jobject instance, jint type, jint param)
{
    EGLRender::GetInstance()->SetIntParams(type, param);

}

/*
 * Class:     com_byteflow_app_egl_NativeBgRender
 * Method:    native_EglRenderDraw
 * Signature: ()V
 */
JNIEXPORT void JNICALL native_EglRenderDraw(JNIEnv *env, jobject instance)
{
    EGLRender::GetInstance()->Draw();
}

/*
 * Class:     com_byteflow_app_egl_NativeBgRender
 * Method:    natuve_BgRenderUnInit
 * Signature: ()V
 */
JNIEXPORT void JNICALL natuve_BgRenderUnInit(JNIEnv *env, jobject instance)
{
    EGLRender::GetInstance()->UnInit();
}


#ifdef __cplusplus
}
#endif

static JNINativeMethod  g_RenderMethods[] = {
        {"native_OnInit", "()V", (void*) native_onInit},
        {"native_OnUnInit", "()V", (void*) native_OnUnInit},
        {"native_SetImageData", "(III[B)V", (void*) native_SetImageData},
        {"native_OnSurfaceCreated", "()V", (void*) native_OnSurfaceCreated},
        {"native_OnSurfaceChanged", "(II)V", (void*) native_OnSurfaceChanged},
        {"native_OnDrawFrame", "()V", (void*) native_OnDrawFrame},
        {"native_UpdateTransformMatrix",     "(FFFF)V",   (void *)(native_UpdateTransformMatrix)},

};

static JNINativeMethod g_BgRenderMethods[] = {
        {"native_EglRenderInit",          "()V",       (void *)(native_EglRenderInit)},
        {"native_EglRenderSetImageData",  "([BII)V",   (void *)(native_EglRenderSetImageData)},
        {"native_EglRenderSetIntParams",  "(II)V",     (void *)(native_EglRenderSetIntParams)},
        {"native_EglRenderDraw",          "()V",       (void *)(native_EglRenderDraw)},
        {"native_EglRenderUnInit",        "()V",       (void *)(natuve_BgRenderUnInit)},
};

static int RegisterNativeMethods(JNIEnv *env, const char *className, JNINativeMethod *methods, int mehodNum) {
    LOGI("RegisterNativeMethods");
    jclass clazz = env->FindClass(className);
    if (clazz == NULL) {
        LOGE("RegisterNativeMethods: FindClass failed");
        return JNI_FALSE;
    }

    if (env->RegisterNatives(clazz, methods, mehodNum) < 0) {
        LOGE("RegisterNativeMethods: RegisterNatives failed");
        return JNI_FALSE;
    }

    return JNI_TRUE;
}

static void UnRegisterNativeMethods(JNIEnv *env, const char *className) {
    LOGI("UnRegisterNativeMethods");
    jclass clazz = env->FindClass(className);
    if (clazz == NULL) {
        LOGE("UnRegisterNativeMethods: FindClass failed");
        return;
    }
    if (env != NULL) {
        env->UnregisterNatives(clazz);
    }
}


extern "C" jint JNI_OnLoad(JavaVM *jvm, void *p) {
    LOGI(" ====== JNI_OnLoad ======");
    jint jniRet = JNI_ERR;
    JNIEnv *env = NULL;

    if (jvm->GetEnv((void**) (&env), JNI_VERSION_1_6) != JNI_OK) {
        return jniRet;
    }

    jint regRet = RegisterNativeMethods(env, NATIVE_RENDER_CLASS_NAME,
                                        g_RenderMethods,
                                        sizeof(g_RenderMethods) / sizeof(g_RenderMethods[0]));
    if (regRet != JNI_TRUE) {
        return jniRet;
    }

    regRet = RegisterNativeMethods(env, NATIVE_BG_RENDER_CLASS_NAME, g_BgRenderMethods,
                                   sizeof(g_BgRenderMethods) /
                                   sizeof(g_BgRenderMethods[0]));
    if (regRet != JNI_TRUE)
    {
        return JNI_ERR;
    }

    return JNI_VERSION_1_6;
}

extern "C" void JNI_OnUnload(JavaVM *jvm, void *p) {
    LOGI(" ====== JNI_UnLoad ======");
    JNIEnv *env = NULL;
    if (jvm->GetEnv((void**) (&env), JNI_VERSION_1_6) != JNI_OK) {
        return;
    }
    UnRegisterNativeMethods(NULL, NATIVE_RENDER_CLASS_NAME);
}
