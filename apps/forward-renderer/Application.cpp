#include "Application.hpp"

#include <iostream>

#include <imgui.h>
#include <glm/glm.hpp>
#include <glmlv/imgui_impl_glfw_gl3.hpp>
#include <glmlv/simple_geometry.hpp>

int Application::run()
{
    float clearColor[3] = { 0, 0, 0 };
    float dirLightColor[3] = { 1,1,0.5f };
    float dirLightDirection[3] = { 0.4f, 0.2f, -1 };
    float pointLightColor[3] = { 40.f,10.f,80.f };
    float pointLightPos[3] = { -1, -10, 1 };
    float ambiantLightColor[3] = { 0.2f,0.2f,0.2f };
    // Loop until the user closes the window
    glEnable(GL_DEPTH_TEST);
    for (auto iterationCount = 0u; !m_GLFWHandle.shouldClose(); ++iterationCount)
    {
        const auto seconds = glfwGetTime();
        float FoV = 60.0f;
        glm::mat4 projectionMat = glm::perspective(
                    glm::radians(FoV), // The vertical Field of View, in radians: the amount of "zoom". Think "camera lens". Usually between 90° (extra wide) and 30° (quite zoomed in)
                    m_nWindowWidth / (float)m_nWindowHeight,       // Aspect Ratio. Depends on the size of your window. Notice that 4/3 == 800/600 == 1280/960, sounds familiar ?
                    0.1f,              // Near clipping plane. Keep as big as possible, or you'll get precision issues.
                    100.0f             // Far clipping plane. Keep as little as possible.
                );
        glm::mat4 viewMat = camera.getViewMatrix();

        glm::mat4 cubeModelMat = glm::rotate(glm::translate(glm::mat4(), glm::vec3(0,0,-5)), glm::pi<float>()/8.0f, glm::vec3(1,1,0.5f));
        glm::mat4 sphereModelMat = glm::rotate(glm::translate(glm::mat4(), glm::vec3(-4,3,-10)), glm::pi<float>()/8.0f, glm::vec3(0.5f,1,1));
        glm::mat4 sceneModelMat =
                glm::scale(
                    glm::rotate(
                        glm::translate(glm::mat4(), glm::vec3(1,-2,-4)),
                        0.0f, glm::vec3(1,1,1)),
                    glm::vec3(0.03f,0.03f,0.03f)
                );

        glm::vec4 directionalDir = glm::normalize(viewMat * glm::vec4(dirLightDirection[0],dirLightDirection[1],dirLightDirection[2],0));
        glProgramUniform3f(program.glId(), uDirectionalLightDir,directionalDir.x,directionalDir.y,directionalDir.z);
        glProgramUniform3f(program.glId(), uDirectionalLightIntensity, dirLightColor[0],dirLightColor[1],dirLightColor[2]);

        glm::vec4 pointPos = viewMat * glm::vec4(pointLightPos[0],pointLightPos[1],pointLightPos[2],1);
        glProgramUniform3f(program.glId(),uPointLightPosition, pointPos.x,pointPos.y,pointPos.z);
        glProgramUniform3f(program.glId(),uPointLightIntensity, pointLightColor[0],pointLightColor[1],pointLightColor[2]);

        glProgramUniform3f(program.glId(),uAmbiantLightIntensity, ambiantLightColor[0],ambiantLightColor[1],ambiantLightColor[2]);

        glProgramUniform3f(program.glId(),uKd, 1, 1, 1);
        glProgramUniform3f(program.glId(),uKs, 0.2f, 0.2f, 0.2f);
        glProgramUniform3f(program.glId(),uKa, 1, 1, 1);
        glProgramUniform1f(program.glId(),uShininess, 0.2f);
        glProgramUniform1i(program.glId(),uUseTexture, 1);

        glClear(GL_COLOR_BUFFER_BIT);
        glClear(GL_DEPTH_BUFFER_BIT);

        // Put here rendering code
        glm::mat4 modelViewMat = viewMat * cubeModelMat;
        glm::mat4 modelViewProjMat = projectionMat * modelViewMat;
        glm::mat4 normalMat = glm::transpose(glm::inverse(modelViewMat));
        glUniformMatrix4fv(uModelViewMatrix, 1, GL_FALSE, &modelViewMat[0][0]);
        glUniformMatrix4fv(uModelViewProjMatrix,1, GL_FALSE, &modelViewProjMat[0][0]);
        glUniformMatrix4fv(uNormalMatrix,1, GL_FALSE, &normalMat[0][0]);

        glActiveTexture(GL_TEXTURE0);
        glUniform1i(uKdSampler, 0); // Set the uniform to 0 because we use texture unit 0
        glBindSampler(0, samplerObject); // Tell to OpenGL what sampler we want to use on this texture unit

        glBindTexture(GL_TEXTURE_2D, woodTexture);

        glBindVertexArray(cubeVAO);
        glDrawElements(GL_TRIANGLES, cube.indexBuffer.size(), GL_UNSIGNED_INT, nullptr);

        modelViewMat = viewMat * sphereModelMat;
        modelViewProjMat = projectionMat * modelViewMat;
        normalMat = glm::transpose(glm::inverse(modelViewMat));
        glUniformMatrix4fv(uModelViewMatrix,1, GL_FALSE, &modelViewMat[0][0]);
        glUniformMatrix4fv(uModelViewProjMatrix,1, GL_FALSE, &modelViewProjMat[0][0]);
        glUniformMatrix4fv(uNormalMatrix,1, GL_FALSE, &normalMat[0][0]);

        glBindTexture(GL_TEXTURE_2D, metalTexture);

        glBindVertexArray(sphereVAO);
        glDrawElements(GL_TRIANGLES, sphere.indexBuffer.size(), GL_UNSIGNED_INT, nullptr);

        glBindTexture(GL_TEXTURE_2D, 0);


        modelViewMat = viewMat * sceneModelMat;
        modelViewProjMat = projectionMat * modelViewMat;
        normalMat = glm::transpose(glm::inverse(modelViewMat));
        glUniformMatrix4fv(uModelViewMatrix,1, GL_FALSE, &modelViewMat[0][0]);
        glUniformMatrix4fv(uModelViewProjMatrix,1, GL_FALSE, &modelViewProjMat[0][0]);
        glUniformMatrix4fv(uNormalMatrix,1, GL_FALSE, &normalMat[0][0]);

        glProgramUniform1i(program.glId(),uUseTexture, 0);

        glBindVertexArray(sceneVAO);
        uint32_t indexOffset = 0, indexCount;

        if (loadedScene.materialCount)
        {
            for (int i=0; i<loadedScene.shapeCount; i++)
            {
                glmlv::ObjData::PhongMaterial& mat=
                        loadedScene.materials[loadedScene.materialIDPerShape[i]];
                if (mat.KdTextureId>=0)
                {
                    glBindTexture(GL_TEXTURE_2D, sceneTextures[mat.KdTextureId]);
                    glProgramUniform1i(program.glId(),uUseTexture, 1);
                }
                else
                {
                    glProgramUniform1i(program.glId(),uUseTexture, 0);
                }
                indexCount = loadedScene.indexCountPerShape[i];
                glProgramUniform3f(program.glId(),uKd, mat.Kd.r,mat.Kd.g,mat.Kd.b);
                glProgramUniform3f(program.glId(),uKs, mat.Ks.r,mat.Ks.g,mat.Ks.b);
                glProgramUniform3f(program.glId(),uKa, mat.Ka.r,mat.Ka.g,mat.Ka.b);
                glProgramUniform1f(program.glId(),uShininess, mat.shininess);
                glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, (const GLvoid*) (indexOffset * sizeof(GLuint)));
                indexOffset += indexCount;
            }
        }
        else
        {
            glm::vec3 Kd(0.8f,0.8f,0.8f);
            glProgramUniform3f(program.glId(),uKd, Kd.r,Kd.g,Kd.b);
            glDrawElements(GL_TRIANGLES, loadedScene.indexBuffer.size(), GL_UNSIGNED_INT, 0);
        }


        glBindVertexArray(0);
        //
        //
        //

        // GUI code:
        ImGui_ImplGlfwGL3_NewFrame();

        {
            ImGui::Begin("GUI");
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::ColorEditMode(ImGuiColorEditMode_RGB);
            if (ImGui::ColorEdit3("clearColor", clearColor)) {
                glClearColor(clearColor[0], clearColor[1], clearColor[2], 1.f);
            }
            ImGui::NewLine();
            ImGui::BeginChild("Directional Light", ImVec2(ImGui::GetWindowContentRegionWidth(),100));
            ImGui::Text("Directional Light");
            ImGui::ColorEdit3("Color", dirLightColor);
            ImGui::DragFloat3("Direction", dirLightDirection,1.0f,-50.f,50.f);
            ImGui::EndChild();

            ImGui::BeginChild("Point Light", ImVec2(ImGui::GetWindowContentRegionWidth(),100));
            ImGui::Text("Point Light");
            ImGui::ColorEdit3("Color", pointLightColor);
            ImGui::DragFloat3("Position", pointLightPos,1.0f,-50.f,50.f);
            ImGui::EndChild();

            ImGui::BeginChild("Ambiant Light", ImVec2(ImGui::GetWindowContentRegionWidth(),100));
            ImGui::Text("Ambiant Light");
            ImGui::ColorEdit3("Color", ambiantLightColor);
            ImGui::EndChild();

            ImGui::End();
        }

        const auto viewportSize = m_GLFWHandle.framebufferSize();
        glViewport(0, 0, viewportSize.x, viewportSize.y);
        ImGui::Render();

        /* Poll for and process events */
        glfwPollEvents();

        /* Swap front and back buffers*/
        m_GLFWHandle.swapBuffers();

        auto ellapsedTime = glfwGetTime() - seconds;
        auto guiHasFocus = ImGui::GetIO().WantCaptureMouse || ImGui::GetIO().WantCaptureKeyboard;
        if (!guiHasFocus) {
            camera.update(float(ellapsedTime));
        }
    }

    return 0;
}

