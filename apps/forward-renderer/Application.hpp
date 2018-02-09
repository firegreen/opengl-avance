#pragma once

#include <glmlv/filesystem.hpp>
#include <glmlv/GLFWHandle.hpp>
#include <glmlv/GLProgram.hpp>
#include <glmlv/simple_geometry.hpp>
#include <glmlv/ViewController.hpp>
#include <glmlv/load_obj.hpp>
#include <glmlv/Image2DRGBA.hpp>

#define MAXLIGHTCOUNT 32

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
    Application(int argc, char** argv);
    ~Application();

    int run();

private:
    void loadImage(std::string filename, GLuint &textureID);
    void loadImage(const glmlv::Image2DRGBA &image, GLuint &textureID);

    const size_t m_nWindowWidth = 1280;
    const size_t m_nWindowHeight = 720;
    glmlv::GLFWHandle m_GLFWHandle{ m_nWindowWidth, m_nWindowHeight, "Template" }; // Note: the handle must be declared before the creation of any object managing OpenGL resource (e.g. GLProgram, GLShader)

    const glmlv::fs::path m_AppPath;
    const std::string m_AppName;
    const std::string m_ImGuiIniFilename;
    const glmlv::fs::path m_ShadersRootPath;
    const glmlv::fs::path m_AssetsRootPath;

    glmlv::ViewController camera;

    glmlv::GLProgram program;

    GLuint cubeVBO = 0;
    GLuint cubeVAO = 0;
    GLuint cubeIBO = 0;

    GLuint sphereVBO = 0;
    GLuint sphereVAO = 0;
    GLuint sphereIBO = 0;

    GLuint sceneVBO = 0;
    GLuint sceneVAO = 0;
    GLuint sceneIBO = 0;

    std::vector<DirectionnalLight> dirLightData;
    std::vector<PointLight> pointLightData;
    GLuint dirLightSSBO = 0;
    GLuint pointLightSSBO = 0;

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

    GLuint uUseTexture;
    GLuint uKdSampler;
    GLuint uKaSampler;
    GLuint uKsSampler;
    GLuint uNormalSampler;
    GLuint samplerObject;

    GLuint bDirLightData;
    GLuint bPointLightData;

    glmlv::SimpleGeometry cube;
    glmlv::SimpleGeometry sphere;
    glmlv::ObjData loadedScene;
};
