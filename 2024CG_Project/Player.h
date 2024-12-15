#pragma once

#include "glm/glm.hpp"
class Model;
class Terrain;

class Player
{
public:
	Player(Model* model);
	bool isJumping;
	bool isAttack;
	bool isGetHit;

	void HandleInput(bool* keys, float deltaTime);
	bool Move(float deltaTime, Terrain* terrain);
	float GetRotY();
	void Jump();
	bool attack(float * enmey);
	bool canMove();
	
	void DecreaseHealth(int amount); // ü���� ����
	void IncreaseHealth(int amount); // ü���� ȸ��
	int GetHealth() const; // ���� ü���� ��ȯ
	int GetMaxHealth() const; // �ִ� ü���� ��ȯ

	Model* GetModel() { return model; }

private:
	

	Model* model;

	const float MOVE_SPEED;
	const float TURN_SPEED;
	const float GRAVITY;
	const float JUMP_POWER;

	float currMoveSpeed;
	float currTurnSpeed;
	float upwardSpeed;
	float groundHeight;
	float attackRange;					// ���� ��Ÿ�

	int health;	// �÷��̾��� ���� ü��
	int maxHealth;	// �÷��̾��� �ִ� ü��

	// bool isJumping;
};
