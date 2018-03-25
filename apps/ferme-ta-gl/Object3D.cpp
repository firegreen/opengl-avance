#include "Object3D.hpp"
#include "assimp/vector2.h"
#include "tools.h"

#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags

#include <QFile>

#include "glexception.h"

Object3D::Object3D(QList<Vertex3D> vertex, const QColor &globalColor, int drawMode)
    : indexStart(0), indexCount(0),drawMode(drawMode),vertex(vertex),
      texture(NULL), normalTexture(NULL),
      diffuseColor(globalColor), ambiantColor(globalColor),
      specularColor(globalColor)
{

}

Object3D::Object3D(std::vector<Vertex3D> vertex, const QColor &globalColor, int drawMode)
    : indexStart(0), indexCount(0),drawMode(drawMode),
      texture(NULL), normalTexture(NULL),
      diffuseColor(globalColor), ambiantColor(globalColor),
      specularColor(globalColor)
{
    for(unsigned int i=0;i<vertex.size();i++)
        this->vertex << vertex[i];
}


Object3D::Object3D(QList<Vertex3D> vertex, int drawMode)
    : Object3D(vertex,QColor(255,255,255),drawMode)
{}

Object3D::Object3D(std::vector<Vertex3D> vertex, int drawMode)
    : Object3D(vertex,QColor(255,255,255),drawMode)
{}

Object3D::Object3D(const QColor &globalColor, int drawMode)
    : drawMode(drawMode), texture(NULL), normalTexture(NULL),
      diffuseColor(globalColor), ambiantColor(globalColor),
      specularColor(globalColor)
{

}

Object3D::Object3D(const Object3D &other)
	: indexStart(other.indexStart),
	  indexCount(other.indexCount),
      drawMode(other.drawMode),
      texture(other.texture),
      normalTexture(other.normalTexture),
	  diffuseColor(other.diffuseColor),
	  ambiantColor(other.ambiantColor),
      specularColor(other.specularColor)
{}

void Object3D::scale(float t)
{
	QList<Vertex3D>::iterator i;
	QMatrix4x4 finalScaleMat;
	finalScaleMat.scale(t);
	QMatrix4x4 normalMat;
	normalMat = finalScaleMat.inverted().transposed();


	for(i=vertex.begin();i!=vertex.end();i++){
		(*i).normal = toAiVector(normalMat * toQVector((*i).normal));
		(*i).position = toAiVector(finalScaleMat * toQVector((*i).position));
	}
}
void Object3D::scale(float x, float y, float z)
{
	QList<Vertex3D>::iterator i;
	QMatrix4x4 finalScaleMat;
	finalScaleMat.scale(x,y,z);
	QMatrix4x4 normalMat;
	normalMat = finalScaleMat.inverted().transposed();


	for(i=vertex.begin();i!=vertex.end();i++){
		(*i).normal = toAiVector(normalMat * toQVector((*i).normal));
		(*i).position = toAiVector(finalScaleMat * toQVector((*i).position));
	}
}

void Object3D::translate(float dx, float dy, float dz)
{
	QList<Vertex3D>::iterator i;
	QMatrix4x4 finalRotationMat;
	finalRotationMat.translate(dx,dy,dz);
	QMatrix4x4 normalMat;
	normalMat = finalRotationMat.inverted().transposed();


	for(i=vertex.begin();i!=vertex.end();i++){
		(*i).normal = toAiVector(normalMat * toQVector((*i).normal));
		(*i).position = toAiVector(finalRotationMat * toQVector((*i).position));
	}
}

void Object3D::rotate(float rotationX, float rotationY, float rotationZ)
{
	QList<Vertex3D>::iterator i;
	QMatrix4x4 finalRotationMat;
	finalRotationMat.rotate(rotationX,1,0,0);
	finalRotationMat.rotate(rotationY,0,1,0);
	finalRotationMat.rotate(rotationZ,0,0,1);
	QMatrix4x4 normalMat;
	normalMat = finalRotationMat.inverted().transposed();


	for(i=vertex.begin();i!=vertex.end();i++){
		(*i).normal = toAiVector(normalMat * toQVector((*i).normal));
		(*i).position = toAiVector(finalRotationMat * toQVector((*i).position));
	}
}

void Object3D::clone(Object3D &other) const
{
	other.indexStart = indexStart;
	other.indexCount = indexCount;
	other.drawMode = drawMode;
}

void Object3D::copy(const Object3D &other)
{
	this->indexCount = other.indexCount;
	this->indexStart = other.indexStart;
	this->drawMode = other.drawMode;
}

void Object3D::draw() const
{
	if(texture){
		texture->bind();
		glDrawArrays(drawMode, indexStart, indexCount);
		texture->release();
	}
	else
		glDrawArrays(drawMode, indexStart, indexCount);
}

void Object3D::setGlobalColor(const QColor &color)
{
    diffuseColor = ambiantColor = specularColor = color;
}

QList<Vertex3D> Object3D::QListFromStdVector(std::vector<Vertex3D> v)
{
    return QList<Vertex3D>::fromVector(QVector<Vertex3D>::fromStdVector(v));
}

QList<Vertex3D> Object3D::makeRect(const QRectF& bounds)
{
     QList<Vertex3D> retour;
     retour << Vertex3D(bounds.left(),bounds.top(),0,0,0)
            << Vertex3D(bounds.left(),bounds.bottom(),0,0,1)
            << Vertex3D(bounds.right(),bounds.top(),0,1,0)
            << Vertex3D(bounds.right(),bounds.bottom(),0,1,1);
     return retour;
}

