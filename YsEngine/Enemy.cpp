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
	currMoveSpeed = MOVE_SPEED;  // 기본 이동 속도 설정
	currTurnSpeed = 0;           // 초기 회전 속도는 0

    chaseSpeed = MOVE_SPEED;    // 추적 속도 (수정 가능)
    detectionRange = 20.0f;      // 감지 범위 (수정 가능)
    attackRange = 1.3f;          // 공격 사거리

	// 랜덤 초기화
	srand(static_cast<unsigned>(time(0)));

    GLfloat* currPos = model->GetTranslate();
    glm::vec3 newPos(currPos[0] + 5, currPos[1], currPos[2]);

    model->SetTranslate(newPos);
}

bool Enemy::Move(float deltaTime, Terrain* terrain, Player* player)
{
    isMoving = true;

    // 현재 적의 위치와 회전 상태 가져오기
    GLfloat* currPos = model->GetTranslate();
    GLfloat* currRot = model->GetRotate();
    glm::vec3 enemyPos(currPos[0], currPos[1], currPos[2]);
    glm::vec3 enemyRot(currRot[0], currRot[1], currRot[2]);

    // 플레이어 위치 가져오기
    GLfloat* playerCurrPos = player->GetModel()->GetTranslate();
    glm::vec3 playerPos(playerCurrPos[0], playerCurrPos[1], playerCurrPos[2]);

    // 플레이어와의 거리 계산
    float distanceToPlayer = GetDistanceBetween(enemyPos, playerPos);

    // 적이 플레이어와의 특정 거리 이상일 때만 이동
    if (distanceToPlayer > attackRange && distanceToPlayer <= detectionRange)
    {
        // **플레이어 추적 로직**
        glm::vec3 direction = glm::normalize(playerPos - enemyPos); // 방향 계산
        glm::vec3 newPos = enemyPos + direction * chaseSpeed * deltaTime;

        // Terrain 높이 보정
        float groundHeight = terrain->GetHeight(newPos.x, newPos.z);
        newPos.y = groundHeight;

        model->SetTranslate(newPos);

        // 적이 플레이어를 바라보도록 회전
        float angleToPlayer = glm::degrees(atan2(direction.x, direction.z));
        glm::vec3 newRotation(0.0f, angleToPlayer, 0.0f);
        model->SetRotate(newRotation);
    }
    else if (distanceToPlayer <= attackRange)
    {
        // 멈춘 상태, (공격 애니메이션 출력)
        currMoveSpeed = 0.0f; // 멈춤
        isMoving = false;
        attack();
    }
    else
    {
        isMoving = true;
        // **랜덤 이동 로직**
        timeSinceLastDirChange += deltaTime;
        if (timeSinceLastDirChange >= changeDirectionTime)
        {
            currTurnSpeed = static_cast<float>(rand() % 360 - 180); // -180 ~ +180도
            timeSinceLastDirChange = 0.f;
        }

        // 랜덤 회전
        float rotation = currTurnSpeed * deltaTime;
        float newRotY = enemyRot.y + rotation;
        if (newRotY > 180.f) newRotY -= 360.f;
        if (newRotY < -180.f) newRotY += 360.f;

        enemyRot.y = newRotY;
        model->SetRotate(enemyRot);

        // 랜덤 이동
        float distance = currMoveSpeed * deltaTime;
        float dx = distance * sinf(glm::radians(enemyRot.y));
        float dz = distance * cosf(glm::radians(enemyRot.y));

        upwardSpeed -= GRAVITY * deltaTime;
        glm::vec3 newPos(enemyPos.x + dx, enemyPos.y + upwardSpeed, enemyPos.z + dz);

        // Terrain 높이 보정
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
	// 못 움직이는 경우 추가
	if (isAttack)
		return false;
	return true;
}
