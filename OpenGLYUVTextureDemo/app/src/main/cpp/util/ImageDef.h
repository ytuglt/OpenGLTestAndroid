//
// Created by lixiang on 2025/8/24.
//

#ifndef OPENGLUVDEMO_IMAGEDEF_H
#define OPENGLUVDEMO_IMAGEDEF_H

#include <malloc.h>
#include <string.h>
#include <unistd.h>
#include "stdio.h"
#include "sys/stat.h"
#include "stdint.h"
#include "LogUtil.h"

#define IMAGE_FORMAT_RGBA           0x01
#define IMAGE_FORMAT_NV21           0x02
#define IMAGE_FORMAT_NV12           0x03
#define IMAGE_FORMAT_I420           0x04
#define IMAGE_FORMAT_YUYV           0x05
#define IMAGE_FORMAT_GRAY           0x06
#define IMAGE_FORMAT_I444           0x07
#define IMAGE_FORMAT_P010           0x08
#define IMAGE_FORMAT_GRAY10         0x09

struct NativeImage
{
    int width;
    int height;
    int format;
    uint8_t *ppPlane[3];

    NativeImage()
    {
        width = 0;
        height = 0;
        format = 0;
        ppPlane[0] = nullptr;
        ppPlane[1] = nullptr;
        ppPlane[2] = nullptr;
    }
};

class NativeImageUtil {
public:
    static void AllocNativeImage(NativeImage *pImage) {
        if (pImage->height == 0 || pImage->width == 0) {
            return;
        }

        switch (pImage->format) {
            case IMAGE_FORMAT_RGBA:
            {
                pImage->ppPlane[0] = static_cast<uint8_t *>(malloc(pImage->width * pImage->height * 4));
            }
                break;
            case IMAGE_FORMAT_NV12:
            case IMAGE_FORMAT_NV21:
            {
                pImage->ppPlane[0] = static_cast<uint8_t *>(malloc(pImage->width * pImage->height * 1.5));
                pImage->ppPlane[1] = pImage->ppPlane[0] + pImage->width * pImage->height;
            }
            default:
                LOGE("AllocNativeImage: format not support");
                break;
        }
    }

    static void CopyNativeImage(NativeImage *pSrcImage, NativeImage *pDstImage) {
        if (pSrcImage == nullptr || pSrcImage->ppPlane[0] == nullptr) {
            return;
        }

        if (pSrcImage->format != pDstImage->format ||
            pSrcImage->width != pDstImage->width ||
            pSrcImage->height != pDstImage->height) {
            return;
        }

        if (pDstImage->ppPlane[0] == nullptr)  {
            AllocNativeImage(pDstImage);
        }

        switch (pSrcImage->format) {
            case IMAGE_FORMAT_RGBA:
            {
                memcpy(pDstImage->ppPlane[0], pSrcImage->ppPlane[0], pSrcImage->width * pSrcImage->height * 4);
            }
                break;
            case IMAGE_FORMAT_NV12:
            case IMAGE_FORMAT_NV21:
            {
                memcpy(pDstImage->ppPlane[0], pSrcImage->ppPlane[0], pSrcImage->width * pSrcImage->height * 1.5);
            }
            default:
                LOGE("CopyNativeImage: format not support");
                break;
        }

    }

    static void FreeNativeImage(NativeImage *pImage)
    {
        if (pImage == nullptr || pImage->ppPlane[0] == nullptr) return;

        free(pImage->ppPlane[0]);
        pImage->ppPlane[0] = nullptr;
        pImage->ppPlane[1] = nullptr;
        pImage->ppPlane[2] = nullptr;
    }
};

#endif //OPENGLUVDEMO_IMAGEDEF_H
