//
// Created by lixiang on 2025/8/30.
//

#include "EGLRender.h"
#include "../util/GLUtils.h"

#define VERTEX_POS_LOC  0
#define TEXTURE_POS_LOC 1

#define PARAM_TYPE_SHADER_INDEX    200


EGLRender *EGLRender::m_Instance = nullptr;

const char vShaderStr[] = R"(
    #version 300 es
    layout(location = 0) in vec4 aPosition;
    layout(location = 1) in vec2 aTexCoord;
    out vec2 vTexCoord;
    void main() {
        gl_Position = aPosition;
        vTexCoord = aTexCoord;
    }
)";

const char fShaderStr0[] = R"(
    #version 300 es
    precision mediump float;
    in vec2 vTexCoord;
    layout(location = 0) out vec4 outColor;
    uniform sampler2D sTextureMap;
    void main() {
        outColor = texture(sTextureMap, vTexCoord);
    }
)";
// mosaic
const char fShaderStr1[] = R"(
    #version 300 es
    precision highp float;
    layout(location = 0) out vec4 outColor;
    in vec2 vTexCoord;
    uniform sampler2D sTextureMap;
    uniform vec2 uTexSize;

    vec4 CrossStitching(vec2 uv) {
        float stitchSize = uTexSize.x / 35.0;
        int invert = 0;
        vec4 color = vec4(0.0);
        float size = stitchSize;
        vec2 cPos = uv * uTexSize;
        vec2 tlPos = floor(cPos / vec2(size, size));
        tlPos *= size;
        int remX = int(mod(cPos.x, size));
        int remY = int(mod(cPos.y, size));
        if (remX == 0 && remY == 0)
        tlPos = cPos;
        vec2 blPos = tlPos;
        blPos.y += (size - 1.0);
        if ((remX == remY) || (((int(cPos.x) - int(blPos.x)) == (int(blPos.y) - int(cPos.y))))) {
            if (invert == 1)
            color = vec4(0.2, 0.15, 0.05, 1.0);
            else
            color = texture(sTextureMap, tlPos * vec2(1.0 / uTexSize.x, 1.0 / uTexSize.y)) * 1.4;
        } else {
            if (invert == 1)
            color = texture(sTextureMap, tlPos * vec2(1.0 / uTexSize.x, 1.0/ uTexSize.y)) * 1.4;
            else
            color = vec4(0.0, 0.0, 0.0, 1.0);
        }
        return color;
    }

    void main() {
        outColor = CrossStitching(vTexCoord);
    }
)";

// vertex coordinate
const GLfloat vVertices[] = {
        -1.0f, -1.0f, 0.0f,
        1.0f, -1.0f, 0.0f,
        -1.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 0.0f
};

// texture coordinate
const GLfloat vTexCoords[] = {
        0.0f, 1.0f, // bottom left
        1.0f, 1.0f, // bottom right
        0.0f, 0.0f, // top left
        1.0f, 0.0f, // top right
};

// fbo
const GLfloat vFboTexCoords[] = {
        0.0f, 0.0f,  // bottom left
        1.0f, 0.0f,  // bottom right
        0.0f, 1.0f,  // top left
        1.0f, 1.0f,  // top right
};

const GLushort indices[] = {
        0, 1, 2,
        1, 3, 2
};

EGLRender::EGLRender() {
    m_ImageTextureId = GL_NONE;
    m_FboTextureId = GL_NONE;
    m_SamplerLoc = GL_NONE;
    m_TexSizeLoc = GL_NONE;
    m_FboId = GL_NONE;
    m_ProgramObj = GL_NONE;
    m_VertexShader = GL_NONE;
    m_FragmentShader = GL_NONE;

    m_IsGLContextReady = false;
    m_ShaderIndex = 0;
}
EGLRender::~EGLRender() {
    if (m_RenderImage.ppPlane[0]) {
        NativeImageUtil::FreeNativeImage(&m_RenderImage);
        m_RenderImage.ppPlane[0] = nullptr;
    }
}

