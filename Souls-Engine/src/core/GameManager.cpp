#include "GameManager.h"
#include "../geometry/Cube.h"
#include "../geometry/Sphere.h"
#include "../geometry/Cylinder.h"
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <algorithm>

namespace SoulsEngine {

GameManager::GameManager(ObjectManager* objectManager, Camera* camera)
    : m_objectManager(objectManager)
    , m_camera(camera)
    , m_score(0)
    , m_timeRemaining(60.0f)  // 60秒游戏时间
    , m_gameDuration(60.0f)
    , m_gameOver(false)
    , m_gameWon(false)
    , m_collectibleSpawnTimer(0.0f)
    , m_collectibleSpawnInterval(3.0f)  // 每3秒生成一个收集物
    , m_obstacleSpawnTimer(0.0f)
    , m_obstacleSpawnInterval(5.0f)  // 每5秒生成一个障碍物
    , m_maxCollectibles(10)
    , m_maxObstacles(8)
    , m_playerSpeed(5.0f)
    , m_gen(m_rd())
    , m_arenaMinX(-15.0f)
    , m_arenaMaxX(15.0f)
    , m_arenaMinY(0.5f)
    , m_arenaMaxY(5.0f)
    , m_arenaMinZ(-15.0f)
    , m_arenaMaxZ(15.0f)
{
}

GameManager::~GameManager() {
}

void GameManager::Initialize() {
    // 清空场景
    m_objectManager->Clear();
    m_collectibles.clear();
    m_obstacles.clear();

    // 重置游戏状态
    m_score = 0;
    m_timeRemaining = m_gameDuration;
    m_gameOver = false;
    m_gameWon = false;

    // 创建玩家
    CreatePlayer();

    // 初始生成一些收集物和障碍物
    for (int i = 0; i < 5; ++i) {
        SpawnCollectible();
    }
    for (int i = 0; i < 3; ++i) {
        SpawnObstacle();
    }

    std::cout << "游戏初始化完成！" << std::endl;
    std::cout << "目标：在 " << m_gameDuration << " 秒内收集尽可能多的黄色立方体！" << std::endl;
    std::cout << "小心红色圆柱体障碍物！" << std::endl;
}

void GameManager::Update(float deltaTime) {
    if (m_gameOver || m_gameWon) {
        return;
    }

    // 更新游戏时间
    m_timeRemaining -= deltaTime;
    if (m_timeRemaining <= 0.0f) {
        m_timeRemaining = 0.0f;
        m_gameOver = true;
        std::cout << "游戏结束！最终得分: " << m_score << std::endl;
    }

    // 更新收集物生成
    m_collectibleSpawnTimer += deltaTime;
    if (m_collectibleSpawnTimer >= m_collectibleSpawnInterval && 
        static_cast<int>(m_collectibles.size()) < m_maxCollectibles) {
        SpawnCollectible();
        m_collectibleSpawnTimer = 0.0f;
    }

    // 更新障碍物生成
    m_obstacleSpawnTimer += deltaTime;
    if (m_obstacleSpawnTimer >= m_obstacleSpawnInterval && 
        static_cast<int>(m_obstacles.size()) < m_maxObstacles) {
        SpawnObstacle();
        m_obstacleSpawnTimer = 0.0f;
    }

    // 更新收集物（旋转动画）
    UpdateCollectibles(deltaTime);

    // 更新障碍物（移动）
    UpdateObstacles(deltaTime);

    // 检查碰撞
    CheckCollectibleCollisions();
    CheckObstacleCollisions();

    // 限制玩家在游戏区域内
    if (m_player) {
        glm::vec3 pos = m_player->GetPosition();
        pos.x = glm::clamp(pos.x, m_arenaMinX, m_arenaMaxX);
        pos.y = glm::clamp(pos.y, m_arenaMinY, m_arenaMaxY);
        pos.z = glm::clamp(pos.z, m_arenaMinZ, m_arenaMaxZ);
        m_player->SetPosition(pos);
    }
}

void GameManager::ProcessPlayerInput(float deltaTime, GLFWwindow* window) {
    if (!m_player || m_gameOver || m_gameWon) {
        return;
    }

    glm::vec3 moveDirection(0.0f);

    // 获取相机方向（用于确定移动方向）
    glm::vec3 cameraFront = m_camera->GetFront();
    glm::vec3 cameraRight = m_camera->GetRight();
    
    // 将相机方向投影到XZ平面（水平移动）
    cameraFront.y = 0.0f;
    cameraRight.y = 0.0f;
    cameraFront = glm::normalize(cameraFront);
    cameraRight = glm::normalize(cameraRight);

    // WASD控制
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        moveDirection += cameraFront;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        moveDirection -= cameraFront;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        moveDirection -= cameraRight;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        moveDirection += cameraRight;
    }

    // 归一化移动方向并应用速度
    if (glm::length(moveDirection) > 0.0f) {
        moveDirection = glm::normalize(moveDirection);
        glm::vec3 movement = moveDirection * m_playerSpeed * deltaTime;
        m_player->Translate(movement);
    }
}

void GameManager::CreatePlayer() {
    // 创建玩家球体（绿色）
    auto playerMesh = std::make_shared<Sphere>(0.5f, 36, 18, glm::vec3(0.0f, 1.0f, 0.0f));
    m_player = m_objectManager->CreateNode("Player", playerMesh);
    m_player->SetPosition(0.0f, 1.0f, 0.0f);
    std::cout << "玩家创建完成" << std::endl;
}

