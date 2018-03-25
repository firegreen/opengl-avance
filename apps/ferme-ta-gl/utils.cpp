#include "utils.hpp"

#include <glmlv/load_obj.hpp>
#include <glmlv/Image2DRGBA.hpp>

#include <glmlv/simple_geometry.hpp>
#include <GL/glu.h>
#include <iostream>

const glm::vec3 computeDirectionVectorUp(glm::vec3 forward)
{
	return glm::normalize(glm::cross(glm::cross(forward, glm::vec3(0, 1, 0)), forward));
}

void checkGlError()
{
	GLenum error = glGetError();
	while (error != GL_NO_ERROR)
	{
		std::string msg;
		switch (error) {
		case GL_INVALID_OPERATION:      msg = "INVALID_OPERATION";      break;
		case GL_INVALID_ENUM:           msg = "INVALID_ENUM";           break;
		case GL_INVALID_VALUE:          msg = "INVALID_VALUE";          break;
		case GL_OUT_OF_MEMORY:          msg = "OUT_OF_MEMORY";          break;
		case GL_INVALID_FRAMEBUFFER_OPERATION:  msg = "INVALID_FRAMEBUFFER_OPERATION";  break;
		}
		std::cerr << "Error has occured : " << error << " (" << msg << ", "
			<< gluErrorString(error) << ")" << std::endl;

		error = glGetError();
	}
}

void loadTexture(const glmlv::Image2DRGBA& image, GLuint& textureID)
{
	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB32F, image.width(), image.height());
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, image.width(), image.height(),
		GL_RGBA, GL_UNSIGNED_BYTE, image.data());
	glBindTexture(GL_TEXTURE_2D, 0);
	checkGlError();
}

void reserveImage(const size_t width, const size_t height, GLuint& textureID, GLenum format)
{
	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexStorage2D(GL_TEXTURE_2D, 1, format, width, height);
	glBindTexture(GL_TEXTURE_2D, 0);
	checkGlError();
}
