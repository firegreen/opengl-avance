#pragma once

#include "utils.hpp"
#include "Light.hpp"

#include <glmlv/filesystem.hpp>
#include <glmlv/GLFWHandle.hpp>
#include <glmlv/GLProgram.hpp>
#include <glmlv/simple_geometry.hpp>
#include <glmlv/ViewController.hpp>
#include <glmlv/load_obj.hpp>
#include <glmlv/Image2DRGBA.hpp>


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

	const GLenum m_GBufferTextureFormat[GBufferTextureCount] = { GL_RGB32F, GL_RGB32F, GL_RGB32F,
																 GL_RGB32F, GL_RGBA32F, GL_DEPTH24_STENCIL8 };

private:
    void loadImage(std::string filename, GLuint &textureID);
	void loadSkybox(std::string foldername, std::string ext, GLuint &textureID);

	const size_t m_nWindowWidth = 1200;
	const size_t m_nWindowHeight = 700;
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
	glmlv::GLProgram depthProgram;
	glmlv::GLProgram skyboxProgram;


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

	GLuint skyboxVBO = 0;
	GLuint skyboxVAO = 0;
	GLuint skyboxIBO = 0;

    GLuint FBO = 0;

    std::vector<DirectionnalLight> dirLightData;
    std::vector<DirectionnalLightShadowMap> dirLightShadows;
    std::vector<PointLight> pointLightData;
    GLuint dirLightSSBO = 0;
    GLuint pointLightSSBO = 0;
	GLuint shadowSSBO = 0;
	GLuint shadowTexturesSSBO = 0;

    GLuint uModelViewProjMatrix = 0;
    GLuint uModelViewMatrix = 0;
    GLuint uNormalMatrix = 0;

	GLuint uSkyProjMatrix = 0;
	GLuint uSkyMatrix = 0;

    GLuint metalTexture = 0;
    GLuint woodTexture = 0;
	GLuint skyboxTexture = 0;
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

	GLuint uSkyboxSampler;

    GLuint uGPosition;
    GLuint uGNormal;
    GLuint uGAmbient;
    GLuint uGDiffuse;
    GLuint uGlossyShininess;
	GLuint uGDepth;
	GLuint uGShadingDepth;
	GLuint samplerBuffer;

    GLuint uCastShadow;

    GLuint uDirLightShadowMap;

    GLuint uDirLightViewProjMatrixShadow;
	GLuint uShadowMapBias;

	GLuint uShadowLightMap;

	GLuint uFogColor;
	GLuint uFogDistance;
	GLuint uFogDensity;

    GLuint bDirLightData;
    GLuint bPointLightData;

    glmlv::SimpleGeometry cube;
	glmlv::SimpleGeometry skyCube;
    glmlv::SimpleGeometry sphere;
    glmlv::ObjData loadedScene;

    GLuint gBufferTextures[GBufferTextureCount];
    bool displayBuffer[GBufferTextureCount];
};
