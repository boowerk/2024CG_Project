#include "Player.h"

#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Model.h"
#include "Enemy.h"
#include "Terrain.h"

Player::Player(Model* model) : MOVE_SPEED(10.f), TURN_SPEED(200.f), GRAVITY(0.2f), JUMP_POWER(0.05f)
{
	this->model = model;
	groundHeight = 10;
	upwardSpeed = 0;
	attackRange = 2.f;

	isJumping = false;
	isAttack = false;
	isGetHit = false;

	health = 100; // 초기 체력
	maxHealth = 100;

	GLfloat* currSize = model->GetScale();
	const float sizeOffset = 0.6f;
	glm::vec3 newSize = glm::vec3(currSize[0] * sizeOffset, currSize[1] * sizeOffset, currSize[2] * sizeOffset);
	model->SetScale(newSize);
}

void Player::DecreaseHealth(int amount) {
	health -= amount;
	if (health < 0) health = 0;
}

void Player::IncreaseHealth(int amount) {
	health += amount;
	if (health > maxHealth) health = maxHealth;
}

int Player::GetHealth() const {
	return health;
}

int Player::GetMaxHealth() const {
	return maxHealth;
}

void Player::HandleInput(bool* keys, float deltaTime)
{
	if(canMove()){
		if (keys[GLFW_KEY_W])
			currMoveSpeed = MOVE_SPEED;
		else if (keys[GLFW_KEY_S])
			currMoveSpeed = -MOVE_SPEED;
		else
			currMoveSpeed = 0;

		if (keys[GLFW_KEY_A])
			currTurnSpeed = TURN_SPEED;
		else if (keys[GLFW_KEY_D])
			currTurnSpeed = -TURN_SPEED;
		else
			currTurnSpeed = 0;
	}

	if (keys[GLFW_KEY_SPACE])
		Jump();
	/*if (keys[GLFW_KEY_K])
		attack();*/
}

bool Player::Move(float deltaTime, Terrain* terrain)
{
	// 회전
	GLfloat* currRot = model->GetRotate();

	float rotation = currTurnSpeed * deltaTime;

	float newRotY = currRot[1] + rotation; // new rotY
	if(newRotY > 180)
		newRotY -= 360.f;
	if (newRotY < -180.f)
		newRotY += 360.f;

	glm::vec3 newRot(currRot[0], newRotY, currRot[2]);

	model->SetRotate(newRot);

	// 이동
	GLfloat* currPos = model->GetTranslate();
	float distance = currMoveSpeed * deltaTime;

	float dx = distance * sinf(glm::radians(newRotY));
	float dz = distance * cosf(glm::radians(newRotY));

	upwardSpeed -= GRAVITY * deltaTime;

	glm::vec3 newPos(currPos[0]+dx, currPos[1] + upwardSpeed, currPos[2]+dz);

	groundHeight = terrain->GetHeight(currPos[0], currPos[2]);
	if (newPos[1] <= groundHeight) // 땅에 닿았다면
	{
		upwardSpeed = 0;
		newPos[1] = groundHeight;
		// isJumping = false;
	}

	model->SetTranslate(newPos);

	return currMoveSpeed != 0;
}

float Player::GetRotY()
{
	return model->GetRotate()[1];
}

void Player::Jump()
{
	if(isJumping)
		return;
	
	upwardSpeed = JUMP_POWER;
	isJumping = true;
}

bool Player::attack(float* enmey)
{
	/*if (isAttack)
		return true;*/

	isAttack = true;

	// 플레이어 위치 가져오기
	GLfloat* playerCurrPos = model->GetTranslate();
	glm::vec3 playerPos(playerCurrPos[0], playerCurrPos[1], playerCurrPos[2]);

	// enemy 위치 가져오기
	glm::vec3 enemyPos(enmey[0], enmey[1], enmey[2]);

	// 플레이어와의 거리 계산
	float distanceToEnemy = glm::distance(enemyPos, playerPos);

	// 적이 플레이어와의 공격 사거리 안에 있는 경우
	if (distanceToEnemy < attackRange) {
		std::cout << "Player attack successful" << std::endl;
		return true;
	}
	else {
		std::cout << "Player is out of attack range" << std::endl;
		return false;
	}
}

bool Player::canMove()
{
	// 못 움직이는 경우 추가
	if (isAttack)
		return false;
	return true;
}
