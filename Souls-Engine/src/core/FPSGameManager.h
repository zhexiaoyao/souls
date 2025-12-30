#pragma once

#include "ObjectManager.h"
#include "Camera.h"
#include "SceneNode.h"
#include <memory>
#include <vector>
#include <random>
#include <glm/glm.hpp>

// Forward declarations
struct GLFWwindow;

namespace SoulsEngine {

// Target information structure
struct Target {
    std::shared_ptr<SceneNode> node;
    glm::vec3 position;
    float radius;  // Target radius
    bool isActive; // Is active
    int targetId;  // Target ID
};

// Wall information structure (for collision detection)
struct Wall {
    std::shared_ptr<SceneNode> node;
    glm::vec3 position;
    glm::vec3 size;  // Wall size (width, height, depth)
    glm::vec3 min;   // AABB minimum point
    glm::vec3 max;   // AABB maximum point
};

// FPS Game Manager class
class FPSGameManager {
public:
    FPSGameManager(ObjectManager* objectManager, Camera* camera);
    ~FPSGameManager();

    // Initialize game
    void Initialize();

    // Update game logic
    void Update(float deltaTime);

    // Process player input
    void ProcessPlayerInput(float deltaTime, GLFWwindow* window, int windowWidth, int windowHeight);

    // Process shooting
    void ProcessShoot(int windowWidth, int windowHeight);

    // Get game state
    int GetScore() const { return m_score; }
    bool IsGameOver() const { return m_gameOver; }
    bool IsZoomed() const { return m_isZoomed; }  // Get zoom state

    // Reset game
    void ResetGame();

    // Get all targets
    const std::vector<Target>& GetTargets() const { return m_targets; }

private:
    // Spawn target
    void SpawnTarget();

    // Raycast (for shooting detection)
    std::shared_ptr<SceneNode> Raycast(const glm::vec3& origin, const glm::vec3& direction, float maxDistance) const;

    // Check if hit target center (small range)
    bool IsHitCenter(const glm::vec3& hitPoint, const glm::vec3& targetCenter, float targetRadius) const;

    // Remove target
    void RemoveTarget(int targetId);

    // Create walls
    void CreateWalls();

    // Check collision between player and walls (AABB collision)
    bool CheckWallCollision(const glm::vec3& playerPos, float playerRadius, const glm::vec3& newPos, glm::vec3& correctedPos) const;

    // Random position generator
    glm::vec3 GetRandomPosition(float minX, float maxX, float minY, float maxY, float minZ, float maxZ);

    ObjectManager* m_objectManager;
    Camera* m_camera;

    // Game objects
    std::vector<Target> m_targets;
    std::vector<Wall> m_walls;

    // Game state
    int m_score;
    bool m_gameOver;

    // Spawn parameters
    float m_targetSpawnTimer;
    float m_targetSpawnInterval;
    int m_maxTargets;
    int m_nextTargetId;

    // Player movement speed
    float m_playerSpeed;
    float m_sprintSpeed;      // Sprint speed multiplier
    float m_jumpSpeed;
    float m_gravity;
    glm::vec3 m_velocity;
    bool m_isGrounded;
    bool m_isSprinting;       // Sprint state
    bool m_isCrouching;       // Crouch state

    // Camera heights
    float m_normalHeight;     // Normal standing height
    float m_crouchHeight;     // Crouching height
    float m_currentHeight;    // Current target height (for smooth transition)

    // Camera zoom state
    bool m_isZoomed;
    float m_normalFov;
    float m_zoomedFov;

    // Random number generator
    std::random_device m_rd;
    std::mt19937 m_gen;

    // Game area boundaries
    float m_arenaMinX, m_arenaMaxX;
    float m_arenaMinY, m_arenaMaxY;
    float m_arenaMinZ, m_arenaMaxZ;
};

} // namespace SoulsEngine

