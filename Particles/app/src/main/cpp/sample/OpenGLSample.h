//
// Created by lixiang on 2025/8/23.
//

#ifndef OPENGL_TRIANGLE_DEMO_TRIANGLESAMPLE_H
#define OPENGL_TRIANGLE_DEMO_TRIANGLESAMPLE_H

#include <GLES3/gl3.h>
#include "../util/LogUtil.h"
#include "../util/GLUtils.h"
#include "../util/ImageDef.h"
#include "../glm/detail/type_mat.hpp"
#include "../glm/detail/type_mat4x4.hpp"
#include <vector>
#include <map>

#define MAX_PARTICLES 500

struct Particle {
    GLfloat dx, dy, dz;
    GLfloat dxSpeed, dySpeed, dzSpeed;
    GLubyte r, g, b, a;
    GLfloat life;
    GLfloat cameraDistance;

    Particle() {
        dx = dy = dz = 0.0f;
        dxSpeed = dySpeed = dzSpeed = 1.0;
        r = static_cast<GLubyte>(1.0f);
        g = static_cast<GLubyte>(1.0f);
        b = static_cast<GLubyte>(1.0f);
        a = static_cast<GLubyte>(1.0f);
        life = 5.0f;
    }

    bool operator<(const Particle& that) const {
        // Sort in reverse order : far particles drawn first.
        return this->cameraDistance > that.cameraDistance;
    }
};

class OpenGLSample {
public:
    OpenGLSample();
    ~OpenGLSample();

    void Init();
    void Draw(int screenWidth, int screenHeight);

    void LoadImage(NativeImage *image);
    virtual void LoadMultiImageWithIndex(int index, NativeImage *pImage);

    virtual void Destroy();

    virtual void UpdateTransformMatrix(float rotateX, float rotateY, float scaleX, float scaleY);

    void UpdateMVPMatrix(glm::mat4 &mvpMatrix, int angleX, int angleY, float ratio);

    virtual void UpdateMatrix(glm::mat4 &mvpMatrix, glm::mat4 &modelMatrix, int angleXRotate, int angleYRotate, float scale, glm::vec3 transVec3, float ratio);

    int FindUnusedParticle();

    void SortParticles();

    int UpdateParticles();

    void GenerateNewParticle(Particle &particle);


private:
    // base
    GLuint m_ProgramObj;
    GLuint m_VertexShader;
    GLuint m_FragmentShader;

    GLuint m_TextureId;
    GLint m_SamplerLoc;
    GLint m_MVPMatLoc;

    GLuint m_VaoId;
    GLuint m_ParticlesVertexVboId;
    GLuint m_ParticlesPosVboId;
    GLuint m_ParticlesColorVboId;

    NativeImage m_RenderImage;
    glm::mat4 m_MVPMatrix;

    // particles relation
    Particle m_ParticlesContainer[MAX_PARTICLES];
    GLfloat* m_pParticlesPosData;
    GLubyte* m_pParticlesColorData;
    int m_LastUsedParticle;

    int m_AngleX;
    int m_AngleY;

    float m_ScaleX;
    float m_ScaleY;
};


#endif //OPENGLTEST_TRIANGLESAMPLE_H
