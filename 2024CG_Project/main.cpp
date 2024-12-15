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
#include "Enemy.h"
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
Model* enemyModel;
Model* currModel;

Player* player;
Enemy* enemy;

Animator* animator;
Animator* animator2;

Animation* idleAnim;
Animation* runAnim;
Animation* jumpAnim;
Animation* attackAnim;
Animation* getHitAnim;

Animation* zombieRunAnim;
Animation* zombieAttackAnim;
Animation* zombieGetHitAnim;

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

// 충돌 검사
bool CheckCollision(glm::vec3 posA, glm::vec3 posB, float radiusA, float radiusB) {
	float distance = glm::distance(posA, posB);
	return distance < (radiusA + radiusB); // 두 반지름의 합보다 거리가 짧으면 충돌
}

// 플레이어와 적 간 충돌 검사
void CheckPlayerEnemyCollision(Player* player, Enemy* enemy) {
	GLfloat* playerCurrPos = player->GetModel()->GetTranslate();
	GLfloat* enemyCurrPos = enemy->GetModel()->GetTranslate();

	glm::vec3 playerPos(playerCurrPos[0], playerCurrPos[1], playerCurrPos[2]);
	glm::vec3 enemyPos(enemyCurrPos[0], enemyCurrPos[1], enemyCurrPos[2]);

	float playerRadius = 0.5f; // 플레이어의 반지름 (가정)
	float enemyRadius = 0.5f;  // 적의 반지름 (가정)

	if (CheckCollision(playerPos, enemyPos, playerRadius, enemyRadius)) {
		std::cout << "Player and Enemy collided!" << std::endl;
		// 충돌 후의 로직 (예: 플레이어 체력 감소 등)을 추가
		player->DecreaseHealth(10); // 체력을 감소시킴
	}
	else {
		std::cout << "Player and Enemy; not colided" << std::endl;
	}
}

