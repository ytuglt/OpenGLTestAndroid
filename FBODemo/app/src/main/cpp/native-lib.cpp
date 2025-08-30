#include <jni.h>
#include <string>
#include "util/LogUtil.h"
#include "MyGLRenderContext.h"

#define NATIVE_RENDER_CLASS_NAME "com/example/fbodemo/MyNativeRender"

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
