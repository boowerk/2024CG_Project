#include "Enemy.h"

#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <cstdlib> // for rand()
#include <ctime>   // for srand()

#include "Model.h"
#include "Terrain.h"

#include "Player.h"

Enemy::Enemy(Model* model) : MOVE_SPEED(1.f), TURN_SPEED(200.f), GRAVITY(0.2f), JUMP_POWER(0.05f)
{
	this->model = model;
	groundHeight = 10;
	upwardSpeed = 0;
	isAttack = false;
    isMoving = false;
	currMoveSpeed = MOVE_SPEED;  // �⺻ �̵� �ӵ� ����
	currTurnSpeed = 0;           // �ʱ� ȸ�� �ӵ��� 0

    chaseSpeed = MOVE_SPEED;    // ���� �ӵ� (���� ����)
    detectionRange = 20.0f;      // ���� ���� (���� ����)
    attackRange = 1.3f;          // ���� ��Ÿ�

	// ���� �ʱ�ȭ
	srand(static_cast<unsigned>(time(0)));

    GLfloat* currPos = model->GetTranslate();
    glm::vec3 newPos(currPos[0] + 5, currPos[1], currPos[2]);

    model->SetTranslate(newPos);
}

bool Enemy::Move(float deltaTime, Terrain* terrain, Player* player)
{
    isMoving = true;

    // ���� ���� ��ġ�� ȸ�� ���� ��������
    GLfloat* currPos = model->GetTranslate();
    GLfloat* currRot = model->GetRotate();
    glm::vec3 enemyPos(currPos[0], currPos[1], currPos[2]);
    glm::vec3 enemyRot(currRot[0], currRot[1], currRot[2]);

    // �÷��̾� ��ġ ��������
    GLfloat* playerCurrPos = player->GetModel()->GetTranslate();
    glm::vec3 playerPos(playerCurrPos[0], playerCurrPos[1], playerCurrPos[2]);

    // �÷��̾���� �Ÿ� ���
    float distanceToPlayer = GetDistanceBetween(enemyPos, playerPos);

    // ���� �÷��̾���� Ư�� �Ÿ� �̻��� ���� �̵�
    if (distanceToPlayer > attackRange && distanceToPlayer <= detectionRange)
    {
        // **�÷��̾� ���� ����**
        glm::vec3 direction = glm::normalize(playerPos - enemyPos); // ���� ���
        glm::vec3 newPos = enemyPos + direction * chaseSpeed * deltaTime;

        // Terrain ���� ����
        float groundHeight = terrain->GetHeight(newPos.x, newPos.z);
        newPos.y = groundHeight;

        model->SetTranslate(newPos);

        // ���� �÷��̾ �ٶ󺸵��� ȸ��
        float angleToPlayer = glm::degrees(atan2(direction.x, direction.z));
        glm::vec3 newRotation(0.0f, angleToPlayer, 0.0f);
        model->SetRotate(newRotation);
    }
    else if (distanceToPlayer <= attackRange)
    {
        // ���� ����, (���� �ִϸ��̼� ���)
        currMoveSpeed = 0.0f; // ����
        isMoving = false;
        attack();
    }
    else
    {
        isMoving = true;
        // **���� �̵� ����**
        timeSinceLastDirChange += deltaTime;
        if (timeSinceLastDirChange >= changeDirectionTime)
        {
            currTurnSpeed = static_cast<float>(rand() % 360 - 180); // -180 ~ +180��
            timeSinceLastDirChange = 0.f;
        }

        // ���� ȸ��
        float rotation = currTurnSpeed * deltaTime;
        float newRotY = enemyRot.y + rotation;
        if (newRotY > 180.f) newRotY -= 360.f;
        if (newRotY < -180.f) newRotY += 360.f;

        enemyRot.y = newRotY;
        model->SetRotate(enemyRot);

        // ���� �̵�
        float distance = currMoveSpeed * deltaTime;
        float dx = distance * sinf(glm::radians(enemyRot.y));
        float dz = distance * cosf(glm::radians(enemyRot.y));

        upwardSpeed -= GRAVITY * deltaTime;
        glm::vec3 newPos(enemyPos.x + dx, enemyPos.y + upwardSpeed, enemyPos.z + dz);

        // Terrain ���� ����
        float groundHeight = terrain->GetHeight(newPos.x, newPos.z);
        if (newPos.y <= groundHeight)
        {
            upwardSpeed = 0;
            newPos.y = groundHeight;
        }

        model->SetTranslate(newPos);
    }

    return currMoveSpeed != 0;
}

float Enemy::GetDistanceBetween(glm::vec3 posA, glm::vec3 posB)
{
    return glm::distance(posA, posB);
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