QList<Vertex3D> vertexFromTexturedMesh(aiMesh* mesh){
	QList<Vertex3D> vertex;
	for(unsigned j=0;j<mesh->mNumFaces;j++){
		aiFace* face = mesh->mFaces + j;
		for(unsigned k=0;k<3;k++){
			int indice = face->mIndices[k];
			vertex.append(Vertex3D(mesh->mVertices[indice],
										aiColor3D(1,1,1),
										toAiVector2D(mesh->mTextureCoords[0][indice]),
										mesh->mNormals[indice]));
		}
	}
	return vertex;
}
QList<Vertex3D> vertexFromColoredMesh(aiMesh* mesh){
	QList<Vertex3D> vertex;
	for(unsigned j=0;j<mesh->mNumFaces;j++){
		aiFace* face = mesh->mFaces + j;
		for(unsigned k=0;k<3;k++){
			int indice = face->mIndices[k];
			vertex.append(Vertex3D(mesh->mVertices[indice],
										toAiColor3D(mesh->mColors[0][indice]),
										aiVector2D(0,0),
										mesh->mNormals[indice]));
		}
	}
	return vertex;
}
QList<Vertex3D> vertexFromBlankMesh(aiMesh* mesh){
	QList<Vertex3D> vertex;
	for(unsigned j=0;j<mesh->mNumFaces;j++){
		aiFace* face = mesh->mFaces + j;
		for(unsigned k=0;k<3;k++){
			int indice = face->mIndices[k];
			vertex.append(Vertex3D(mesh->mVertices[indice],
										aiColor3D(1,1,1),
										aiVector2D(0,0),
										mesh->mNormals[indice]));
		}
	}
	return vertex;
}

QList<Object3D *> Object3D::importFromFile(QString filepath)
{
	Assimp::Importer importer;
	QFile file(filepath);
	file.open(QIODevice::ReadOnly);
	if(!file.isOpen())
		throw GLException("3D Import Error",
						  "Cannot read file : " + filepath + "\n" +
						  file.errorString());
	char* data = new char[file.size()];
	file.read(data,file.size());

	const aiScene* scene = importer.ReadFileFromMemory(data, file.size(),
			aiProcess_Triangulate	|
			aiProcess_GenNormals	|
			aiProcess_GenUVCoords);
	delete data;
	file.close();

	if( !scene){
		qDebug() << importer.GetErrorString();
		throw GLException("3D Import Error",
						  QString(importer.GetErrorString()));
		//throw std::exception(importer.GetErrorString());
	}
	QList<Object3D*> _return;
	for(unsigned i=0;i<scene->mNumMeshes;i++){
		aiMesh* mesh = scene->mMeshes[i];
		//aiMaterial* m = scene->mMaterials[mesh->mMaterialIndex];
		if(mesh->mTextureCoords[0]==NULL)
			if(mesh->mColors[0]==NULL)
				_return.append(new Object3D(vertexFromBlankMesh(mesh)));
			else
				_return.append(new Object3D(vertexFromColoredMesh(mesh)));
		else
			_return.append(new Object3D(vertexFromTexturedMesh(mesh)));
	}
	return _return; //ToDo
}

Vertex3D::Vertex3D(aiVector3D position, aiColor3D color, aiVector2D texCoord,
				   aiVector3D normal, aiVector3D bitangente, aiVector3D tangente)
	: position(position), color(color), texCoord(texCoord),
	  normal(normal), bitangente(bitangente), tangente(tangente)
{}

Vertex3D::Vertex3D(aiVector3D position, aiColor3D color,
				   aiVector2D texCoord, aiVector3D normal)
	: position(position), color(color), texCoord(texCoord),
	  normal(normal)
{}

Vertex3D::Vertex3D(aiVector3D position, aiColor3D color)
	: position(position), color(color)
{}

Vertex3D::Vertex3D(aiVector3D position, aiVector2D texCoord)
	: position(position), color(1,1,1), texCoord(texCoord)
{}

Vertex3D::Vertex3D(aiVector3D position)
	: position(position), color(1,1,1)
{

}

Vertex3D::Vertex3D()
	: color(1,1,1)
{

}

Vertex3D::Vertex3D(float x, float y, float z)
	: position(x,y,z), color(1,1,1)
{}

Vertex3D::Vertex3D(float x, float y, float z, float u, float v)
	: position(x,y,z), color(1,1,1), texCoord(u,v)
{}

Vertex3D::Vertex3D(float x, float y, float z, float r, float g, float b)
	: position(x,y,z), color(r,g,b)
{}

QString Vertex3D::toString()
{
	return "Position: " + QString("(%1,%2,%3)").arg(position.x)
												.arg(position.y)
												.arg(position.z) +
			"\nColor: " + QString("(%1,%2,%3)").arg(color.r)
												.arg(color.g)
												.arg(color.b) +
			"\nTexCoords: " + QString("(%1,%2)").arg(texCoord.x)
												.arg(texCoord.y);
}

    © 2018 GitHub, Inc.
    Terms
    Privacy
    Security
    Status
    Help

    Contact GitHub
    API
    Training
    Shop
    Blog
    About

Press h to open a hovercard with more details.
