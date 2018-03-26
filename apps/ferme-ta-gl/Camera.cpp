#include "Camera.hpp"
#include "utils.hpp"
#include "Application.hpp"

Camera::Camera()
	: frontVector(0,0,-1,0)
	, upVector(0,1,0,0)
	, leftVector(1,0,0,0)
	, position(0,0,0,1),
	  translationSpeed(0.5,2,2),
	  rotationSpeed(0,2,1)
	{}

void Camera::reset()
{
	modelMatrix = glm::mat4();
	getViewMatrix();
}

void Camera::update(float elapsedTime)
{
	position = position + glm::vec4(translationSpeed,0);
	modelMatrix = glm::rotate(modelMatrix, rotationSpeed.x*elapsedTime, glm::vec3(1,0,0));
	modelMatrix = glm::rotate(modelMatrix, rotationSpeed.y*elapsedTime, glm::vec3(0,1,0));
	modelMatrix = glm::rotate(modelMatrix, rotationSpeed.z*elapsedTime, glm::vec3(0,0,1));
}

const glm::mat4& Camera::getViewMatrix()
{
	glm::vec4 frontVector = modelMatrix * this->frontVector;
	glm::vec4 upVector = modelMatrix * this->upVector;

	viewM = glm::lookAt(glm::vec3(position), glm::vec3(position + frontVector), glm::vec3(upVector));
	return viewM;
}

const glm::mat4& Camera::getProjectionMatrix(bool update)
{
	if (projM == glm::mat4() || update)
	{
		projM = glm::perspective(
			glm::radians(FoV),
			Application::windowWidth / (float) Application::windowHeight,
			zNear,
			zFar
		);
	}
	return projM;
}

glm::mat4 Camera::getVPMatrix()
{
	getViewMatrix();
	getProjectionMatrix();
	return projM * viewM;
}

glm::mat4 Camera::getRcpViewMatrix() const
{
	return glm::inverse(viewM);
}
