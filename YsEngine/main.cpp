#define STB_IMAGE_IMPLEMENTATION
#include <iostream>
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>

#include <assimp/Importer.hpp>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "ImGuizmo.h"

#include "Mesh.h"
#include "Shader.h"
#include "Window.h"
#include "Model.h"
#include "Light.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "Texture.h"
#include "Material.h"
#include "FrameBuffer.h"
#include "ScenePanel.h"
#include "InspectorPanel.h"
#include "Skybox.h"
#include "Animation.h"
#include "Animator.h"
#include "HierarchyPanel.h"
#include "Entity.h"
#include "Terrain.h"
#include "Player.h"
#include "FreeCamera.h"
#include "PlayerCamera.h"

#define WIDTH 1600
#define HEIGHT 900

Window* mainWindow;

CameraBase* currCamera;
FreeCamera* freeCamera;
PlayerCamera* playerCamera;

GLfloat deltaTime = 0.f;
GLfloat lastTime = 0.f;

// Vertex Shader
static const char* vShaderPath = "Shaders/Shader.vert";
// Fragment Shader
static const char* fShaderPath = "Shaders/Shader.frag";

//std::vector<Shader*> shaderList;
Shader* modelShader;
Shader* pointLightShader;
std::vector<Entity*> entityList;

Model* mainModel;
Model* currModel;

Player* player;

Animator* animator;

Animation* idleAnim;
Animation* runAnim;

DirectionalLight* directionalLight;

PointLight* pointLights[MAX_POINT_LIGHTS];
unsigned int pointLightCount = 0;

Skybox* skybox;
Terrain* terrain;

ScenePanel* scenePanel;
HierarchyPanel* hierarchyPanel;

bool isPlayMode;

// 쉐이더 변수 핸들
GLuint loc_modelMat = 0;
GLuint loc_PVM = 0;
GLuint loc_diffuseSampler = 0;
GLuint loc_normalSampler = 0;
GLuint loc_normalMat = 0;
GLuint loc_eyePos = 0;
GLuint loc_finalBonesMatrices = 0;

void CreateShader();
void GetModelShaderHandles();
glm::mat3 GetNormalMat(glm::mat4& modelMat);
void MoveCamera();
void TogglePlayMode();