void GameManager::SpawnCollectible() {
    // 创建收集物（黄色立方体）
    glm::vec3 pos = GetRandomPosition(m_arenaMinX, m_arenaMaxX, 
                                      m_arenaMinY, m_arenaMaxY, 
                                      m_arenaMinZ, m_arenaMaxZ);
    auto collectibleMesh = std::make_shared<Cube>(0.6f, glm::vec3(1.0f, 1.0f, 0.0f));
    std::string name = "Collectible_" + std::to_string(m_collectibles.size());
    auto collectible = m_objectManager->CreateNode(name, collectibleMesh);
    collectible->SetPosition(pos);
    m_collectibles.push_back(collectible);
}

void GameManager::SpawnObstacle() {
    // 创建障碍物（红色圆柱体）
    glm::vec3 pos = GetRandomPosition(m_arenaMinX, m_arenaMaxX, 
                                      m_arenaMinY, m_arenaMaxY, 
                                      m_arenaMinZ, m_arenaMaxZ);
    auto obstacleMesh = std::make_shared<Cylinder>(0.5f, 1.5f, 36, glm::vec3(1.0f, 0.0f, 0.0f));
    std::string name = "Obstacle_" + std::to_string(m_obstacles.size());
    auto obstacle = m_objectManager->CreateNode(name, obstacleMesh);
    obstacle->SetPosition(pos);
    m_obstacles.push_back(obstacle);
}

void GameManager::UpdateCollectibles(float deltaTime) {
    // 让收集物旋转
    for (auto& collectible : m_collectibles) {
        if (collectible) {
            collectible->RotateY(90.0f * deltaTime);  // 每秒旋转90度
        }
    }
}

void GameManager::UpdateObstacles(float deltaTime) {
    // 让障碍物缓慢移动
    static float globalTime = 0.0f;
    globalTime += deltaTime;
    
    for (size_t i = 0; i < m_obstacles.size(); ++i) {
        auto& obstacle = m_obstacles[i];
        if (obstacle) {
            // 简单的来回移动
            glm::vec3 pos = obstacle->GetPosition();
            
            // 每个障碍物有不同的移动模式
            float speed = 2.0f + (i % 3) * 0.5f;
            float offset = glm::sin(globalTime * speed) * 2.0f * deltaTime;
            
            if (i % 2 == 0) {
                pos.x += offset;
            } else {
                pos.z += offset;
            }
            
            // 限制在区域内
            pos.x = glm::clamp(pos.x, m_arenaMinX, m_arenaMaxX);
            pos.z = glm::clamp(pos.z, m_arenaMinZ, m_arenaMaxZ);
            obstacle->SetPosition(pos);
        }
    }
}

void GameManager::CheckCollectibleCollisions() {
    if (!m_player) return;

    glm::vec3 playerPos = m_player->LocalToWorld(glm::vec3(0.0f, 0.0f, 0.0f));
    float playerRadius = 0.5f;

    auto it = m_collectibles.begin();
    while (it != m_collectibles.end()) {
        auto collectible = *it;
        if (collectible) {
            glm::vec3 collectiblePos = collectible->LocalToWorld(glm::vec3(0.0f, 0.0f, 0.0f));
            float collectibleRadius = 0.4f;

            if (CheckCollision(playerPos, playerRadius, collectiblePos, collectibleRadius)) {
                // 收集成功
                m_score += 10;
                std::cout << "收集成功！得分: " << m_score << std::endl;
                
                // 移除收集物
                m_objectManager->RemoveNode(collectible);
                it = m_collectibles.erase(it);
            } else {
                ++it;
            }
        } else {
            ++it;
        }
    }
}

void GameManager::CheckObstacleCollisions() {
    if (!m_player || m_gameOver) return;

    glm::vec3 playerPos = m_player->LocalToWorld(glm::vec3(0.0f, 0.0f, 0.0f));
    float playerRadius = 0.5f;

    for (auto& obstacle : m_obstacles) {
        if (obstacle) {
            glm::vec3 obstaclePos = obstacle->LocalToWorld(glm::vec3(0.0f, 0.0f, 0.0f));
            float obstacleRadius = 0.6f;

            if (CheckCollision(playerPos, playerRadius, obstaclePos, obstacleRadius)) {
                // 碰撞障碍物，扣分
                m_score = (std::max)(0, m_score - 5);  // 使用括号避免Windows max宏冲突
                std::cout << "撞到障碍物！当前得分: " << m_score << std::endl;
                
                // 将玩家推开
                glm::vec3 pushDirection = glm::normalize(playerPos - obstaclePos);
                m_player->Translate(pushDirection * 0.5f);
            }
        }
    }
}

bool GameManager::CheckCollision(const glm::vec3& pos1, float radius1, 
                                  const glm::vec3& pos2, float radius2) const {
    float distance = glm::length(pos1 - pos2);
    return distance < (radius1 + radius2);
}

glm::vec3 GameManager::GetRandomPosition(float minX, float maxX, 
                                          float minY, float maxY, 
                                          float minZ, float maxZ) {
    std::uniform_real_distribution<float> distX(minX, maxX);
    std::uniform_real_distribution<float> distY(minY, maxY);
    std::uniform_real_distribution<float> distZ(minZ, maxZ);
    
    return glm::vec3(distX(m_gen), distY(m_gen), distZ(m_gen));
}

void GameManager::ResetGame() {
    Initialize();
}

} // namespace SoulsEngine

