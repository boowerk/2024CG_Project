#pragma once

#include "glm/glm.hpp"
class Model;
class Terrain;

class Enemy
{
public:
	Enemy(Model* model);
	bool isAttack;

	bool Move(float deltaTime, Terrain* terrain);
	float GetRotY();
	void attack();
	bool canMove();

	Model* GetModel() { return model; }

private:

	Model* model;

	// Enemy 클래스의 멤버 변수
	float timeSinceLastDirChange = 0.f;  // 방향 변경을 위한 타이머
	float changeDirectionTime = 2.f;    // 방향 변경 주기 (초)

	const float MOVE_SPEED;
	const float TURN_SPEED;
	const float GRAVITY;
	const float JUMP_POWER;

	float currMoveSpeed;
	float currTurnSpeed;
	float upwardSpeed;
	float groundHeight;
};

