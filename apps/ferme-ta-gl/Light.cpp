#include "Light.hpp"

const int DirectionnalLightShadowMap::DIR_SHADOW_TEXTURE_UNIT_OFFSET = 15;
const unsigned int DirectionnalLightShadowMap::DIR_MAX_SHADOW_COUNT = 10;
float DirectionnalLightShadowMap::shadowMapBias = 0.01f;
int32_t DirectionnalLightShadowMap::resolution = 1024;
GLuint* DirectionnalLightShadowMap::textures = nullptr;
GLuint DirectionnalLightShadowMap::textureCount = 0;
GLuint DirectionnalLightShadowMap::sampler = 0;

inline void DirectionnalLightShadowMap::initialiaze()
{
	if (!textures)
	{
		textures = new GLuint[DirectionnalLightShadowMap::DIR_MAX_SHADOW_COUNT];

		for (int i = 0; i<DIR_MAX_SHADOW_COUNT; ++i)
			reserveImage(resolution, resolution, textures[i], GL_DEPTH24_STENCIL8);

		GLint texturesunits[DIR_MAX_SHADOW_COUNT];
		for (int i = 0; i < DirectionnalLightShadowMap::DIR_MAX_SHADOW_COUNT; ++i)
			texturesunits[i] = DirectionnalLightShadowMap::DIR_SHADOW_TEXTURE_UNIT_OFFSET + i;
		checkGlError();
		textureCount = 0;
	}

	if (!sampler)
	{
		glGenSamplers(1, &sampler);
		glSamplerParameteri(sampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glSamplerParameteri(sampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glSamplerParameteri(sampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glSamplerParameteri(sampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		glSamplerParameteri(sampler, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE); // Cette ligne
		glSamplerParameteri(sampler, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL); // Et celle ci
		checkGlError();
	}

	layerID = textureCount;
	++textureCount;

	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, FBO);

	glFramebufferTexture(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, textures[layerID], 0);

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	checkGlError();

	GLenum error = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);
	if (error != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cerr << "Aie aie aie gro, frame buffer status returns error: " << error << std::endl;
	}
}


void DirectionnalLightShadowMap::setUniforms(glmlv::GLProgram &shadingProgram, GLuint uShadowLightTexture)
{
	glProgramUniform1iv(shadingProgram.glId(), uShadowLightTexture, DirectionnalLightShadowMap::DIR_MAX_SHADOW_COUNT, texturesunits);
	for (int i = 0; i < DirectionnalLightShadowMap::textureCount; ++i)
	{
		glActiveTexture(GL_TEXTURE0 + DirectionnalLightShadowMap::DIR_SHADOW_TEXTURE_UNIT_OFFSET + i);
		glBindTexture(GL_TEXTURE_2D, DirectionnalLightShadowMap::textures[i]);
		glBindSampler(DirectionnalLightShadowMap::DIR_SHADOW_TEXTURE_UNIT_OFFSET + i, DirectionnalLightShadowMap::sampler);
	}
}

void DirectionnalLightShadowMap::freeMaps()
{
	textureCount = 0;
}


void DirectionnalLight::castShadow(bool cast)
{
	if (cast)
	{
		shadowMap.reset(new DirectionnalLightShadowMap());
		shadowMap->initialiaze();
	}
	else
	{
		shadowMap.release();
	}
}

void DirectionnalLight::computeMatrix(glm::vec3 sceneCenter, float sceneDiag)
{
	const glm::vec3 lightDir = glm::vec3(data.direction);
	const auto dirLightUpVector = computeDirectionVectorUp(lightDir);
	const auto dirLightViewMatrix = glm::lookAt(sceneCenter - lightDir * sceneDiag * 0.5f, sceneCenter, dirLightUpVector);
	auto lightProj = glm::ortho(-sceneDiag, sceneDiag, -sceneDiag, sceneDiag, 0.01f * sceneDiag, sceneDiag);
	data.lightMatrix = lightProj * dirLightViewMatrix;
}
