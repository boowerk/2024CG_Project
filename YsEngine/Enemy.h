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

	// ���� �÷��̾��� �Ÿ� �Լ�
	float GetDistanceBetween(glm::vec3 posA, glm::vec3 posB);

	Model* GetModel() { return model; }

private:

	Model* model;

	// Enemy Ŭ������ ��� ����
	float timeSinceLastDirChange = 0.f;  // ���� ������ ���� Ÿ�̸�
	float changeDirectionTime = 2.f;    // ���� ���� �ֱ� (��)

	float chaseSpeed;					// �÷��̾ �Ѿư� ���� �ӵ�
	float detectionRange;				// �÷��̾ ������ �� �ִ� ����
	float attackRange;					// ���� ��Ÿ�

	const float MOVE_SPEED;
	const float TURN_SPEED;
	const float GRAVITY;
	const float JUMP_POWER;

	float currMoveSpeed;
	float currTurnSpeed;
	float upwardSpeed;
	float groundHeight;
};