int main()
{
    // GLFW 초기화
    if (!glfwInit())
    {
        printf("GLFW 초기화 실패\n");
        glfwTerminate();
        return 1;
    }

	isPlayMode = false;

    mainWindow = new Window(WIDTH, HEIGHT);
    mainWindow->Initialize();

	CreateShader();

	// Directional Light
	directionalLight = new DirectionalLight
		(0.5f, 1.f,
		glm::vec4(1.f, 1.f, 1.f, 1.f), 
		glm::vec3(0.f, 1.f, 1.f));
	entityList.push_back(directionalLight);

	// Point Light
	pointLights[0] = new PointLight
		(0.f, 0.5f,
		glm::vec4(1.f, 0.f, 0.f, 1.f),
		glm::vec3(2.f, 1.5f, 0.2f),
		1.0f, 0.22f, 0.20f);
	pointLightCount++;
	pointLights[1] = new PointLight
		(0.0f, 0.5f,
		glm::vec4(0.f, 1.f, 0.f, 1.f),
		glm::vec3(-2.0f, 2.0f, -1.f),
		1.0, 0.045f, 0.0075f);
	pointLightCount++;

	for(int i=0;i<pointLightCount;i++)
		entityList.push_back(pointLights[i]);

	// Skybox
	std::vector<std::string> skyboxFaces;
	skyboxFaces.push_back("Textures/Skybox/px.png");
	skyboxFaces.push_back("Textures/Skybox/nx.png");
	skyboxFaces.push_back("Textures/Skybox/py.png");
	skyboxFaces.push_back("Textures/Skybox/ny.png");
	skyboxFaces.push_back("Textures/Skybox/pz.png");
	skyboxFaces.push_back("Textures/Skybox/nz.png");
	skybox = new Skybox(skyboxFaces);

	// Terrain
	terrain = new Terrain();
	terrain->LoadTerrain("Heightmaps/iceland_heightmap.png");
	entityList.push_back(terrain);

	// Model
	mainModel = new Model();
	std::string modelPath = "devola_-_nier_automata/Rigged.gltf";
	mainModel->LoadModel(modelPath);
	entityList.push_back(mainModel);
	currModel = mainModel;

	// Player
	player = new Player(mainModel);

	// Camera
	freeCamera = new FreeCamera(glm::vec3(0.f, 0.f, 5.f), 10.f, 0.3f);
	playerCamera = new PlayerCamera(player);
	currCamera = freeCamera;

	// Animation
	idleAnim = new Animation("Animations/Idle.gltf", currModel);
	runAnim = new Animation("Animations/SlowRun.gltf", currModel);

	// Animator
	animator = new Animator(nullptr);

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows

	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(mainWindow->GetGLFWwindow(), true); // Second param install_callback=true will install GLFW callbacks and chain to existing ones. 
	ImGui_ImplOpenGL3_Init();

	// Frame Buffer 생성
	FrameBuffer sceneBuffer(mainWindow->getBufferWidth(), mainWindow->getBufferHeight());
	mainWindow->SetSceneBuffer(&sceneBuffer);

	// Panel 생성
	scenePanel = new ScenePanel(&sceneBuffer, currCamera, mainWindow);
	hierarchyPanel = new HierarchyPanel(entityList, scenePanel);

    ///////////////////////////////////////////////////////////////////////////
    /// main loop
    //////////////////////////////////////////////////////////////////////////
	while (!mainWindow->GetShouldClose())
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		lastTime = now;

		// ---------------------------------------
		glfwPollEvents();

		if (currCamera->CanMove())
			MoveCamera();
		currCamera->Update();

		if (isPlayMode)
		{
			player->HandleInput(mainWindow->GetKeys(), deltaTime);
			if (player->Move(deltaTime, terrain))
			{
				if(animator->GetCurrAnimation() != runAnim)
					animator->PlayAnimation(runAnim);
			}
			else
			{
				if (animator->GetCurrAnimation() != idleAnim)
					animator->PlayAnimation(idleAnim);
			}
		}

		animator->UpdateAnimation(deltaTime);
		// ----------------------------------------

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		ImGui::DockSpaceOverViewport();

		// Clear the window
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// --------------------------------------------------------------------------------
		// Clear the framebuffer
		sceneBuffer.Bind();
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// --------------------------------------------------------------------------------

		glm::mat4 viewMat = currCamera->GetViewMatrix();
		glm::mat4 projMat = currCamera->GetProjectionMatrix(scenePanel->GetWidth(), scenePanel->GetHeight());

		glm::vec3 camPos = currCamera->GetPosition();

		skybox->DrawSkybox(viewMat, projMat);

		terrain->UseShader();
		terrain->GetShader()->UseDirectionalLight(directionalLight);
		terrain->GetShader()->UsePointLights(pointLights, pointLightCount);
		terrain->GetShader()->UseEyePos(camPos);
		terrain->DrawTerrain(viewMat, projMat);

		pointLightShader->UseShader();
		for (int i = 0; i < pointLightCount; i++)
			pointLights[i]->DrawPointLight(viewMat, projMat, pointLightShader);

		modelShader->UseShader();
		{
			GetModelShaderHandles();

			Model* currModel = mainModel;

			glm::mat4 modelMat = currModel->GetModelMat();
			glm::mat4 PVM = projMat * viewMat * modelMat;
			glm::mat3 normalMat = GetNormalMat(modelMat);
			glUniformMatrix4fv(loc_modelMat, 1, GL_FALSE, glm::value_ptr(modelMat));
			glUniformMatrix4fv(loc_PVM, 1, GL_FALSE, glm::value_ptr(PVM));
			glUniformMatrix3fv(loc_normalMat, 1, GL_FALSE, glm::value_ptr(normalMat));

			modelShader->UseEyePos(camPos);
			modelShader->UseDirectionalLight(directionalLight);
			modelShader->UsePointLights(pointLights, pointLightCount);
			
			modelShader->UseMaterial(mainModel->GetMaterial());

			const auto& transforms = animator->GetFinalBoneMatrices();
			modelShader->UseFinalBoneMatrices(transforms);

			glUniform1i(loc_diffuseSampler, 0);
			glUniform1i(loc_normalSampler, 1);

			mainModel->RenderModel();

			GLenum error = glGetError();
			if (error != GL_NO_ERROR)
				std::cout << "error : " << error << std::endl;
		}

		glUseProgram(0);

		// --------------------------------------------------------------------------------
		sceneBuffer.Unbind();
		// --------------------------------------------------------------------------------

		scenePanel->Update();
		hierarchyPanel->UpdateEntityList(entityList);
		hierarchyPanel->Update();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		// Update and Render additional Platform Windows
		// (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
		//  For this specific demo app we could also call glfwMakeContextCurrent(window) directly)
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			GLFWwindow* backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);
		}

		mainWindow->swapBuffers();
	}

    return 0;
}

void CreateShader()
{
	// shader setup

	modelShader = new Shader;
	modelShader->CreateFromFiles(vShaderPath, fShaderPath);

	pointLightShader = new Shader();
	pointLightShader->CreateFromFiles("Shaders/PointLight.vert", "Shaders/PointLight.frag");
}

void GetModelShaderHandles()
{
	// 핸들 얻어오기
	loc_modelMat = modelShader->GetModelMatLoc();
	loc_PVM = modelShader->GetPVMLoc();
	loc_normalMat = modelShader->GetNormalMatLoc();
	loc_eyePos = modelShader->GetEyePosLoc();
	loc_finalBonesMatrices = modelShader->GetFinalBonesMatricesLoc();
	loc_diffuseSampler = modelShader->GetColorSamplerLoc();
	loc_normalSampler = modelShader->GetNormalSamplerLoc();
}

glm::mat3 GetNormalMat(glm::mat4& modelMat)
{
	return glm::mat3(glm::transpose(glm::inverse(modelMat)));
}

void MoveCamera()
{
	currCamera->KeyControl(mainWindow->GetKeys(), deltaTime);
	currCamera->MouseControl(mainWindow->getXChange(), mainWindow->getYChange());
	currCamera->ScrollControl(mainWindow->GetScrollYChange());
}

void TogglePlayMode()
{

	// 플래그 변경
	isPlayMode = !isPlayMode;

	// 카메라 변경
	if(isPlayMode)
		currCamera = playerCamera;
	else
	{
		GLfloat* mp = mainModel->GetTranslate();
		freeCamera->SetPosition(glm::vec3(mp[0], mp[1], mp[2]));
		currCamera = freeCamera;
	}
	scenePanel->SetCamera(currCamera);

	scenePanel->SetIsPlayMode(isPlayMode);

	if (isPlayMode)
	{
		animator->PlayAnimation(idleAnim);
		mainModel->SetRotate(glm::vec3(0.f, 0.f, 0.f));
	}
	else
	{
		animator->Reset();
		mainModel->SetRotate(glm::vec3(-90.f, 0.f, 0.f));
	}
}