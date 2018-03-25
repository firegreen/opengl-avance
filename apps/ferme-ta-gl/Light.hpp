#pragma once
#include "utils.hpp"

#include <glmlv/GLProgram.hpp>
#include <glm/matrix.hpp>


// Point Light data
struct PointLight
{
	glm::vec4 color{ 0,0,0,0 };
	glm::vec4 position{ 0,0,0,0 };

	PointLight() {}
	PointLight(glm::vec4 color, glm::vec4 position)
		: color(color), position(position) {}
};

// Directional light
struct DirectionnalLight
{
	// Directional light data to send to shader
	struct Data
	{
		glm::vec4 color{ 0,0,0,0 };
		glm::vec4 direction{ 1,0,0,1 };
		// data to send to the shader
		glm::mat4 lightMatrix;

		Data() {}
		Data(glm::vec4 color, glm::vec4 direction)
			: color(color), direction(direction) {}
	} data;

	bool isDirty = true;
	int shadowMapId = -1;

	DirectionnalLight() {}
	DirectionnalLight(glm::vec4 color, glm::vec4 direction)
		: data(color, direction) {}

	void computeMatrix(glm::vec3 sceneCenter, float sceneDiag);
	void update(glm::vec3 sceneCenter, float sceneDiag);
};

// Shadow map data for directional lights
struct DirectionnalLightShadowMap
{
	static const unsigned int DIR_MAX_SHADOW_COUNT;
	static const int DIR_SHADOW_TEXTURE_UNIT_OFFSET;
	static float shadowMapBias;
	static int32_t resolution;
	static GLuint* textures;
	static GLuint textureCount;
	static GLuint sampler;

	GLuint layer = 0; 
	GLuint FBO = 0;

	bool isDirty = true;


	GLuint shadowMapLayer = 0; // shadow map i

	DirectionnalLightShadowMap() {}
	void initialiaze(glmlv::GLProgram& shadingProgram, GLuint uShadowLightTexture);
};
