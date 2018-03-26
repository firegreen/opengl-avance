#include "Application.hpp"
#include "Object3D.hpp"
#include "utils.hpp"

#include <iostream>

#include <imgui.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glmlv/imgui_impl_glfw_gl3.hpp>
#include <glmlv/simple_geometry.hpp>
#include <GL/glu.h>



const size_t Application::windowHeight = 700;
const size_t Application::windowWidth = 1200;

int Application::run()
{
	// Profiling
	GLuint64 startTime, stopTime;
	GLuint queryID[20];
	glGenQueries(20, queryID);

	checkGlError();

	int lightStorageSize = 0;
	DirectionnalLight::Data* shadowPtr;

	GLint texturesunits[DirectionnalLightShadowMap::DIR_MAX_SHADOW_COUNT];
	for (int i=0; i < DirectionnalLightShadowMap::DIR_MAX_SHADOW_COUNT; ++i)
		texturesunits[i] = 20 + i;
	glProgramUniform1iv(shadingProgram.glId(), shadingProgram.uShadowLightMap, DirectionnalLightShadowMap::DIR_MAX_SHADOW_COUNT, texturesunits);
	for (int i=0; i < DirectionnalLightShadowMap::DIR_MAX_SHADOW_COUNT; ++i)
	{
		glActiveTexture(GL_TEXTURE20 + i);
		glBindTexture(GL_TEXTURE_2D, DirectionnalLightShadowMap::textures[i]);
		glBindSampler(20 + i, DirectionnalLightShadowMap::sampler);
	}

	for (auto iterationCount = 0u; !m_GLFWHandle.shouldClose(); ++iterationCount)
	{
		const auto seconds = glfwGetTime();

		checkGlError();

		lightStorageSize = sizeof(DirectionnalLight::Data)*currentScene->dirLightData.size();
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, dirLightSSBO);
		shadowPtr = (DirectionnalLight::Data*) glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0,
														lightStorageSize, GL_MAP_WRITE_BIT);
		checkGlError();

		double start = glfwGetTime();
		glQueryCounter(queryID[0], GL_TIMESTAMP);
		//std::cout << "========= Shadow map process =========" << std::endl;
		shadowRender(shadowPtr);
		glQueryCounter(queryID[1], GL_TIMESTAMP);
		//std::cout << "\tdone in " << glfwGetTime() - start << "s" << std::endl;
		checkGlError();

		//std::cout << "========= Geometry pass process =========" << std::endl;
		start = glfwGetTime();
		glQueryCounter(queryID[2], GL_TIMESTAMP);
		materialRender();
		glQueryCounter(queryID[3], GL_TIMESTAMP);
		//std::cout << "\tdone in " << glfwGetTime() - start << "s" << std::endl;
		checkGlError();

		//std::cout << "========= Shading pass process =========" << std::endl;
		glQueryCounter(queryID[4], GL_TIMESTAMP);
		start = glfwGetTime();
		glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
		shadowPtr = nullptr;
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		glBindFramebuffer(GL_READ_FRAMEBUFFER, FBO);
		shadingRender();
		glQueryCounter(queryID[5], GL_TIMESTAMP);
		//std::cout << "\tdone in " << glfwGetTime() - start << "s" << std::endl;
		checkGlError();

		//std::cout << "========= ... =========" << std::endl;
		glQueryCounter(queryID[6], GL_TIMESTAMP);
		glQueryCounter(queryID[7], GL_TIMESTAMP);
		//std::cout << "\tdone in " << glfwGetTime() - start << "s" << std::endl;
		checkGlError();

		/*
		glReadBuffer(GL_COLOR_ATTACHMENT2);
		glBlitFramebuffer(0,0,windowWidth/2,windowHeight,
						  0,0,windowWidth/2,windowHeight,
						  GL_COLOR_BUFFER_BIT, GL_NEAREST);*/

		int width = windowWidth; int height = windowHeight;
		//std::cout << "========= GUI =========" << std::endl;
		glQueryCounter(queryID[8], GL_TIMESTAMP);
		start = glfwGetTime();

		if (displayGUI)
		{
			// GUI code:
			ImGui_ImplGlfwGL3_NewFrame();

			{
				ImGui::Begin("GUI");
				ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
				ImGui::ColorEditMode(ImGuiColorEditMode_RGB);

				ImGui::NewLine();

				if (currentScene)
				{
					ImGui::BeginChild("Ambiant Light", ImVec2(ImGui::GetWindowContentRegionWidth(),100));
					ImGui::Text("Ambiant Light");
					ImGui::ColorEdit3("Color", &currentScene->ambiantLight[0]);
					ImGui::EndChild();
				}

				ImGui::NewLine();

				ImGui::End();
			}

			const auto viewportSize = m_GLFWHandle.framebufferSize();
			glViewport(0, 0, viewportSize.x, viewportSize.y);
			ImGui::Render();
		}
		
		glQueryCounter(queryID[9], GL_TIMESTAMP);
		//std::cout << "\tdone in " << glfwGetTime() - start << "s" << std::endl;
		checkGlError();

		/* Poll for and process events */
		glfwPollEvents();

		/* Swap front and back buffers*/
		//std::cout << "========= Swap Buffers =========" << std::endl;
		start = glfwGetTime();
		m_GLFWHandle.swapBuffers();
		//std::cout << "\tdone in " << glfwGetTime() - start << "s" << std::endl;
		checkGlError();


		auto ellapsedTime = glfwGetTime() - seconds;
		camera.update(float(ellapsedTime));
		camera.getViewMatrix();
		shadowPtr = (DirectionnalLight::Data*) glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0,
														lightStorageSize, GL_MAP_WRITE_BIT);
		shadowViewUpdate(shadowPtr);
		glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
		shadowPtr = nullptr;
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
		//std::cout << "========= Shadow map update =========" << std::endl;
		start = glfwGetTime();
		glQueryCounter(queryID[10], GL_TIMESTAMP);

		auto guiHasFocus = ImGui::GetIO().WantCaptureMouse || ImGui::GetIO().WantCaptureKeyboard;
		if (!guiHasFocus) {
			if (glfwGetKey(m_GLFWHandle.window(), GLFW_KEY_P)) {
				geometryProgram = GeometryProgramHandler(m_ShadersRootPath / m_AppName);
				shadingProgram = ShadingProgramHandler(m_ShadersRootPath / m_AppName);
				shadowProgram = ShadowProgramHandler(m_ShadersRootPath / m_AppName);
				depthProgram = DepthProgramHandler(m_ShadersRootPath / m_AppName);
			}
			if (glfwGetKey(m_GLFWHandle.window(), GLFW_KEY_D)) {
				displayGUI = !displayGUI;
			}
		}
		glQueryCounter(queryID[11], GL_TIMESTAMP);
		//std::cout << "\tdone in " << glfwGetTime() - start << "s" << std::endl << std::endl << std::endl;
		checkGlError();
		/*
		GLint stopTimerAvailable = 0;
		while (!stopTimerAvailable) {
			glGetQueryObjectiv(queryID[9], GL_QUERY_RESULT_AVAILABLE,  &stopTimerAvailable);
		}*/

		//std::cout << "========= GPU TIMES =========" << std::endl;
		/*
		for (int k=0; k<=10;k+=2)
		{
			// get query results
			glGetQueryObjectui64v(queryID[k], GL_QUERY_RESULT, &startTime);
			glGetQueryObjectui64v(queryID[k+1], GL_QUERY_RESULT, &stopTime);

			//printf("%f ms\n", (stopTime - startTime) / 1000000.0);
		}*/
		
		//std::cout << "=============================" << std::endl;
	}

	return 0;
}