void RenderPlayerHealth(Player* player) {
	int health = player->GetHealth();
	int maxHealth = player->GetMaxHealth();

	ImGui::Begin("Player Health");
	ImGui::Text("Health: %d / %d", health, maxHealth);

	// 체력을 프로그래스 바 형태로 렌더링
	float healthPercentage = (float)health / (float)maxHealth;
	ImGui::ProgressBar(healthPercentage, ImVec2(200, 20));
	ImGui::End();
}

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
	enemyModel = new Model();

	//std::string modelPath = "world/map.gltf";
	std::string modelPath = "knight/knight.gltf";
	std::string zombieModelPath = "zombie/zombie_walk.gltf";
	mainModel->LoadModel(modelPath);
	enemyModel->LoadModel(zombieModelPath);

	entityList.push_back(mainModel);
	entityList.push_back(enemyModel);
	currModel = mainModel;

	// Player
	player = new Player(mainModel);
	enemy = new Enemy(enemyModel);

	// Camera
	freeCamera = new FreeCamera(glm::vec3(0.f, 0.f, 5.f), 10.f, 0.3f);
	playerCamera = new PlayerCamera(player);
	currCamera = freeCamera;

	// Animation
	/*
	idleAnim = new Animation("Animations/Idle.gltf", currModel);
	runAnim = new Animation("Animations/SlowRun.gltf", currModel);
	*/
	idleAnim = new Animation("Animations/knight_idle.gltf", currModel);
	// runAnim = new Animation("Animations/walkinplace.gltf", currModel);
	runAnim = new Animation("Animations/knight_run.gltf", currModel);
	jumpAnim = new Animation("Animations/jump.gltf", currModel);
	// attackAnim = new Animation("Animations/attack1.gltf", currModel);
	attackAnim = new Animation("Animations/attack2.gltf", currModel);
	getHitAnim = new Animation("Animations/knight_get_hit.gltf", currModel);

	zombieRunAnim = new Animation("Animations/zombie_walk.gltf", enemyModel);
	zombieAttackAnim = new Animation("Animations/zombie_attack.gltf", enemyModel);
	zombieGetHitAnim = new Animation("Animations/zombie_get_hit.gltf", enemyModel);

	// Animator
	animator = new Animator(nullptr);
	// TODO : 
	animator2 = new Animator(zombieRunAnim);

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
			// player
			{
				player->HandleInput(mainWindow->GetKeys(), deltaTime);
				const bool* keys = mainWindow->GetKeys();
				if (keys[GLFW_KEY_K]) {		// 공격한경우
					enemy->isGetHit = player->attack(enemy->model->GetTranslate());
				}

				bool isMoving = player->Move(deltaTime, terrain);

				// 점프 상태에 따라 애니메이션 전환
				Animation* targetAnim = nullptr;

				if (player->isGetHit) {
					targetAnim = getHitAnim;  // 공격 받는 애니메이션
				}
				else if (player->isAttack) {
					targetAnim = attackAnim;  // 공격 애니메이션
				}
				else if (player->isJumping) {
					targetAnim = jumpAnim;  // 점프 애니메이션
				}
				else if (isMoving) {
					targetAnim = runAnim;   // 달리기 애니메이션
				}
				else {
					targetAnim = idleAnim;  // 대기 애니메이션
				}

				// 현재 애니메이션과 목표 애니메이션이 다를 경우에만 변경
				if (animator->GetCurrAnimation() != targetAnim) {
					animator->PlayAnimation(targetAnim);
					enemy->isGetHit = false;
					player->isAttack = false;
				}

				// 애니메이션이 한번 진행되면 최소 1번 출력 보장
				if (animator->IsAnimationFinished(20.f)) {
					if (player->isAttack) {
						player->isAttack = false;
					}
					player->isJumping = false;
					player->isGetHit = false;
				}
			}

			// 플레이어와 적 간 충돌 검사
			CheckPlayerEnemyCollision(player, enemy);

			// enemy
			{
				enemy->Move(deltaTime, terrain, player);

				Animation* targetAnim = nullptr;

				if (enemy->isGetHit) {
					targetAnim = zombieGetHitAnim;  // 공격 받는 애니메이션
				}
				else if (enemy->isAttack) {
					targetAnim = zombieAttackAnim;  // 공격 애니메이션
				}
				else if (enemy->isMoving) {
					targetAnim = zombieRunAnim;
				}

				// 현재 애니메이션과 목표 애니메이션이 다를 경우에만 변경
				if (animator2->GetCurrAnimation() != targetAnim) {
					animator2->PlayAnimation(targetAnim);
					player->isGetHit = false;
					enemy->isAttack = false;
				}

				// 애니메이션이 한번 진행되면 최소 1번 출력 보장
				if (animator2->IsAnimationFinished(500.f) && enemy->isAttack) {
					enemy->isAttack = false;
					player->isGetHit = true;
					enemy->isGetHit = false;
				}
			}

		}

		animator->UpdateAnimation(deltaTime);
		animator2->UpdateAnimation(deltaTime);
		// ----------------------------------------

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		ImGui::DockSpaceOverViewport();

		// Render Player Health
		// RenderPlayerHealth(player);

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
			Model* enemysModel = enemyModel;

			{
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
				modelShader->UseMaterial(enemyModel->GetMaterial());

				const auto& transforms = animator->GetFinalBoneMatrices();
				modelShader->UseFinalBoneMatrices(transforms);

				glUniform1i(loc_diffuseSampler, 0);
				glUniform1i(loc_normalSampler, 1);

				mainModel->RenderModel();
			}
			{
				glm::mat4 modelMat = enemysModel->GetModelMat();
				glm::mat4 PVM = projMat * viewMat * modelMat;
				glm::mat3 normalMat = GetNormalMat(modelMat);
				glUniformMatrix4fv(loc_modelMat, 1, GL_FALSE, glm::value_ptr(modelMat));
				glUniformMatrix4fv(loc_PVM, 1, GL_FALSE, glm::value_ptr(PVM));
				glUniformMatrix3fv(loc_normalMat, 1, GL_FALSE, glm::value_ptr(normalMat));

				modelShader->UseEyePos(camPos);
				modelShader->UseDirectionalLight(directionalLight);
				modelShader->UsePointLights(pointLights, pointLightCount);

				modelShader->UseMaterial(mainModel->GetMaterial());
				modelShader->UseMaterial(enemyModel->GetMaterial());

				const auto& transforms = animator2->GetFinalBoneMatrices();

				modelShader->UseFinalBoneMatrices(transforms);

				glUniform1i(loc_diffuseSampler, 0);
				glUniform1i(loc_normalSampler, 1);

				enemyModel->RenderModel();
			}
			


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