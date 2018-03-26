#include "DiscoScene.hpp"

DiscoScene::DiscoScene(std::vector<std::shared_ptr<ObjectModel>>& models)
	: Scene(models)
{
	// Init scene function

	std::shared_ptr<Object3D> ptr(models[0]->instance());
	objects.push_back(ptr);

	ptr->translate(0, 0, -200);
	ptr->scale(0.1);
}

void DiscoScene::welcome(Camera& camera)
{
	// ChangeScene function
	camera.FoV = 60;
	camera.zNear = sceneDiag * 0.001;
	camera.zFar = sceneDiag;
	camera.getProjectionMatrix(true);

	camera.rotationSpeed = glm::vec3(0, 0, 0);
	camera.translationSpeed = glm::vec3(0, 0, -0.05);
}

void DiscoScene::update(float elapsedTime, Camera & cam)
{
	// Update function
}
