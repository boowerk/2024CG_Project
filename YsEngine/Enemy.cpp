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
	currMoveSpeed = MOVE_SPEED;  // 기본 이동 속도 설정
	currTurnSpeed = 0;           // 초기 회전 속도는 0

    chaseSpeed = MOVE_SPEED;    // 추적 속도 (수정 가능)
    detectionRange = 10.0f;      // 감지 범위 (수정 가능)

	// 랜덤 초기화
	srand(static_cast<unsigned>(time(0)));

    GLfloat* currPos = model->GetTranslate();
    glm::vec3 newPos(currPos[0] + 5, currPos[1], currPos[2]);

    model->SetTranslate(newPos);
}

bool Enemy::Move(float deltaTime, Terrain* terrain, Player* player)
{
    // 랜덤 방향 변경
    timeSinceLastDirChange += deltaTime;
    if (timeSinceLastDirChange >= changeDirectionTime)
    {
        // 새 방향 설정 (랜덤한 회전 속도)
        currTurnSpeed = static_cast<float>(rand() % 360 - 180); // -180 ~ +180도
        timeSinceLastDirChange = 0.f;  // 타이머 초기화
    }

    // 회전
    GLfloat* currRot = model->GetRotate();
    float rotation = currTurnSpeed * deltaTime;

    float newRotY = currRot[1] + rotation;
    if (newRotY > 180)
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

    glm::vec3 newPos(currPos[0] + dx, currPos[1] + upwardSpeed, currPos[2] + dz);

    GLfloat* playerCurrPos = player->GetModel()->GetTranslate();

    glm::vec3 playerPos(playerCurrPos[0], playerCurrPos[1], playerCurrPos[2]);

    float distanceToPlayer = GetDistanceBetween(newPos, playerPos);

    if (distanceToPlayer <= detectionRange) {
        // 플레이어 방향으로 이동
        glm::vec3 direction = glm::normalize(playerPos - newPos);
        glm::vec3 pos = newPos + direction * chaseSpeed * deltaTime;

        // 적의 높이 업데이트 (Terrain 위에 있게)
        float groundHeight = terrain->GetHeight(pos.x, pos.z);
        pos.y = groundHeight;

        model->SetTranslate(pos);

		// 적이 플레이어를 바라보도록 회전
		float angleToPlayer = glm::degrees(atan2(direction.x, direction.z));
		glm::vec3 newRotation = glm::vec3(0.0f, angleToPlayer, 0.0f);
		model->SetRotate(newRotation);
    }


    groundHeight = terrain->GetHeight(currPos[0], currPos[2]);
    if (newPos[1] <= groundHeight) // 땅에 닿았다면
    {
        upwardSpeed = 0;
        newPos[1] = groundHeight;
    }

    model->SetTranslate(newPos);

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
