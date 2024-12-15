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

	void HandleInput(bool* keys, float deltaTime);
	bool Move(float deltaTime, Terrain* terrain);
	float GetRotY();
	void Jump();
	void attack();
	bool canMove();
	
	void DecreaseHealth(int amount); // 체력을 감소
	void IncreaseHealth(int amount); // 체력을 회복
	int GetHealth() const; // 현재 체력을 반환
	int GetMaxHealth() const; // 최대 체력을 반환

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

	int health;	// 플레이어의 현재 체력
	int maxHealth;	// 플레이어의 최대 체력

	// bool isJumping;
};

