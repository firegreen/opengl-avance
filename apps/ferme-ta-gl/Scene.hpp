#pragma once
#include "utils.hpp"
#include "Light.hpp"
#include "Object3D.hpp"

#include <glmlv/GLProgram.hpp>
#include <glmlv/simple_geometry.hpp>
#include <glm/matrix.hpp>



// Shadow map data for directional lights
struct Scene
{
	static ObjectModel* skyModel;

	std::vector<DirectionnalLight::Data> dirLightData;
	std::vector<DirectionnalLight> dirLights;
	std::vector<uint32_t> shadowTextureData;
	std::vector<PointLight> pointLightData;

	glm::vec3 ambiantLight = glm::vec3(0.2f, 0.2f, 0.2f);

	glm::vec3 fogColor = glm::vec3(1.f, 1.f, 1.f);
	float fogDistance = 1.f;
	float fogDensity = 0.0002f;



	glm::vec3 sceneCenter;
	float sceneDiag = 2000;

	std::vector<std::shared_ptr<Object3D>> objects;

	GLuint skyboxTexture = 0;
	std::shared_ptr<Object3D> skyCube;

	Scene();
	void resetLights(int lightsCount = 3);
};