void Application::loadImage(std::string filename, GLuint& textureID)
{
    auto image = glmlv::readImage(m_AssetsRootPath / m_AppName / "textures" / filename);
    loadImage(image,textureID);
}

void Application::loadImage(const glmlv::Image2DRGBA& image, GLuint& textureID)
{
    glActiveTexture(GL_TEXTURE0);

    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB32F, image.width(), image.height());
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, image.width(), image.height(), GL_RGBA, GL_UNSIGNED_BYTE, image.data());
    glBindTexture(GL_TEXTURE_2D, 0);
}

Application::Application(int argc, char** argv):
    m_AppPath { glmlv::fs::path{ argv[0] } },
    m_AppName { m_AppPath.stem().string() },
    m_ImGuiIniFilename { m_AppName + ".imgui.ini" },
    m_ShadersRootPath { m_AppPath.parent_path() / "shaders" },
    m_AssetsRootPath { m_AppPath.parent_path() / "assets" },
    camera(m_GLFWHandle.window(),0),
    cube(glmlv::makeCube()),
    sphere(glmlv::makeSphere(10))
{
    ImGui::GetIO().IniFilename = m_ImGuiIniFilename.c_str(); // At exit, ImGUI will store its windows positions in this file

    glmlv::loadObj(m_AssetsRootPath / m_AppName / "models/crytek-sponza/sponza.obj",loadedScene, true);
    camera.setSpeed(glm::length(loadedScene.bboxMax - loadedScene.bboxMin) * 0.1f);

    // VBO Init (vertex buffer)
    glGenBuffers(1, &cubeVBO);
    glGenBuffers(1, &sphereVBO);
    glGenBuffers(1, &sceneVBO);

    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferStorage(GL_ARRAY_BUFFER, cube.vertexBuffer.size()*sizeof(glmlv::Vertex3f3f2f), cube.vertexBuffer.data(), 0);
    glBindBuffer(GL_ARRAY_BUFFER, sphereVBO);
    glBufferStorage(GL_ARRAY_BUFFER, sphere.vertexBuffer.size()*sizeof(glmlv::Vertex3f3f2f), sphere.vertexBuffer.data(), 0);
    glBindBuffer(GL_ARRAY_BUFFER, sceneVBO);
    glBufferStorage(GL_ARRAY_BUFFER, loadedScene.vertexBuffer.size()*sizeof(glmlv::Vertex3f3f2f), loadedScene.vertexBuffer.data(), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // IBO Init (indice Buffer)
    glGenBuffers(1, &cubeIBO);
    glGenBuffers(1, &sphereIBO);
    glGenBuffers(1, &sceneIBO);

    glBindBuffer(GL_ARRAY_BUFFER, cubeIBO);
    glBufferStorage(GL_ARRAY_BUFFER, sizeof(uint32_t)*cube.indexBuffer.size(), cube.indexBuffer.data(), 0);
    glBindBuffer(GL_ARRAY_BUFFER, sphereIBO);
    glBufferStorage(GL_ARRAY_BUFFER, sizeof(uint32_t)*sphere.indexBuffer.size(), sphere.indexBuffer.data(), 0);
    glBindBuffer(GL_ARRAY_BUFFER, sceneIBO);
    glBufferStorage(GL_ARRAY_BUFFER, sizeof(uint32_t)*loadedScene.indexBuffer.size(), loadedScene.indexBuffer.data(), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenVertexArrays(1, &cubeVAO);
    glGenVertexArrays(1, &sphereVAO);
    glGenVertexArrays(1, &sceneVAO);

    // Here we load and compile shaders from the library
    program = glmlv::compileProgram({ m_ShadersRootPath / m_AppName / "forward.vs.glsl", m_ShadersRootPath / m_AppName / "forward.fs.glsl" });

    // Here we use glGetAttribLocation(program, attribname) to obtain attrib locations; We could also directly use locations if they are set in the vertex shader (cf. triangle app)
    const GLint positionAttrLocation = 0;//glGetAttribLocation(program.glId(), "aPosition");
    const GLint normalAttrLocation = 1;//glGetAttribLocation(program.glId(), "aNormal");
    const GLint texCoordsAttrLocation = 2;//glGetAttribLocation(program.glId(), "aTexCoords");

    uModelViewProjMatrix = glGetUniformLocation(program.glId(), "uModelViewProjMatrix");
    uModelViewMatrix = glGetUniformLocation(program.glId(), "uModelViewMatrix");
    uNormalMatrix = glGetUniformLocation(program.glId(), "uNormalMatrix");

    uDirectionalLightDir = glGetUniformLocation(program.glId(), "uDirectionalLightDir");
    uDirectionalLightIntensity = glGetUniformLocation(program.glId(), "uDirectionalLightIntensity");

    uPointLightPosition = glGetUniformLocation(program.glId(), "uPointLightPosition");
    uPointLightIntensity = glGetUniformLocation(program.glId(), "uPointLightIntensity");

    uAmbiantLightIntensity = glGetUniformLocation(program.glId(), "uAmbiantLightIntensity");

    uKd = glGetUniformLocation(program.glId(), "uKd");
    uKs = glGetUniformLocation(program.glId(), "uKs");
    uKa = glGetUniformLocation(program.glId(), "uKa");
    uShininess = glGetUniformLocation(program.glId(), "uShininess");

    uUseTexture = glGetUniformLocation(program.glId(), "uUseTexture");
    uKdSampler = glGetUniformLocation(program.glId(), "uKdSampler");

    glBindVertexArray(cubeVAO);

    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);

    glEnableVertexAttribArray(positionAttrLocation);
    glVertexAttribPointer(positionAttrLocation, 3, GL_FLOAT, GL_FALSE, sizeof(glmlv::Vertex3f3f2f), (const GLvoid*) offsetof(glmlv::Vertex3f3f2f, position));

    glEnableVertexAttribArray(normalAttrLocation);
    glVertexAttribPointer(normalAttrLocation, 3, GL_FLOAT, GL_FALSE, sizeof(glmlv::Vertex3f3f2f), (const GLvoid*) offsetof(glmlv::Vertex3f3f2f, normal));

    glEnableVertexAttribArray(texCoordsAttrLocation);
    glVertexAttribPointer(texCoordsAttrLocation, 2, GL_FLOAT, GL_FALSE, sizeof(glmlv::Vertex3f3f2f), (const GLvoid*) offsetof(glmlv::Vertex3f3f2f, texCoords));

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeIBO);


    glBindVertexArray(sphereVAO);

    glBindBuffer(GL_ARRAY_BUFFER, sphereVBO);

    glEnableVertexAttribArray(positionAttrLocation);
    glVertexAttribPointer(positionAttrLocation, 3, GL_FLOAT, GL_FALSE, sizeof(glmlv::Vertex3f3f2f), (const GLvoid*) offsetof(glmlv::Vertex3f3f2f, position));

    glEnableVertexAttribArray(normalAttrLocation);
    glVertexAttribPointer(normalAttrLocation, 3, GL_FLOAT, GL_FALSE, sizeof(glmlv::Vertex3f3f2f), (const GLvoid*) offsetof(glmlv::Vertex3f3f2f, normal));

    glEnableVertexAttribArray(texCoordsAttrLocation);
    glVertexAttribPointer(texCoordsAttrLocation, 2, GL_FLOAT, GL_FALSE, sizeof(glmlv::Vertex3f3f2f), (const GLvoid*) offsetof(glmlv::Vertex3f3f2f, texCoords));

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphereIBO);


    glBindVertexArray(sceneVAO);

    glBindBuffer(GL_ARRAY_BUFFER, sceneVBO);

    glEnableVertexAttribArray(positionAttrLocation);
    glVertexAttribPointer(positionAttrLocation, 3, GL_FLOAT, GL_FALSE, sizeof(glmlv::Vertex3f3f2f), (const GLvoid*) offsetof(glmlv::Vertex3f3f2f, position));

    glEnableVertexAttribArray(normalAttrLocation);
    glVertexAttribPointer(normalAttrLocation, 3, GL_FLOAT, GL_FALSE, sizeof(glmlv::Vertex3f3f2f), (const GLvoid*) offsetof(glmlv::Vertex3f3f2f, normal));

    glEnableVertexAttribArray(texCoordsAttrLocation);
    glVertexAttribPointer(texCoordsAttrLocation, 2, GL_FLOAT, GL_FALSE, sizeof(glmlv::Vertex3f3f2f), (const GLvoid*) offsetof(glmlv::Vertex3f3f2f, texCoords));

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sceneIBO);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);

    loadImage("sample_metalTexture.jpg", metalTexture);
    loadImage("sample_woodTexture.jpg", woodTexture);
    sceneTextures.resize(loadedScene.textures.size(),0);
    for (int i=0; i<sceneTextures.size(); ++i)
    {
        loadImage(loadedScene.textures[i], sceneTextures[i]);
    }

    // Note: no need to bind a sampler for modifying it: the sampler API is already direct_state_access
    glGenSamplers(1, &samplerObject);
    glSamplerParameteri(samplerObject, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glSamplerParameteri(samplerObject, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glSamplerParameteri(samplerObject, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glSamplerParameteri(samplerObject, GL_TEXTURE_WRAP_T, GL_REPEAT);

    program.use();
}

Application::~Application()
{
    if (cubeIBO) {
        glDeleteBuffers(1, &cubeIBO);
    }
    if (cubeVBO) {
        glDeleteBuffers(1, &cubeVBO);
    }
    if (cubeVAO) {
        glDeleteBuffers(1, &cubeVAO);
    }

    if (sphereIBO) {
        glDeleteBuffers(1, &sphereIBO);
    }
    if (sphereVBO) {
        glDeleteBuffers(1, &sphereVBO);
    }
    if (sphereVAO) {
        glDeleteBuffers(1, &sphereVAO);
    }

    if (sceneIBO) {
        glDeleteBuffers(1, &sceneIBO);
    }
    if (sceneVBO) {
        glDeleteBuffers(1, &sceneVBO);
    }
    if (sceneVAO) {
        glDeleteBuffers(1, &sceneVAO);
    }

    ImGui_ImplGlfwGL3_Shutdown();
    glfwTerminate();
}
