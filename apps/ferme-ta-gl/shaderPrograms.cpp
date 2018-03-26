#include "shaderPrograms.hpp"


#include "utils.hpp"
#include "Application.hpp"

GeometryProgramHandler::GeometryProgramHandler(glmlv::fs::path path)
	: glProgram(glmlv::compileProgram({ path / "geometryPass.vs.glsl", path / "geometryPass.fs.glsl" }))

	, uModelViewProjMatrix(glProgram.getUniformLocation("uModelViewProjMatrix"))
	, uModelViewMatrix(glProgram.getUniformLocation("uModelViewMatrix"))
	, uNormalMatrix(glProgram.getUniformLocation("uNormalMatrix"))

	, uKd(glProgram.getUniformLocation("uKd"))
	, uKs(glProgram.getUniformLocation("uKs"))
	, uKa(glProgram.getUniformLocation("uKa"))
	, uShininess(glProgram.getUniformLocation("uShininess"))

	, uUseDTexture(glProgram.getUniformLocation("uUseDTexture"))
	, uUseATexture(glProgram.getUniformLocation("uUseATexture"))
	, uUseSpecTexture(glProgram.getUniformLocation("uUseSpecTexture"))
	, uUseShinTexture(glProgram.getUniformLocation("uUseShinTexture"))

	, uKdSampler(glProgram.getUniformLocation("uKdSampler"))
	, uKaSampler(glProgram.getUniformLocation("uKaSampler"))
	, uKspecSampler(glProgram.getUniformLocation("uKspecSampler"))
	, uKshinSampler(glProgram.getUniformLocation("uKshinSampler"))
	, uNormalSampler(glProgram.getUniformLocation("uNormalSampler"))
{
	checkGlError();
}

ShadingProgramHandler::ShadingProgramHandler(glmlv::fs::path path)
	: glProgram(glmlv::compileProgram({ path / "shadingPass.vs.glsl", path / "shadingPass.fs.glsl" }))

	, uGPosition(glProgram.getUniformLocation("uGPosition"))
	, uGNormal(glProgram.getUniformLocation("uGNormal"))
	, uGAmbient(glProgram.getUniformLocation("uGAmbient"))
	, uGDiffuse(glProgram.getUniformLocation("uGDiffuse"))
	, uGlossyShininess(glProgram.getUniformLocation("uGlossyShininess"))
	, uGDepth(glProgram.getUniformLocation("uGDepth"))
	, uGShadingDepth(glProgram.getUniformLocation("uGShadingDepth"))

	, uSkyboxSampler(glProgram.getUniformLocation("uSkyboxSampler"))
	, uAmbiantLightIntensity(glProgram.getUniformLocation("uAmbiantLightIntensity"))

	, uShadowMapBias(glProgram.getUniformLocation("uShadowMapBias"))
	, uCastShadow(glProgram.getUniformLocation("uCastShadow"))
	, uShadowLightMap(glProgram.getUniformLocation("uShadowLightMap"))

	, uFogColor(glProgram.getUniformLocation("uFogColor"))
	, uFogDensity(glProgram.getUniformLocation("uFogDensity"))
	, uFogDistance(glProgram.getUniformLocation("uFogDistance"))
{
	checkGlError();
}

ShadowProgramHandler::ShadowProgramHandler(glmlv::fs::path path)
	: glProgram(glmlv::compileProgram({ path / "directionalSM.vs.glsl", path / "directionalSM.fs.glsl" }))

	, uDirLightViewProjMatrixShadow(glProgram.getUniformLocation("uDirLightViewProjMatrixShadow"))
{
	checkGlError();
}

DepthProgramHandler::DepthProgramHandler(glmlv::fs::path path)
	: glProgram(glmlv::compileProgram({ path / "shadingPass.vs.glsl", path / "depth.fs.glsl" }))

	, uGDepth(glProgram.getUniformLocation("uGDepth"))
{
	checkGlError();
}