void Application::loadImage(std::string filename, GLuint& textureID)
{
	auto image = glmlv::readImage(m_AssetsRootPath / m_AppName / "textures" / filename);
	loadTexture(image, textureID);
}

void Application::loadSkybox(std::string foldername, std::string ext, GLuint &textureID)
{

	auto xn = glmlv::readImage(m_AssetsRootPath / m_AppName / "textures" / foldername / ("XN." + ext));
	auto xp = glmlv::readImage(m_AssetsRootPath / m_AppName / "textures" / foldername / ("XP." + ext));
	auto yn = glmlv::readImage(m_AssetsRootPath / m_AppName / "textures" / foldername / ("YN." + ext));
	auto yp = glmlv::readImage(m_AssetsRootPath / m_AppName / "textures" / foldername / ("YP." + ext));
	auto zn = glmlv::readImage(m_AssetsRootPath / m_AppName / "textures" / foldername / ("ZN." + ext));
	auto zp = glmlv::readImage(m_AssetsRootPath / m_AppName / "textures" / foldername / ("ZP." + ext));

	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	checkGlError();

	//glTexStorage2D(GL_TEXTURE_CUBE_MAP, 1, GL_RGBA32F, xn.width(), xp.height());

	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGBA32F, xn.width(), xn.height(),
					0, GL_RGBA, GL_UNSIGNED_BYTE, xn.data());
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGBA32F, xn.width(), xn.height(),
					0, GL_RGBA, GL_UNSIGNED_BYTE, xp.data());
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGBA32F, xn.width(), xn.height(),
					0, GL_RGBA, GL_UNSIGNED_BYTE, yn.data());
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGBA32F, xn.width(), xn.height(),
					0, GL_RGBA, GL_UNSIGNED_BYTE, yp.data());
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGBA32F, xn.width(), xn.height(),
					0, GL_RGBA, GL_UNSIGNED_BYTE, zn.data());
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGBA32F, xn.width(), xn.height(),
					0, GL_RGBA, GL_UNSIGNED_BYTE, zp.data());

	checkGlError();

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	checkGlError();
}

