#pragma once

#include "glm/glm.hpp"
#include "Player.h"
class Model;
class Terrain;

class Enemy
{
public:
	Enemy(Model* model);
	bool isAttack;
	bool isMoving;

	bool Move(float deltaTime, Terrain* terrain, Player* player);
	float GetRotY();
	void attack();
	bool canMove();

	// 적과 플레이어의 거리 함수
	float GetDistanceBetween(glm::vec3 posA, glm::vec3 posB);

	Model* GetModel() { return model; }

private:

	Model* model;

	// Enemy 클래스의 멤버 변수
	float timeSinceLastDirChange = 0.f;  // 방향 변경을 위한 타이머
	float changeDirectionTime = 2.f;    // 방향 변경 주기 (초)

	float chaseSpeed;					// 플레이어를 쫓아갈 때의 속도
	float detectionRange;				// 플레이어를 감지할 수 있는 범위
	float attackRange;					// 공격 사거리

	const float MOVE_SPEED;
	const float TURN_SPEED;
	const float GRAVITY;
	const float JUMP_POWER;

	float currMoveSpeed;
	float currTurnSpeed;
	float upwardSpeed;
	float groundHeight;
};

