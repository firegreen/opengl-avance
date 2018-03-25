#include "Camera.hpp"
#include "utils.hpp"
#include "Application.hpp"

Camera::Camera()
	: frontVector(0,0,-1), upVector(0,1,0), leftVector(1,0,0), position(0,0,0)
	{}

void Camera::reset()
{
	modelMatrix = glm::mat4();
	getViewMatrix();
}

void Camera::update(float elapsedTime)
{
	modelMatrix = glm::translate(modelMatrix, translationSpeed*elapsedTime);
	modelMatrix = glm::rotate(modelMatrix, rotationSpeed.x*elapsedTime, vec3(1,0,0));
	modelMatrix = glm::rotate(modelMatrix, rotationSpeed.y*elapsedTime, vec3(0,1,0));
	modelMatrix = glm::rotate(modelMatrix, rotationSpeed.z*elapsedTime, vec3(0,0,1));
}

const glm::mat4& Camera::getViewMatrix()
{
	glm::vec3 position = modelMatrix * this->position;
	glm::vec3 frontVector = modelMatrix * this->frontVector;
	glm::vec3 upVector = modelMatrix * this->upVector;

	viewM = glm::lookAt(position, position + frontVector, upVector);
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

glm::mat4 Camera::getVPMatrix() const
{
	getViewMatrix();
	getProjectionMatrix();
	return projM * viewM;
}

glm::mat4 Camera::getRcpViewMatrix() const
{
	return glm::inverse(viewM);
}
