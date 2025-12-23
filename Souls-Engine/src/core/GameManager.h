#pragma once

#include "ObjectManager.h"
#include "Camera.h"
#include "SceneNode.h"
#include <memory>
#include <vector>
#include <random>
#include <glm/glm.hpp>

// 前向声明
struct GLFWwindow;

namespace SoulsEngine {

// 游戏对象类型
enum class GameObjectType {
    PLAYER,      // 玩家
    COLLECTIBLE, // 可收集物品
    OBSTACLE     // 障碍物
};

// 游戏管理器类
class GameManager {
public:
    GameManager(ObjectManager* objectManager, Camera* camera);
    ~GameManager();

    // 初始化游戏
    void Initialize();

    // 更新游戏逻辑
    void Update(float deltaTime);

    // 处理玩家输入
    void ProcessPlayerInput(float deltaTime, GLFWwindow* window);

    // 获取游戏状态
    int GetScore() const { return m_score; }
    float GetTimeRemaining() const { return m_timeRemaining; }
    bool IsGameOver() const { return m_gameOver; }
    bool IsGameWon() const { return m_gameWon; }

    // 重置游戏
    void ResetGame();

    // 碰撞检测
    bool CheckCollision(const glm::vec3& pos1, float radius1, const glm::vec3& pos2, float radius2) const;

private:
    // 创建玩家
    void CreatePlayer();

    // 生成收集物
    void SpawnCollectible();

    // 生成障碍物
    void SpawnObstacle();

    // 更新收集物
    void UpdateCollectibles(float deltaTime);

    // 更新障碍物
    void UpdateObstacles(float deltaTime);

    // 检查玩家与收集物的碰撞
    void CheckCollectibleCollisions();

    // 检查玩家与障碍物的碰撞
    void CheckObstacleCollisions();

    // 随机位置生成器
    glm::vec3 GetRandomPosition(float minX, float maxX, float minY, float maxY, float minZ, float maxZ);

    ObjectManager* m_objectManager;
    Camera* m_camera;

    // 游戏对象
    std::shared_ptr<SceneNode> m_player;
    std::vector<std::shared_ptr<SceneNode>> m_collectibles;
    std::vector<std::shared_ptr<SceneNode>> m_obstacles;

    // 游戏状态
    int m_score;
    float m_timeRemaining;
    float m_gameDuration;  // 游戏总时长（秒）
    bool m_gameOver;
    bool m_gameWon;

    // 生成参数
    float m_collectibleSpawnTimer;
    float m_collectibleSpawnInterval;
    float m_obstacleSpawnTimer;
    float m_obstacleSpawnInterval;
    int m_maxCollectibles;
    int m_maxObstacles;

    // 玩家移动速度
    float m_playerSpeed;

    // 随机数生成器
    std::random_device m_rd;
    std::mt19937 m_gen;

    // 游戏区域边界
    float m_arenaMinX, m_arenaMaxX;
    float m_arenaMinY, m_arenaMaxY;
    float m_arenaMinZ, m_arenaMaxZ;
};

} // namespace SoulsEngine

