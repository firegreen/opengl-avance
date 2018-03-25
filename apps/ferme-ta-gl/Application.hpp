#pragma once

#include "utils.hpp"
#include "Light.hpp"
#include "Camera.hpp"

#include <glmlv/filesystem.hpp>
#include <glmlv/GLFWHandle.hpp>
#include <glmlv/GLProgram.hpp>
#include <glmlv/simple_geometry.hpp>
#include <glmlv/ViewController.hpp>
#include <glmlv/load_obj.hpp>
#include <glmlv/Image2DRGBA.hpp>
#include "Scene.hpp"

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

	enum BindingId
	{
		dirlightBindingId = 1,
		pointlightBindingId = 2,
		shadowBindingId = 3,
		shadowTexturesBindingId = 4
	};

	enum InputLocation
	{
		positionAttrLocation = 0,
		normalAttrLocation = 1,
		texCoordsAttrLocation = 2
	};

    Application(int argc, char** argv);
    ~Application();

    int run();

	const GLenum m_GBufferTextureFormat[GBufferTextureCount] = { GL_RGB32F, GL_RGB32F, GL_RGB32F,
																 GL_RGB32F, GL_RGBA32F, GL_DEPTH24_STENCIL8 };

private:
    void loadImage(std::string filename, GLuint &textureID);
	void loadSkybox(std::string foldername, std::string ext, GLuint &textureID);
	void initialiseLights();
	void initialiseBuffer();
	void initialiseScreen();
	void initialiseSamplerObjects();

	void resetLights(int lightsCount = 3);

	void solidRenderTo(GLuint FBO, size_t x, size_t y, size_t width, size_t height);
	void solidRender();
	void materialRenderTo(GLuint FBO, size_t x, size_t y, size_t width, size_t height);
	void materialRender();
	void shadowRender(DirectionnalLight* shadowPtr);
	void shadowViewUpdate(DirectionnalLight* shadowPtr);
	void shadingRenderTo(GLuint FBO, size_t x, size_t y, size_t width, size_t height);
	void shadingRender();

	const static size_t windowWidth;
	const static size_t windowHeight;
	glmlv::GLFWHandle m_GLFWHandle{ windowWidth, windowHeight, "Ferme Ta GL" };

    const glmlv::fs::path m_AppPath;
    const std::string m_AppName;
    const std::string m_ImGuiIniFilename;
    const glmlv::fs::path m_ShadersRootPath;
    const glmlv::fs::path m_AssetsRootPath;

	GLuint samplerObject;
	GLuint bufferSamplerObject;

	bool displayGUI = false;

	Camera camera;

    glmlv::GLProgram geometryProgram;
    glmlv::GLProgram shadingProgram;
    glmlv::GLProgram shadowProgram;
	glmlv::GLProgram depthProgram;
	glmlv::GLProgram skyboxProgram;

    GLuint FBO = 0;
	GLuint shadingVBO = 0;
	GLuint shadingVAO = 0;

    GLuint dirLightSSBO = 0;
    GLuint pointLightSSBO = 0;
	GLuint shadowTexturesSSBO = 0;

    GLuint gBufferTextures[GBufferTextureCount];

	std::vector<ObjectModel*> models;
	std::vector<Scene> scenes;
	Scene* currentScene;

	float shadowMapBias = 0.01;
};
