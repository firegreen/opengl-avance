#include "Scene.hpp"

ObjectModel* Scene::skyModel = nullptr;

Scene::Scene()
{
	if (Scene::skyModel == nullptr)
	{
		glmlv::SimpleGeometry cube = glmlv::makeCube();
		skyModel = new ObjectModel(cube.vertexBuffer, cube.indexBuffer);
	}
	skyCube.reset(skyModel->instance());
}

void Scene::resetLights(int lightsCount)
{
	dirLightData.clear();
	pointLightData.clear();
	shadowTextureData.clear();
	dirLights.push_back(DirectionnalLight(glm::vec4(0.2 + (float)(rand() % 140) * 0.01,
												   0.2 + (float)(rand() % 140) * 0.01,
												   0.2 + (float)(rand() % 140) * 0.01,1),
										 glm::vec4( glm::normalize(glm::vec3(-50 + rand() % 100, -50, -50 + rand() % 100)),0)));
	dirLights.back().castShadow(true);
	dirLightData.push_back(dirLights.back().data);
	shadowTextureData.push_back(dirLights.back().shadowMap.get()->layerID);
	for (int i = 0; i < lightsCount; ++i)
	{
		if (rand()%2)
		{
			dirLights.push_back(DirectionnalLight(glm::vec4(0.2 + (float)(rand() % 140) * 0.01,
														   0.2 + (float)(rand() % 140) * 0.01,
														   0.2 + (float)(rand() % 140) * 0.01,1),
												 glm::vec4( glm::normalize(glm::vec3(-50 + rand() % 100, -50, -50 + rand() % 100)),0)));
			dirLights.back().castShadow(true);
			dirLightData.push_back(dirLights.back().data);
			shadowTextureData.push_back(dirLights.back().shadowMap.get()->layerID);
		}
	}
	pointLightData.push_back(PointLight(glm::vec4(0.2 + (float)(rand() % 140) * 0.01,
												   0.2 + (float)(rand() % 140) * 0.01,
												   0.2 + (float)(rand() % 140) * 0.01, 1),
										glm::vec4(-10 + rand() % 40 * 0.5, -10 + rand() % 40 * 0.5, -10 + rand() % 40 * 0.5,1)));
	for (int i = 0; i < lightsCount; ++i)
	{
		if (rand()%2)
			pointLightData.push_back(PointLight(glm::vec4(0.2 + (float)(rand() % 140) * 0.01,
														   0.2 + (float)(rand() % 140) * 0.01,
														   0.2 + (float)(rand() % 140) * 0.01, 1),
												glm::vec4(-10 + rand() % 40 * 0.5, -10 + rand() % 40 * 0.5, -10 + rand() % 40 * 0.5,1)));
	}
}
