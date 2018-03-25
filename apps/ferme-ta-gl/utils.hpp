#pragma once

#include <glm/glm.hpp>
#include <glmlv/load_obj.hpp>
#include <glmlv/Image2DRGBA.hpp>
#include <glad/glad.h>

// Allocate GPU memory to store an image
void reserveImage(const size_t width, const size_t height, GLuint &textureID, GLenum format = GL_RGB32F);
// Load image data from loaded file
void loadTexture(const glmlv::Image2DRGBA &image, GLuint &textureID);

/*
	Returns a normaized up vector from a given foward one
*/
const glm::vec3 computeDirectionVectorUp(glm::vec3 forward);

void checkGlError();