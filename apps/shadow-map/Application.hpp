#pragma once

#include <glmlv/filesystem.hpp>
#include <glmlv/GLFWHandle.hpp>
#include <glmlv/GLProgram.hpp>
#include <glmlv/simple_geometry.hpp>
#include <glmlv/ViewController.hpp>
#include <glmlv/load_obj.hpp>
#include <glmlv/Image2DRGBA.hpp>

#define MAXLIGHTCOUNT 32

void reserve3DImage(const size_t width, const size_t height, const size_t nbLayer, GLuint &textureID, GLenum format = GL_RGB32F);
void reserveImage(const size_t width, const size_t height, GLuint &textureID, GLenum format = GL_RGB32F);
void loadTexture(const glmlv::Image2DRGBA &image, GLuint &textureID);

const glm::vec3 computeDirectionVectorUp(float phiRadians, float thetaRadians);

struct PointLight
{
    glm::vec4 color{0,0,0,0};
    glm::vec4 position{0,0,0,0};

    PointLight() {}
    PointLight(glm::vec4 color, glm::vec4 position)
        : color(color), position(position){}
};

struct DirectionnalLight
{
    glm::vec4 color{0,0,0,0};
    glm::vec4 direction{1,0,0,1};

    DirectionnalLight() {}
    DirectionnalLight(glm::vec4 color, glm::vec4 direction)
        : color(color), direction(direction){}
};

struct DirectionnalLightShadowData
{
    static const int DIR_SHADOW_TEXTURE_UNIT_OFFSET;
    glm::mat4 lightViewProjMatrix;
    GLuint shadowMapLayer = 0;
    GLuint shadowMap = 0;
    float shadowMapBias = 0.0f;

    DirectionnalLightShadowData() {}
};

struct DirectionnalLightShadow
{
    static const int DIR_MAX_SHADOW_COUNT;
    static GLuint textures;
    static GLuint textureCount;
    static GLuint sampler;

    GLuint layer = 0;
    GLuint FBO = 0;
    int32_t resolution = 512;
    bool isDirty = true;

    DirectionnalLightShadowData data;

    DirectionnalLightShadow() {}
    void initialiaze()
    {
        if (!textures)
        {
            reserve3DImage(resolution, resolution, DIR_MAX_SHADOW_COUNT, textures, GL_DEPTH_COMPONENT32F);
            textureCount = 0;
        }

        layer = textureCount;
        ++textureCount;

        glGenFramebuffers(1, &FBO);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, FBO);

        glFramebufferTextureLayer( GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, textures, 0, layer);

        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

        GLenum error = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);
        if (error != GL_FRAMEBUFFER_COMPLETE)
        {
            std::cerr << "Aie aie aie gro, frame buffer status returns error: " << error << std::endl;
        }

        if (!sampler)
        {
            glGenSamplers(1, &sampler);
            glSamplerParameteri(sampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glSamplerParameteri(sampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glSamplerParameteri(sampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
            glSamplerParameteri(sampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        }

        data.shadowMapLayer = layer;
    }
};


/*
struct ShaderLightsData
{
    uint pointLightCount = 0;
    uint directionnalLightCount = 0;

    PointLight pointLights[MAXLIGHTCOUNT];
    DirectionnalLight dirLights[MAXLIGHTCOUNT];
};*/

class Application
{
public:
    enum GBufferTextureType
    {
        GPosition = 0,
        GNormal,
        GAmbient,
        GDiffuse,
        GGlossyShininess,
        GDepth, // On doit créer une texture de depth mais on écrit pas directement dedans dans le FS. OpenGL le fait pour nous (et l'utilise).
        GBufferTextureCount
    };

    Application(int argc, char** argv);
    ~Application();

    int run();

    const GLenum m_GBufferTextureFormat[GBufferTextureCount] = { GL_RGB32F, GL_RGB32F, GL_RGB32F, GL_RGB32F, GL_RGBA32F, GL_DEPTH_COMPONENT32F };

private:
    void loadImage(std::string filename, GLuint &textureID);

    const size_t m_nWindowWidth = 1280;
    const size_t m_nWindowHeight = 720;
    glmlv::GLFWHandle m_GLFWHandle{ m_nWindowWidth, m_nWindowHeight, "Template" }; // Note: the handle must be declared before the creation of any object managing OpenGL resource (e.g. GLProgram, GLShader)

    const glmlv::fs::path m_AppPath;
    const std::string m_AppName;
    const std::string m_ImGuiIniFilename;
    const glmlv::fs::path m_ShadersRootPath;
    const glmlv::fs::path m_AssetsRootPath;

    glmlv::ViewController camera;

    glmlv::GLProgram geometryProgram;
    glmlv::GLProgram shadingProgram;
    glmlv::GLProgram shadowProgram;

    GLuint cubeVBO = 0;
    GLuint cubeVAO = 0;
    GLuint cubeIBO = 0;

    GLuint sphereVBO = 0;
    GLuint sphereVAO = 0;
    GLuint sphereIBO = 0;

    GLuint sceneVBO = 0;
    GLuint sceneVAO = 0;
    GLuint sceneIBO = 0;

    GLuint quadVBO = 0;
    GLuint quadVAO = 0;

    GLuint FBO = 0;

    std::vector<DirectionnalLight> dirLightData;
    std::vector<DirectionnalLightShadow> dirLightShadows;
    std::vector<PointLight> pointLightData;
    GLuint dirLightSSBO = 0;
    GLuint pointLightSSBO = 0;
    GLuint shadowSSBO = 0;

    GLuint uModelViewProjMatrix = 0;
    GLuint uModelViewMatrix = 0;
    GLuint uNormalMatrix = 0;

    GLuint metalTexture = 0;
    GLuint woodTexture = 0;
    std::vector<GLuint> sceneTextures;

    GLuint uDirectionalLightDir;
    GLuint uDirectionalLightIntensity;

    GLuint uPointLightPosition;
    GLuint uPointLightIntensity;

    GLuint uAmbiantLightIntensity;

    GLuint uKd;
    GLuint uKs;
    GLuint uKa;
    GLuint uShininess;

    GLuint uUseDTexture;
    GLuint uUseATexture;
    GLuint uUseSpecTexture;
    GLuint uUseShinTexture;
    GLuint uKdSampler;
    GLuint uKaSampler;
    GLuint uKspecSampler;
    GLuint uKshinSampler;
    GLuint uNormalSampler;
    GLuint samplerObject;

    GLuint uGPosition;
    GLuint uGNormal;
    GLuint uGAmbient;
    GLuint uGDiffuse;
    GLuint uGlossyShininess;

    GLuint uCastShadow;

    GLuint uDirLightShadowMap;

    GLuint uDirLightViewProjMatrixShadow;

    GLuint bDirLightData;
    GLuint bPointLightData;

    glmlv::SimpleGeometry cube;
    glmlv::SimpleGeometry sphere;
    glmlv::ObjData loadedScene;

    GLuint gBufferTextures[GBufferTextureCount];
    bool displayBuffer[GBufferTextureCount];
};