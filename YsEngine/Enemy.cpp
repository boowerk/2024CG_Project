#include "Enemy.h"

#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <cstdlib> // for rand()
#include <ctime>   // for srand()

#include "Model.h"
#include "Terrain.h"

Enemy::Enemy(Model* model) : MOVE_SPEED(10.f), TURN_SPEED(200.f), GRAVITY(0.2f), JUMP_POWER(0.05f)
{
	this->model = model;
	groundHeight = 10;
	upwardSpeed = 0;
	isAttack = false;
	currMoveSpeed = MOVE_SPEED;  // �⺻ �̵� �ӵ� ����
	currTurnSpeed = 0;           // �ʱ� ȸ�� �ӵ��� 0

	// ���� �ʱ�ȭ
	srand(static_cast<unsigned>(time(0)));
}

bool Enemy::Move(float deltaTime, Terrain* terrain)
{
    // ���� ���� ����
    timeSinceLastDirChange += deltaTime;
    if (timeSinceLastDirChange >= changeDirectionTime)
    {
        // �� ���� ���� (������ ȸ�� �ӵ�)
        currTurnSpeed = static_cast<float>(rand() % 360 - 180); // -180 ~ +180��
        timeSinceLastDirChange = 0.f;  // Ÿ�̸� �ʱ�ȭ
    }

    // ȸ��
    GLfloat* currRot = model->GetRotate();
    float rotation = currTurnSpeed * deltaTime;

    float newRotY = currRot[1] + rotation;
    if (newRotY > 180)
        newRotY -= 360.f;
    if (newRotY < -180.f)
        newRotY += 360.f;

    glm::vec3 newRot(currRot[0], newRotY, currRot[2]);
    model->SetRotate(newRot);

    // �̵�
    GLfloat* currPos = model->GetTranslate();
    float distance = currMoveSpeed * deltaTime;

    float dx = distance * sinf(glm::radians(newRotY));
    float dz = distance * cosf(glm::radians(newRotY));

    upwardSpeed -= GRAVITY * deltaTime;

    glm::vec3 newPos(currPos[0] + dx, currPos[1] + upwardSpeed, currPos[2] + dz);

    groundHeight = terrain->GetHeight(currPos[0], currPos[2]);
    if (newPos[1] <= groundHeight) // ���� ��Ҵٸ�
    {
        upwardSpeed = 0;
        newPos[1] = groundHeight;
    }

    model->SetTranslate(newPos);

    return currMoveSpeed != 0;
}

float Enemy::GetRotY()
{
	return model->GetRotate()[1];
}

void Enemy::attack()
{
	if (isAttack)
		return;

	isAttack = true;
}

bool Enemy::canMove()
{
	// �� �����̴� ��� �߰�
	if (isAttack)
		return false;
	return true;
}