void Application::initialiseBuffer()
{
	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, FBO);
	int i;
	for (i=0; i<GBufferTextureCount-1; ++i)
	{
		reserveImage(windowWidth, windowHeight, gBufferTextures[i], m_GBufferTextureFormat[i]);
		glFramebufferTexture2D( GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, gBufferTextures[i], 0);
	}
	reserveImage(windowWidth, windowHeight, gBufferTextures[i], m_GBufferTextureFormat[i]);
	glFramebufferTexture2D( GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, gBufferTextures[i], 0);


	GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0,
							 GL_COLOR_ATTACHMENT1,
							 GL_COLOR_ATTACHMENT2,
							 GL_COLOR_ATTACHMENT3,
							 GL_COLOR_ATTACHMENT4};
	glDrawBuffers(5, drawBuffers);
	GLenum error = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);

	if (error != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cerr << "Aie aie aie gro, frame buffer status returns error: " << error << std::endl;
	}

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

	checkGlError();
}

void Application::initialiseScreen()
{
	glm::vec2 quadCoord[3] = {glm::vec2(-1,-1), glm::vec2(3,-1), glm::vec2(-1, 3)};


	glGenBuffers(1, &shadingVBO);
	glBindBuffer(GL_ARRAY_BUFFER, shadingVBO);
	glBufferStorage(GL_ARRAY_BUFFER, 3*sizeof(glm::vec2), quadCoord, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenVertexArrays(1, &shadingVAO);
	glBindVertexArray(shadingVAO);
	glBindBuffer(GL_ARRAY_BUFFER, shadingVBO);

	glEnableVertexAttribArray(positionAttrLocation);
	glVertexAttribPointer(positionAttrLocation, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (const GLvoid*) 0);

	glBindVertexArray(0);

	checkGlError();
}

void Application::initialiseLights()
{
	glGenBuffers(1, &dirLightSSBO);
	glGenBuffers(1, &pointLightSSBO);
	glGenBuffers(1, &shadowTexturesSSBO);

	checkGlError();
}

void Application::initialiseSamplerObjects()
{
	// Note: no need to bind a sampler for modifying it: the sampler API is already direct_state_access
	glGenSamplers(1, &samplerObject);
	glSamplerParameteri(samplerObject, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glSamplerParameteri(samplerObject, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glSamplerParameteri(samplerObject, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glSamplerParameteri(samplerObject, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// Note: no need to bind a sampler for modifying it: the sampler API is already direct_state_access
	glGenSamplers(1, &bufferSamplerObject);
	glSamplerParameteri(bufferSamplerObject, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glSamplerParameteri(bufferSamplerObject, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

void Application::initialiseModels()
{
	models.push_back(std::shared_ptr<ObjectModel>(new ObjectModel(m_AssetsRootPath / "glmlv/models/crytek-sponza/sponza.obj", true)));
}

void Application::resetLights(int lightsCount)
{
	if (currentScene)
	{
		currentScene->resetLights(lightsCount);

		glBindBuffer(GL_SHADER_STORAGE_BUFFER, dirLightSSBO);
		glBufferStorage(GL_SHADER_STORAGE_BUFFER, sizeof(DirectionnalLight::Data)*currentScene->dirLightData.size(), currentScene->dirLightData.data(),
			GL_DYNAMIC_STORAGE_BIT | GL_MAP_WRITE_BIT);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, dirlightBindingId, dirLightSSBO);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
		checkGlError();

		glBindBuffer(GL_SHADER_STORAGE_BUFFER, pointLightSSBO);
		glBufferStorage(GL_SHADER_STORAGE_BUFFER, sizeof(PointLight)*currentScene->pointLightData.size(), currentScene->pointLightData.data(),
			GL_DYNAMIC_STORAGE_BIT | GL_MAP_WRITE_BIT);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, pointlightBindingId, pointLightSSBO);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
		checkGlError();

		glBindBuffer(GL_SHADER_STORAGE_BUFFER, shadowTexturesSSBO);
		glBufferStorage(GL_SHADER_STORAGE_BUFFER, sizeof(int)*currentScene->shadowTextureData.size(), currentScene->shadowTextureData.data(),
			GL_DYNAMIC_STORAGE_BIT | GL_MAP_WRITE_BIT);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, shadowBindingId, shadowTexturesSSBO);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

		checkGlError();

		shadingProgram.bDirLightData = glGetProgramResourceIndex(shadingProgram.glId(), GL_SHADER_STORAGE_BLOCK, "bDirLightData");
		glShaderStorageBlockBinding(shadingProgram.glId(), shadingProgram.bDirLightData, Application::dirlightBindingId);

		shadingProgram.bPointLightData = glGetProgramResourceIndex(shadingProgram.glId(), GL_SHADER_STORAGE_BLOCK, "bPointLightData");
		glShaderStorageBlockBinding(shadingProgram.glId(), shadingProgram.bPointLightData, Application::pointlightBindingId);

		shadingProgram.bShadowData = glGetProgramResourceIndex(shadingProgram.glId(), GL_SHADER_STORAGE_BLOCK, "bShadowData");
		glShaderStorageBlockBinding(shadingProgram.glId(), shadingProgram.bShadowData, Application::shadowBindingId);

		checkGlError();
	}
}

void Application::solidRenderTo(GLuint FBO, size_t x, size_t y, size_t width, size_t height)
{
	glEnable(GL_DEPTH_TEST);

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, FBO);
	glViewport(x, y, width, height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	checkGlError();
	if (currentScene)
	{
		for (const std::shared_ptr<Object3D>& oPtr : currentScene->objects)
		{
			const Object3D& o = *oPtr.get();
			glBindVertexArray(o.VAO);
			int indexCount, indexOffset = 0;
			glUniformMatrix4fv(shadowProgram.uDirLightModelMatrixShadow, 1,
							   GL_FALSE, glm::value_ptr(o.modelMatrix));
			for (int i = 0; i < o.data->shapeCount; i++)
			{
				indexCount = o.data->indexCountPerShape[i];
				glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, (const GLvoid*)(indexOffset * sizeof(GLuint)));
				indexOffset += indexCount;
			}
			glBindVertexArray(0);
		}
	}
	checkGlError();
}

void Application::solidRender()
{
	solidRenderTo(0, 0, 0, windowWidth, windowHeight);
}

void Application::materialRenderTo(GLuint FBO, size_t x, size_t y, size_t width, size_t height)
{
	glEnable(GL_DEPTH_TEST);

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, FBO);
	glViewport(x, y, width, height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	checkGlError();

	glm::mat4 VPMatrix = camera.getVPMatrix();
	//glm::mat4 MVMatrix =  glm::mat4();
	//glm::mat4 MVPMatrix = VPMatrix * glm::scale(glm::mat4(), glm::vec3(5,5,5));
	glm::mat4 normalMat =  glm::mat4();

	glm::mat4 MVMatrix(glm::mat4(glm::mat3(camera.getViewMatrix())) * glm::scale(glm::mat4(), glm::vec3(80,80,80)));
	glm::mat4 MVPMatrix(glm::perspective(
						glm::radians(camera.FoV),
						windowWidth / (float)windowHeight,
						1.f,
						90.f
					) * MVMatrix);

	glProgramUniformMatrix4fv(geometryProgram.glId(), geometryProgram.uModelViewMatrix,     1, GL_FALSE, glm::value_ptr(normalMat));
	glProgramUniformMatrix4fv(geometryProgram.glId(), geometryProgram.uModelViewProjMatrix, 1, GL_FALSE, glm::value_ptr(MVPMatrix));
	glProgramUniformMatrix4fv(geometryProgram.glId(), geometryProgram.uNormalMatrix,        1, GL_FALSE, glm::value_ptr(normalMat));

	geometryProgram.glProgram.use();

	glDepthMask(GL_FALSE);
	glBindVertexArray(currentScene->skyCube->VAO);
	glDrawElements(GL_TRIANGLES, currentScene->skyCube->data->indexBuffer.size(), GL_UNSIGNED_INT, (const GLvoid*) (0));
	glDepthMask(GL_TRUE);
	checkGlError();

	glProgramUniform1i(geometryProgram.glId(), geometryProgram.uKdSampler, 0); // Set the uniform to 0 because we use texture unit 0
	glProgramUniform1i(geometryProgram.glId(), geometryProgram.uKaSampler, 1);
	glProgramUniform1i(geometryProgram.glId(), geometryProgram.uKspecSampler, 2);
	glProgramUniform1i(geometryProgram.glId(), geometryProgram.uKshinSampler, 3);
	glProgramUniform1i(geometryProgram.glId(), geometryProgram.uNormalSampler, 4);
	checkGlError();

	glBindSampler(0, samplerObject);
	glBindSampler(1, samplerObject);
	glBindSampler(2, samplerObject);
	glBindSampler(3, samplerObject);
	glBindSampler(4, samplerObject);

	checkGlError();


	if (currentScene)
	{
		glDepthMask(GL_TRUE);
		for (const std::shared_ptr<Object3D>& oPtr : currentScene->objects)
		{
			const Object3D& o = *oPtr.get();
			MVMatrix = camera.getViewMatrix() * o.modelMatrix;
			MVPMatrix = VPMatrix * o.modelMatrix;
			normalMat = glm::transpose(glm::inverse(MVMatrix));

			glProgramUniformMatrix4fv(geometryProgram.glId(), geometryProgram.uModelViewMatrix,     1, GL_FALSE, glm::value_ptr(MVMatrix));
			glProgramUniformMatrix4fv(geometryProgram.glId(), geometryProgram.uModelViewProjMatrix, 1, GL_FALSE, glm::value_ptr(MVPMatrix));
			glProgramUniformMatrix4fv(geometryProgram.glId(), geometryProgram.uNormalMatrix,        1, GL_FALSE, glm::value_ptr(normalMat));

			glBindVertexArray(o.VAO);
			int indexCount, indexOffset = 0;
			if (o.data->materialCount)
			{
				for (int i=0; i<o.data->shapeCount; i++)
				{
					const glmlv::ObjData::PhongMaterial& mat= o.data->materials[o.data->materialIDPerShape[i]];
					if (mat.KdTextureId>=0)
					{
						glActiveTexture(GL_TEXTURE0);
						glBindTexture(GL_TEXTURE_2D, (*o.textures)[mat.KdTextureId]);
						glProgramUniform1i(geometryProgram.glId(), geometryProgram.uUseDTexture, 1);
					}
					else
					{
						glProgramUniform1i(geometryProgram.glId(), geometryProgram.uUseDTexture, 0);
					}
					if (mat.KaTextureId>=0)
					{
						glActiveTexture(GL_TEXTURE1);
						glBindTexture(GL_TEXTURE_2D, (*o.textures)[mat.KaTextureId]);
						glProgramUniform1i(geometryProgram.glId(), geometryProgram.uUseATexture, 1);
					}
					else
					{
						glProgramUniform1i(geometryProgram.glId(), geometryProgram.uUseATexture, 0);
					}
					if (mat.KsTextureId>=0)
					{
						glActiveTexture(GL_TEXTURE2);
						glBindTexture(GL_TEXTURE_2D, (*o.textures)[mat.KsTextureId]);
						glProgramUniform1i(geometryProgram.glId(), geometryProgram.uUseSpecTexture, 1);
					}
					else
					{
						glProgramUniform1i(geometryProgram.glId(), geometryProgram.uUseSpecTexture, 0);
					}
					if (mat.shininessTextureId>=0)
					{
						glActiveTexture(GL_TEXTURE3);
						glBindTexture(GL_TEXTURE_2D, (*o.textures)[mat.shininessTextureId]);
						glProgramUniform1i(geometryProgram.glId(), geometryProgram.uUseShinTexture, 1);
					}
					else
					{
						glProgramUniform1i(geometryProgram.glId(), geometryProgram.uUseShinTexture, 0);
					}

					indexCount = o.data->indexCountPerShape[i];
					glProgramUniform3f(geometryProgram.glId(), geometryProgram.uKd, mat.Kd.r,mat.Kd.g,mat.Kd.b);
					glProgramUniform3f(geometryProgram.glId(), geometryProgram.uKs, mat.Ks.r,mat.Ks.g,mat.Ks.b);
					glProgramUniform3f(geometryProgram.glId(), geometryProgram.uKa, mat.Ka.r,mat.Ka.g,mat.Ka.b);
					glProgramUniform1f(geometryProgram.glId(), geometryProgram.uShininess, mat.shininess);
					glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, (const GLvoid*) (indexOffset * sizeof(GLuint)));
					indexOffset += indexCount;
				}
			}
			else
			{
				glm::vec3 Kd(0.8f,0.8f,0.8f);
				glProgramUniform3f(geometryProgram.glId(), geometryProgram.uKd, Kd.r, Kd.g, Kd.b);
				glDrawElements(GL_TRIANGLES, o.data->indexBuffer.size(), GL_UNSIGNED_INT, 0);
			}
			glBindVertexArray(0);
			checkGlError();
		}
	}
}

void Application::materialRender()
{
	materialRenderTo(FBO, 0, 0, windowWidth, windowHeight);
}

void Application::shadowRender(DirectionnalLight::Data *shadowPtr)
{
	for (int i=0; i< currentScene->dirLights.size(); ++i)
	{
		if (currentScene->dirLights[i].shadowMap.get())
		{
			DirectionnalLightShadowMap& shadow = *currentScene->dirLights[i].shadowMap.get();
			if (shadow.isDirty)
			{
				shadowProgram.glProgram.use();
				currentScene->dirLights[i].computeMatrix(currentScene->sceneCenter, currentScene->sceneDiag);
				glUniformMatrix4fv(shadowProgram.uDirLightViewProjMatrixShadow, 1,
								   GL_FALSE, glm::value_ptr(currentScene->dirLights[i].data.lightMatrix));
				checkGlError();
				solidRenderTo(shadow.FBO, 0, 0, DirectionnalLightShadowMap::resolution, DirectionnalLightShadowMap::resolution);
				currentScene->dirLights[i].data.lightMatrix = currentScene->dirLights[i].data.lightMatrix *
						camera.getRcpViewMatrix();
				memcpy(shadowPtr + i, (void*)&currentScene->dirLights[i].data, sizeof(glm::mat4));
				shadow.isDirty = false;
				checkGlError();
			}
		}
	}
}

void Application::shadowViewUpdate(DirectionnalLight::Data *shadowPtr)
{
	for (int i=0; i< currentScene->dirLightData.size(); ++i)
	{
		if (currentScene->dirLights[i].shadowMap.get())
		{
			currentScene->dirLights[i].computeMatrix(currentScene->sceneCenter, currentScene->sceneDiag);
			currentScene->dirLights[i].data.lightMatrix = currentScene->dirLights[i].data.lightMatrix *
					camera.getRcpViewMatrix();
			memcpy(shadowPtr + i, (void*)&currentScene->dirLights[i].data, sizeof(glm::mat4));
		}
	}
}

void Application::shadingRenderTo(GLuint FBO, size_t x, size_t y, size_t width, size_t height)
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, FBO);
	glViewport(x, y, width, height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (currentScene)
	{
		glActiveTexture(GL_TEXTURE30);
		glBindTexture(GL_TEXTURE_CUBE_MAP, currentScene->skyboxTexture);

		checkGlError();
		glProgramUniform1i(shadingProgram.glId(), shadingProgram.uSkyboxSampler, 30);

		glProgramUniform3fv(shadingProgram.glId(), shadingProgram.uAmbiantLightIntensity, 1,
							glm::value_ptr(currentScene->ambiantLight));
		glProgramUniform1f(shadingProgram.glId(), shadingProgram.uShadowMapBias, shadowMapBias);

		glProgramUniform1i(shadingProgram.glId(), shadingProgram.uGDiffuse,        0); // Set the uniform to 0 because we use texture unit 0
		glProgramUniform1i(shadingProgram.glId(), shadingProgram.uGAmbient,        1);
		glProgramUniform1i(shadingProgram.glId(), shadingProgram.uGlossyShininess, 2);
		glProgramUniform1i(shadingProgram.glId(), shadingProgram.uGPosition,       3);
		glProgramUniform1i(shadingProgram.glId(), shadingProgram.uGNormal,         4);
		glProgramUniform1i(shadingProgram.glId(), shadingProgram.uGShadingDepth, 5);

		glProgramUniform3fv(shadingProgram.glId(), shadingProgram.uFogColor, 1, glm::value_ptr(currentScene->fogColor));
		glProgramUniform1f(shadingProgram.glId(), shadingProgram.uFogDistance, currentScene->fogDistance);
		glProgramUniform1f(shadingProgram.glId(), shadingProgram.uFogDensity, currentScene->fogDensity);

		glProgramUniformMatrix4fv(shadingProgram.glId(),
								  shadingProgram.uViewMatrix, 1, GL_FALSE, glm::value_ptr(camera.getViewMatrix()));

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, gBufferTextures[GDiffuse]);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, gBufferTextures[GAmbient]);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, gBufferTextures[GGlossyShininess]);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, gBufferTextures[GPosition]);
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, gBufferTextures[GNormal]);
		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_2D, gBufferTextures[GDepth]);
		checkGlError();

		glBindSampler(0, bufferSamplerObject);
		glBindSampler(1, bufferSamplerObject);
		glBindSampler(2, bufferSamplerObject);
		glBindSampler(3, bufferSamplerObject);
		glBindSampler(4, bufferSamplerObject);
		glBindSampler(5, bufferSamplerObject);
		checkGlError();

		shadingProgram.glProgram.use();
		glBindVertexArray(shadingVAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glBindVertexArray(0);
		checkGlError();
	}
}

