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

	// Enemy Ŭ������ ��� ����
	float timeSinceLastDirChange = 0.f;  // ���� ������ ���� Ÿ�̸�
	float changeDirectionTime = 2.f;    // ���� ���� �ֱ� (��)

	const float MOVE_SPEED;
	const float TURN_SPEED;
	const float GRAVITY;
	const float JUMP_POWER;

	float currMoveSpeed;
	float currTurnSpeed;
	float upwardSpeed;
	float groundHeight;
};