void EGLRender::Init() {
    LOGI("EGLRender::Init");
    if (CreateGlesEnv() == 0) {
        m_IsGLContextReady = true;
    }

    if (!m_IsGLContextReady) {
        LOGE("EGLRender::Init: CreateGlesEnv failed");
        return;
    }

    m_fShaderStrs[0] = fShaderStr0;
    m_fShaderStrs[1] = fShaderStr1;
    m_fShaderStrs[2] = fShaderStr0;
    m_fShaderStrs[3] = fShaderStr0;
    m_fShaderStrs[4] = fShaderStr0;
    m_fShaderStrs[5] = fShaderStr0;
    m_fShaderStrs[6] = fShaderStr0;

    glGenTextures(1, &m_ImageTextureId);
    glBindTexture(GL_TEXTURE_2D, m_ImageTextureId);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, GL_NONE);

    glGenTextures(1, &m_FboTextureId);
    glBindTexture(GL_TEXTURE_2D, m_FboTextureId);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, GL_NONE);

    m_ProgramObj = GLUtils::CreateProgram(vShaderStr, m_fShaderStrs[m_ShaderIndex], m_VertexShader, m_FragmentShader);
    if (!m_ProgramObj) {
        GLUtils::CheckGLError("Create Program");
        LOGE("EGLRender::Init: Create Program failed");
        return;
    }

    m_SamplerLoc = glGetUniformLocation(m_ProgramObj, "sTextureMap");
    m_TexSizeLoc = glGetUniformLocation(m_ProgramObj, "uTexSize");

    // generate VBO ids and load the VBOs with data
    glGenBuffers(3, m_VboIds);
    glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vVertices), vVertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vTexCoords), vTexCoords, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[2]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    GO_CHECK_GL_ERROR();

    // generate VAO Ids
    glGenVertexArrays(1, m_VaoIds);

    // FBO off screen rendering VAO
    glBindVertexArray(m_VaoIds[0]);

    glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[0]);
    glEnableVertexAttribArray(VERTEX_POS_LOC);
    glVertexAttribPointer(VERTEX_POS_LOC, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), GL_NONE);
    glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

    glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[1]);
    glEnableVertexAttribArray(TEXTURE_POS_LOC);
    glVertexAttribPointer(TEXTURE_POS_LOC, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), GL_NONE);
    glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_VboIds[2]);

    GO_CHECK_GL_ERROR();

    glBindVertexArray(GL_NONE);
}

int EGLRender::CreateGlesEnv() {
    // EGL config attributes
    const EGLint confAttr[] = {
            EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT_KHR,
            EGL_SURFACE_TYPE, EGL_PBUFFER_BIT, // EGL_WINDOW_BIT EGL_PBUFFER_BIT we will create a pixelbuffer surface
            EGL_RED_SIZE,
            EGL_GREEN_SIZE,
            EGL_BLUE_SIZE,
            EGL_ALPHA_SIZE,
            EGL_DEPTH_SIZE,
            EGL_STENCIL_SIZE,
            EGL_NONE
    };
    // EGL context attributes
    const EGLint ctxAttr[] = {
            EGL_CONTEXT_CLIENT_VERSION, 2,
            EGL_NONE
    };
    // surface attributes, the surface size is set to the input frame size
    const EGLint surfAttr[] = {
            EGL_WIDTH, 1,
            EGL_HEIGHT, 1,
            EGL_NONE
    };
    EGLint eglMajvers, eglMinVers;
    EGLint numConfigs;

    int resultCode = 0;
    do {
        // 1. get the EGLDisplay object, which connect to local window system
        m_eglDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
        if (m_eglDisplay == EGL_NO_DISPLAY) {
            //unable to open connection to local windowing system
            LOGE("EGLRender::CreateGlesEnv: eglGetDisplay failed");
            resultCode = -1;
            break;
        }

        // 2. initialize EGL
        if (!eglInitialize(m_eglDisplay, &eglMajvers, &eglMinVers)) {
            LOGE("EGLRender::CreateGlesEnv: eglInitialize failed");
            resultCode = -1;
            break;
        }

        LOGI("EGLRender::CreateGlesEnv: eglInitialize int with version %d.%d", eglMajvers, eglMinVers);

        // 3. get EGLConfig object ,verify surface config info
        if (!eglChooseConfig(m_eglDisplay, confAttr, &m_eglConf, 1, &numConfigs)) {
            LOGE("EGLRender::CreateGlesEnv: eglChooseConfig failed");
            resultCode = -1;
            break;
        }

        // 4. get EGLSurface object , user eglCratePbufferSurface to create offscreen surface
        m_eglSurface = eglCreatePbufferSurface(m_eglDisplay, m_eglConf, surfAttr);
        if (m_eglSurface == EGL_NO_SURFACE) {
            switch (eglGetError()) {
                case EGL_BAD_ALLOC:
                    LOGE("EGLRender::CreateGlesEnv: eglCreatePbufferSurface failed with EGL_BAD_ALLOC");
                    break;
                case EGL_BAD_CONFIG:
                    LOGE("EGLRender::CreateGlesEnv: eglCreatePbufferSurface failed with EGL_BAD_CONFIG");
                    break;
                case EGL_BAD_PARAMETER:
                    LOGE("EGLRender::CreateGlesEnv: eglCreatePbufferSurface failed with EGL_BAD_PARAMETER");
                    break;
                case EGL_BAD_MATCH:
                    LOGE("EGLRender::CreateGlesEnv: eglCreatePbufferSurface failed with EGL_BAD_MATCH");
                    break;
            }
        }

        // 5. create render context EGLContext,
        m_eglCtx = eglCreateContext(m_eglDisplay, m_eglConf, EGL_NO_CONTEXT, ctxAttr);
        if (m_eglCtx == EGL_NO_CONTEXT) {
            EGLint error = eglGetError();
            if (error == EGL_BAD_CONFIG) {
                LOGE("EGLRender::CreateGlesEnv: eglCreateContext failed with EGL_BAD_CONFIG");
                resultCode = -1;
                break;
            }
        }
        // 6. bind the context to the surface
        if (!eglMakeCurrent(m_eglDisplay, m_eglSurface, m_eglSurface, m_eglCtx)) {
            LOGE("EGLRender::CreateGlesEnv: eglMakeCurrent failed");
            resultCode = -1;
            break;
        }
        LOGI("EGLRender::CreateGlesEnv: eglMakeCurrent success");
    } while (false);

    if (resultCode != 0) {
        LOGI("EGLRender::CreateGlesEnv: CreateGlesEnv failed");
    }

    return resultCode;
}

