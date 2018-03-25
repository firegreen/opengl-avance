#include <glmlv/GLProgram.hpp>

#include <glmlv/filesystem.hpp>

struct GeometryProgramHandler {
	
	GeometryProgramHandler(glmlv::fs::path path);

	glmlv::GLProgram glProgram;
	GLuint glId() const { return glProgram.glId(); }

	GLuint uModelViewProjMatrix;
	GLuint uModelViewMatrix;
	GLuint uNormalMatrix;

	GLuint uKd, uKs, uKa, uShininess;

	GLuint uUseDTexture;
	GLuint uUseATexture;
	GLuint uUseSpecTexture;
	GLuint uUseShinTexture;

	GLuint uKdSampler;
	GLuint uKaSampler;
	GLuint uKspecSampler;
	GLuint uKshinSampler;
	GLuint uNormalSampler;

};

struct ShadingProgramHandler {

	ShadingProgramHandler(glmlv::fs::path path);

	glmlv::GLProgram glProgram;
	GLuint glId() const { return glProgram.glId(); }

	GLuint uGPosition;
	GLuint uGNormal;
	GLuint uGAmbient;
	GLuint uGDiffuse;
	GLuint uGlossyShininess;
	GLuint uGDepth;
	GLuint uGShadingDepth;

	GLuint uSkyboxSampler;
	GLuint uAmbiantLightIntensity;

	GLuint uShadowMapBias;
	GLuint uCastShadow;
	GLuint uShadowLightMap;

	GLuint uFogColor;
	GLuint uFogDensity;
	GLuint uFogDistance;

	// TODO g�rer les BindingIndex
	const static GLuint dirlightBindingIndex = 1, pointlightBindingIndex = 2;

	GLuint bDirLightData;
	GLuint bPointLightData;
};

struct ShadowProgramHandler {

	ShadowProgramHandler(glmlv::fs::path path);
	glmlv::GLProgram glProgram;
	GLuint glId() const { return glProgram.glId(); }

	GLuint uDirLightViewProjMatrixShadow;

};


struct DepthProgramHandler {

	DepthProgramHandler(glmlv::fs::path path);
	glmlv::GLProgram glProgram;
	GLuint glId() const { return glProgram.glId(); }

	GLuint uGDepth;

};