void Application::shadingRender()
{
	shadingRenderTo(0, 0, 0, windowWidth, windowHeight);
}

Application::Application(int argc, char** argv):
	m_AppPath { glmlv::fs::path{ argv[0] } },
	m_AppName { m_AppPath.stem().string() },
	m_ImGuiIniFilename { m_AppName + ".imgui.ini" },
	m_ShadersRootPath { m_AppPath.parent_path() / "shaders" },
	m_AssetsRootPath { m_AppPath.parent_path() / "assets" },
	scenes(3),
	currentScene(nullptr),
	geometryProgram(m_ShadersRootPath / m_AppName),
	shadingProgram(m_ShadersRootPath / m_AppName),
	shadowProgram(m_ShadersRootPath / m_AppName),
	depthProgram(m_ShadersRootPath / m_AppName)
{
	glm::vec2 quadCoord[3] = {glm::vec2(-1,-1), glm::vec2(3,-1), glm::vec2(-1, 3)};
	ImGui::GetIO().IniFilename = m_ImGuiIniFilename.c_str(); // At exit, ImGUI will store its windows positions in this file

	initialiseModels();
	initialiseBuffer();
	initialiseLights();
	if (currentScene == nullptr)
	{
		currentScene = &scenes[0];
		std::shared_ptr<Object3D> ptr(models[0]->instance());
		currentScene->objects.push_back(ptr);
		loadSkybox("skybox1","jpg",currentScene->skyboxTexture);
		camera.FoV = 60;
		camera.zNear = currentScene->sceneDiag * 0.01;
		camera.zFar = currentScene->sceneDiag * 2;
		camera.getProjectionMatrix(true);

		resetLights();
	}
	initialiseScreen();
	initialiseSamplerObjects();


	checkGlError();
}

Application::~Application()
{
	if (shadingVAO)
	{
		glDeleteVertexArrays(1, &shadingVAO);
	}
	if (shadingVBO)
	{
		glDeleteBuffers(1, &shadingVBO);
	}
	if (dirLightSSBO)
	{
		glDeleteBuffers(1, &dirLightSSBO);
	}
	if (pointLightSSBO)
	{
		glDeleteBuffers(1, &pointLightSSBO);
	}
	if (FBO)
	{
		glDeleteBuffers(1, &FBO);
	}

	ImGui_ImplGlfwGL3_Shutdown();
	glfwTerminate();
}
