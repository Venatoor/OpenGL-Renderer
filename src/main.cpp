
#include<iostream>

#include "Window.h"
#include<stb/stb_image.h>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>

#include"imgui.h"
#include"imgui_impl_glfw.h"
#include"imgui_impl_opengl3.h"


#include"Texture.h"
#include"shaderClass.h"
#include"VAO.h"
#include"VBO.h"
#include"EBO.h"
#include"Camera.h"
#include"Model.h"

#include"LightManager.h"
#include "PostProcesser.h"
#include <chrono>

#include "Skybox.h"
#include "Cubemap.h"

#include "Mesh.h"
#include "DirectionalLight.h"
#include "SpotLight.h"
#include "PointLight.h"

#include "BlendManager.h"
#include <algorithm>

#include "ShadowMap.h"
#include "Scene.h"
#include "ShaderLibrary.h"

#include "MeshData.h"
#include "MaterialManager.h"
#include "ForwardRenderer.h"
#include "SceneManager.h"
#include "BasicRenderingScene.h"

//Global Settings
const unsigned int WIDTH = 1900;
const unsigned int HEIGHT = 1080;


void InitializeImGui(GLFWwindow* window);
void ProcessInput(GLFWwindow* window);


int main() {
    Window mainWindow;
    if (!mainWindow.Initialize(WIDTH, HEIGHT, "YAGE")) {
        return -1;
    }


    InitializeImGui(mainWindow.GetNativeWindow());


    // Initialisation du Scene Manager
    auto& sceneManager = SceneManager::Get();

    // Enregistrement de toutes les scènes disponibles
    sceneManager.RegisterScene("Basic Rendering", std::make_unique<BasicRenderingScene>());

	glViewport(0, 0, WIDTH, HEIGHT);

	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //FINALLY - QUERY

    //auto SceneEntities = scene.GetRenderEntities();

    
    auto startTime = std::chrono::high_resolution_clock::now();
    static std::string currentSceneName = sceneManager.GetCurrentScene()->GetName();

    //MAIN LOOP
	while (!mainWindow.ShouldClose()) {


        // TODO : when further refactoring the architecture, this is in the EngineCore

        auto currentTime = std::chrono::high_resolution_clock::now();
        float time = std::chrono::duration<float>(currentTime - startTime).count();
		ProcessInput(mainWindow.GetNativeWindow());
        
        //TODO : this is the camera input ? but it seems i put it out of place

        SceneBase* currentScene = sceneManager.GetCurrentScene();
        if (currentScene) {
            Camera& cam = currentScene->GetCamera();

            cam.Inputs(mainWindow.GetNativeWindow());

            cam.UpdateMatrix(45.0f, 0.1f, 100.0f);
        }
        

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        sceneManager.Update(time);
        sceneManager.Render();


        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Scene Manager");

        ImGui::Text("Current Scene: %s", currentSceneName.c_str());
        ImGui::Separator();

        ImGui::Text("Available Scenes:");
        auto& sceneNames = sceneManager.GetSceneNames();
        for (const auto& name : sceneNames) {
            if (ImGui::Button(name.c_str())) {
                sceneManager.SwitchScene(name);
                currentSceneName = name;
            }
        }

        ImGui::Separator();
        if (ImGui::Button("Reload Current Scene")) {
            sceneManager.ReloadCurrentScene();
        }

        ImGui::Separator();
        ImGui::Text("Application Info:");
        ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
        ImGui::Text("Frame Time: %.3f ms", 1000.0f / ImGui::GetIO().Framerate);

        ImGui::End();

        // UI spécifique à la scène courante
        sceneManager.OnImGUIRender();

        // === Contrôles Globaux (optionnels) ===
        ImGui::Begin("Engine Controls");
        ImGui::Text("Global Settings");

        static bool vsyncEnabled = true;
        if (ImGui::Checkbox("VSync", &vsyncEnabled)) {
            // Implémenter le changement de VSync si nécessaire
        }

        static bool wireframeMode = false;
        if (ImGui::Checkbox("Wireframe Mode", &wireframeMode)) {
            if (wireframeMode) {
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            }
            else {
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            }
        }
        ImGui::End();
        
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        mainWindow.SwapBuffers();
        mainWindow.PollEvents();
	}

	glfwTerminate();
	return 0;
}


void InitializeImGui(GLFWwindow* window)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 460");
}

void ProcessInput(GLFWwindow* window) {

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {

		glfwSetWindowShouldClose(window, true);
	}
}


