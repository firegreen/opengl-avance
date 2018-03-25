#pragma once

#include "utils.hpp"

#include <vector>
#include <glmlv/load_obj.hpp>
#include <glmlv/filesystem.hpp>

class Object3D;

class ObjectLoader
{
	GLuint VBO = 0;
	GLuint VAO = 0;
	GLuint IBO = 0;

	std::shared_ptr<glmlv::ObjData> data;
	ObjectLoader(const fs::path & objPath, const fs::path & mtlBaseDir, ObjData & data, bool loadTextures = true);
	Object3D* instance();
};


class Object3D
{
public:

	GLuint VBO = 0;
	GLuint VAO = 0;
	GLuint IBO = 0;

	std::shared_ptr<glmlv::ObjData> data;
	glm::mat4 transformM;

	void scale(float t);
	void scale(float x, float y, float z);
	void translate(float dx, float dy, float dz);
	void rotate(float rotationX, float rotationY, float rotationZ);

	virtual void draw() const;
};