#include "Object3D.hpp"
#include "utils.hpp"

const GLint ObjectModel::positionAttrLocation = 0;//glGetAttribLocation(program.glId(), "aPosition");
const GLint ObjectModel::normalAttrLocation = 1;//glGetAttribLocation(program.glId(), "aNormal");
const GLint ObjectModel::texCoordsAttrLocation = 2;//glGetAttribLocation(program.glId(), "aTexCoords");

ObjectModel::ObjectModel(const glmlv::fs::path &objPath, bool loadTextures)
{
	glmlv::loadObj(objPath, data, loadTextures);
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferStorage(GL_ARRAY_BUFFER, data.vertexBuffer.size()*sizeof(glmlv::Vertex3f3f2f),
					data.vertexBuffer.data(), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// IBO Init (indice Buffer)
	glGenBuffers(1, &IBO);
	glBindBuffer(GL_ARRAY_BUFFER, IBO);
	glBufferStorage(GL_ARRAY_BUFFER, sizeof(uint32_t)*data.indexBuffer.size(), data.indexBuffer.data(), 0);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glEnableVertexAttribArray(positionAttrLocation);
	glVertexAttribPointer(positionAttrLocation, 3, GL_FLOAT, GL_FALSE, sizeof(glmlv::Vertex3f3f2f),
						  (const GLvoid*) offsetof(glmlv::Vertex3f3f2f, position));

	glEnableVertexAttribArray(normalAttrLocation);
	glVertexAttribPointer(normalAttrLocation, 3, GL_FLOAT, GL_FALSE, sizeof(glmlv::Vertex3f3f2f),
						  (const GLvoid*) offsetof(glmlv::Vertex3f3f2f, normal));

	glEnableVertexAttribArray(texCoordsAttrLocation);
	glVertexAttribPointer(texCoordsAttrLocation, 2, GL_FLOAT, GL_FALSE, sizeof(glmlv::Vertex3f3f2f),
						  (const GLvoid*) offsetof(glmlv::Vertex3f3f2f, texCoords));

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	textures.resize(data.textures.size(),0);
	for (int i=0; i<data.textures.size(); ++i)
	{
		loadTexture(data.textures[i], textures[i]);
	}
}

ObjectModel::ObjectModel(const std::vector<glmlv::Vertex3f3f2f> &vertex, const std::vector<uint32_t>& indices)
{
	data.indexBuffer = indices;
	data.vertexBuffer = vertex;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferStorage(GL_ARRAY_BUFFER, vertex.size()*sizeof(glmlv::Vertex3f3f2f), vertex.data(), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// IBO Init (indice Buffer)
	glGenBuffers(1, &IBO);
	glBindBuffer(GL_ARRAY_BUFFER, IBO);
	glBufferStorage(GL_ARRAY_BUFFER, sizeof(uint32_t)*indices.size(), indices.data(), 0);


	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glEnableVertexAttribArray(positionAttrLocation);
	glVertexAttribPointer(positionAttrLocation, 3, GL_FLOAT, GL_FALSE, sizeof(glmlv::Vertex3f3f2f),
						  (const GLvoid*) offsetof(glmlv::Vertex3f3f2f, position));

	glEnableVertexAttribArray(normalAttrLocation);
	glVertexAttribPointer(normalAttrLocation, 3, GL_FLOAT, GL_FALSE, sizeof(glmlv::Vertex3f3f2f),
						  (const GLvoid*) offsetof(glmlv::Vertex3f3f2f, normal));

	glEnableVertexAttribArray(texCoordsAttrLocation);
	glVertexAttribPointer(texCoordsAttrLocation, 2, GL_FLOAT, GL_FALSE, sizeof(glmlv::Vertex3f3f2f),
						  (const GLvoid*) offsetof(glmlv::Vertex3f3f2f, texCoords));

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
}

ObjectModel::~ObjectModel()
{
	if (IBO) {
		glDeleteBuffers(1, &IBO);
	}
	if (VBO) {
		glDeleteBuffers(1, &VBO);
	}
	if (VAO) {
		glDeleteBuffers(1, &VAO);
	}
}

Object3D *ObjectModel::instance()
{
	return new Object3D(VBO, VAO, IBO, data, textures);
}

Object3D::Object3D(GLuint VBO, GLuint VAO, GLuint IBO, const glmlv::ObjData &data, const std::vector<GLuint>& textures)
	: VBO(VBO), VAO(VAO), IBO(IBO), data(data), textures(textures)
{}

void Object3D::scale(float t)
{
	modelMatrix = glm::scale(modelMatrix, glm::vec3(t,t,t));
}
void Object3D::scale(float x, float y, float z)
{
	modelMatrix = glm::scale(modelMatrix, glm::vec3(x,y,z));
}

void Object3D::translate(float dx, float dy, float dz)
{
	modelMatrix = glm::translate(modelMatrix, glm::vec3(dx,dy,dz));
}

void Object3D::rotate(float rx, float ry, float rz)
{
	modelMatrix =
			glm::rotate(
			glm::rotate(
			glm::rotate(modelMatrix,
				rx, glm::vec3(1,0,0)),
				ry, glm::vec3(0,1,0)),
				rz, glm::vec3(0,0,1));
}
