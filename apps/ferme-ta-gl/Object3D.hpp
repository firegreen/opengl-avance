#pragma once

#include "utils.hpp"

#include <vector>
#include <glmlv/load_obj.hpp>
#include <glmlv/filesystem.hpp>

class Object3D;

class ObjectModel
{	

public:
	ObjectModel(const glmlv::fs::path & objPath, bool loadTextures = true);
	ObjectModel(const std::vector<glmlv::Vertex3f3f2f>& vertex, const std::vector<uint32_t> &indices);
	~ObjectModel();
	Object3D* instance();

	static const GLint positionAttrLocation;
	static const GLint normalAttrLocation;
	static const GLint texCoordsAttrLocation;
private:
	GLuint VBO = 0;
	GLuint VAO = 0;
	GLuint IBO = 0;
	glmlv::ObjData data;
	std::vector<GLuint> textures;
};


class Object3D
{
public:

	Object3D(GLuint VBO, GLuint VAO, GLuint IBO, glmlv::ObjData& data, std::vector<GLuint>& textures);

	void scale(float t);
	void scale(float x, float y, float z);
	void translate(float dx, float dy, float dz);
	void rotate(float rotationX, float rotationY, float rotationZ);

	const GLuint VBO = 0;
	const GLuint VAO = 0;
	const GLuint IBO = 0;

	glmlv::ObjData* data;
	std::vector<GLuint>* textures;
	glm::mat4 modelMatrix;
};
