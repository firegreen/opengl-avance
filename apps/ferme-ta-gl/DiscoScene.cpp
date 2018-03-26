#include "DiscoScene.hpp"

DiscoScene::DiscoScene(std::vector<std::shared_ptr<ObjectModel>>& models)
	: Scene(models)
{
	// Init scene function
	fogColor = glm::vec3(0, 0, 0);

	std::shared_ptr<Object3D> ptr(models[1]->instance());
	objects.push_back(ptr);
	std::shared_ptr<Object3D> ptr2(models[0]->instance());
	objects.push_back(ptr2);

	for (int i = 0; i < 20; ++i) {
		std::shared_ptr<Object3D> ptr3(models[1]->instance());
		objects.push_back(ptr3);
		ptr3->translate(-25 + rand() % 50, -25 + rand() % 50, -380  - rand() % 200);
		ptr3->scale(0.003f * (float)(rand() % 100));
	}

	ptr->translate(0, 0, -20);
	ptr->scale(0.01);
}

void DiscoScene::welcome(Camera& camera)
{
	// ChangeScene function
	camera.FoV = 60;
	camera.zNear = sceneDiag * 0.001;
	camera.zFar = sceneDiag;
	camera.getProjectionMatrix(true);

	camera.rotationSpeed = glm::vec3(-0, 0.01, 0);
	camera.translationSpeed = glm::vec3(0, 0, -0.05);
}

void DiscoScene::update(float elapsedTime, Camera & cam)
{
	// Update function
	for (int i = 2; i < 22; ++i) {
		objects[i]->translate(0, 0, 8);
		objects[i]->rotate(0.04, 0.05, 0.06);
		if(i%2) objects[i]->rotate(-0.01, 0.03, 0.04);
	}
}
