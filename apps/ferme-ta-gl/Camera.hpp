#pragma once
#include "utils.hpp"

#include <glmlv/GLProgram.hpp>
#include <glm/matrix.hpp>

// Directional light
struct Camera
{
public:
	Camera();

	glm::mat4 modelMatrix;
	glm::vec3 translationSpeed;
	glm::vec3 rotationSpeed;

	float FoV = 60.0f;
	float zFar = 1000.0f;
	float zNear = 0.1f;

	void reset();
	void update(float elapsedTime);

	const glm::mat4& getViewMatrix();
	const glm::mat4& getProjectionMatrix(bool update = false);
	glm::mat4 getVPMatrix();
	glm::mat4 getRcpViewMatrix() const;

private:
	glm::vec4 frontVector;
	glm::vec4 leftVector;
	glm::vec4 upVector;
	glm::vec4 position;
	glm::mat4 viewM;
	glm::mat4 projM;
};