void EGLRender::SetImageData(uint8_t *pData, int width, int height) {
    LOGI("EGLRender::SetImageData: pData = %p , width = %d, height = %d", pData, width, height);
    if (!pData || !m_IsGLContextReady) {
        LOGE("EGLRender::SetImageData: invalid params");
        return;
    }

    if (m_RenderImage.ppPlane[0]){
        NativeImageUtil::FreeNativeImage(&m_RenderImage);
        m_RenderImage.ppPlane[0] = nullptr;
    }

    m_RenderImage.width = width;
    m_RenderImage.height = height;
    m_RenderImage.format = IMAGE_FORMAT_RGBA;
    NativeImageUtil::AllocNativeImage(&m_RenderImage);
    memcpy(m_RenderImage.ppPlane[0], pData, static_cast<size_t>(width * height * 4));

    glBindTexture(GL_TEXTURE_2D, m_ImageTextureId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_RenderImage.width,
                 m_RenderImage.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_RenderImage.ppPlane[0]);
    glBindTexture(GL_TEXTURE_2D, GL_NONE);

    if( m_FboId == GL_NONE) {
        // create FBO
        glGenFramebuffers(1, &m_FboId);
        glBindFramebuffer(GL_FRAMEBUFFER, m_FboId);
        glBindTexture(GL_TEXTURE_2D, m_FboTextureId);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_FboTextureId, 0);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_RenderImage.width, m_RenderImage.height, 0, GL_RGBA, GL_UNSIGNED_BYTE,
                     nullptr);
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            LOGE("EGLRender::SetImageData: glCheckFramebufferStatus failed, FrameBufferStatus = %d", glCheckFramebufferStatus(GL_FRAMEBUFFER));
        }
        glBindTexture(GL_TEXTURE_2D, GL_NONE);
        glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE);
    }
}

void EGLRender::SetIntParams(int paramType, int param) {
    LOGI("EGLRender::SetIntParams: paramType = %d, param = %d", paramType, param);
    switch (paramType) {
        case PARAM_TYPE_SHADER_INDEX:
            if (param < 0 ) {
                break;
            }
            m_ShaderIndex = param % EGL_FEATURE_NUM;
            if(m_ProgramObj) {
                glDeleteProgram(m_ProgramObj);
                m_ProgramObj = GL_NONE;
            }

            m_ProgramObj = GLUtils::CreateProgram(vShaderStr, m_fShaderStrs[m_ShaderIndex], m_VertexShader, m_FragmentShader);
            if (!m_ProgramObj) {
                GLUtils::CheckGLError("Create Program");
                LOGE("EGLRender::SetIntParams: CreateProgram failed");
                return;
            }

            m_SamplerLoc = glGetUniformLocation(m_ProgramObj, "sTextureMap");
            GO_CHECK_GL_ERROR();
            m_TexSizeLoc = glGetUniformLocation(m_ProgramObj, "uTexSize");
            GO_CHECK_GL_ERROR();
            break;
        default:
            break;
    }
}

void EGLRender::Draw() {
    LOGI("EGLRender::Draw");
    if (m_ProgramObj == GL_NONE) {
        LOGE("EGLRender::Draw: m_ProgramObj is not created");
        return;
    }

    glViewport(0, 0, m_RenderImage.width, m_RenderImage.height);

    // do fbo off screen rendering
    glUseProgram(m_ProgramObj);
    glBindFramebuffer(GL_FRAMEBUFFER, m_FboId);

    glBindVertexArray(m_VaoIds[0]);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_ImageTextureId);
    glUniform1i(m_SamplerLoc, 0);

    if (m_TexSizeLoc > -1) {
        GLfloat size[2];
        size[0] = m_RenderImage.width;
        size[1] = m_RenderImage.height;
        glUniform2f(m_TexSizeLoc, size[0], size[1]);
    }

    // render
    GO_CHECK_GL_ERROR();
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, (const void *)0);
    GO_CHECK_GL_ERROR();
    glBindVertexArray(GL_NONE);
    glBindTexture(GL_TEXTURE_2D, GL_NONE);
}

void EGLRender::DestroyGlesEnv() {}

void EGLRender::UnInit() {}
