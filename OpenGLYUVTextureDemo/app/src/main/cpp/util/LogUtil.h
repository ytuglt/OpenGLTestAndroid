//
// Created by lixiang on 2025/8/23.
//

#ifndef OPENGLTEST_LOGUTIL_H
#define OPENGLTEST_LOGUTIL_H

#include <android/log.h>

#define LOG_TAG "OpenGLTriangleDemo"

#define LOGE(format, ...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, format, ##__VA_ARGS__)
#define LOGI(format, ...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, format, ##__VA_ARGS__)

#endif //OPENGLTEST_LOGUTIL_H
