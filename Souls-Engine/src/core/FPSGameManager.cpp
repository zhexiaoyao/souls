#include "FPSGameManager.h"
#include "../geometry/Disk.h"
#include "../geometry/Cube.h"
#include "Material.h"
#include <GLFW/glfw3.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/intersect.hpp>
#include <iostream>
#include <algorithm>
#include <limits>

// Avoid max/min macro conflicts on Windows
#ifdef _WIN32
#undef max
#undef min
#endif

namespace SoulsEngine {

FPSGameManager::FPSGameManager(ObjectManager* objectManager, Camera* camera)
    : m_objectManager(objectManager)
    , m_camera(camera)
    , m_score(0)
    , m_gameOver(false)
    , m_targetSpawnTimer(0.0f)
    , m_targetSpawnInterval(2.0f)  // Spawn a target every 2 seconds
    , m_maxTargets(15)
    , m_nextTargetId(0)
    , m_playerSpeed(5.0f)
    , m_sprintSpeed(8.0f)
    , m_jumpSpeed(8.0f)
    , m_gravity(-20.0f)
    , m_velocity(0.0f)
    , m_isGrounded(true)
    , m_isSprinting(false)
    , m_isCrouching(false)
    , m_normalHeight(1.6f)
    , m_crouchHeight(0.9f)
    , m_currentHeight(1.6f)
    , m_isZoomed(false)
    , m_normalFov(45.0f)
    , m_zoomedFov(20.0f)
    , m_gen(m_rd())
    , m_arenaMinX(-20.0f)
    , m_arenaMaxX(20.0f)
    , m_arenaMinY(0.0f)
    , m_arenaMaxY(10.0f)
    , m_arenaMinZ(-20.0f)
    , m_arenaMaxZ(20.0f)
{
}

FPSGameManager::~FPSGameManager() {
}

void FPSGameManager::Initialize() {
    // Clear scene
    m_objectManager->Clear();
    m_targets.clear();
    m_walls.clear();

    // Reset game state
    m_score = 0;
    m_gameOver = false;
    m_nextTargetId = 0;
    m_velocity = glm::vec3(0.0f);
    m_isGrounded = true;
    m_isSprinting = false;
    m_isCrouching = false;
    m_currentHeight = m_normalHeight;

    // Set camera initial position (first-person view)
    m_camera->SetPosition(glm::vec3(0.0f, m_normalHeight, 0.0f));  // Eye height approximately 1.6 meters
    m_camera->SetZoom(m_normalFov);

    // Create ground (using cube with scaling)
    // 地面应该足够大以覆盖整个竞技场并接收阴影
    float groundSize = 50.0f;  // 地面大小（比竞技场大，确保覆盖所有阴影）
    float groundHeight = 0.2f;  // 地面厚度
    glm::vec3 groundColor(0.4f, 0.4f, 0.35f);  // 灰褐色地面
    
    auto groundMesh = std::make_shared<Cube>(groundSize, groundColor);
    auto ground = m_objectManager->CreateNode("Ground", groundMesh);
    // 地面位置：y = -groundHeight/2，这样地面顶部在y=0
    ground->SetPosition(0.0f, -groundHeight / 2.0f, 0.0f);
    ground->SetScale(1.0f, groundHeight / groundSize, 1.0f);  // 缩放高度
    
    // 创建漫反射材质用于地面（高漫反射，低镜面反射，适合地面）
    auto groundMaterial = std::make_shared<Material>();
    groundMaterial->SetColor(groundColor);  // 设置环境光和漫反射颜色
    groundMaterial->SetAmbient(groundColor * 0.3f);  // 环境光稍暗
    groundMaterial->SetDiffuse(groundColor);  // 漫反射使用完整颜色
    groundMaterial->SetSpecular(0.1f, 0.1f, 0.1f);  // 低镜面反射（地面不反光）
    groundMaterial->SetShininess(8.0f);  // 低光泽度（粗糙表面）
    groundMaterial->SetName("GroundMaterial");
    
    // 将材质应用到地面
    auto groundSceneNode = std::dynamic_pointer_cast<SceneNode>(ground);
    if (groundSceneNode) {
        groundSceneNode->SetMaterial(groundMaterial);
        std::cout << "Ground material applied successfully" << std::endl;
    } else {
        std::cerr << "Warning: Failed to apply material to ground" << std::endl;
    }

    // Create walls
    CreateWalls();

    // Spawn initial targets
    for (int i = 0; i < 8; ++i) {
        SpawnTarget();
    }

    std::cout << "FPS game initialized!" << std::endl;
    std::cout << "Controls:" << std::endl;
    std::cout << "  - WASD: Move" << std::endl;
    std::cout << "  - Shift: Sprint" << std::endl;
    std::cout << "  - Ctrl: Crouch" << std::endl;
    std::cout << "  - Space: Jump" << std::endl;
    std::cout << "  - Mouse: Rotate view" << std::endl;
    std::cout << "  - Right-click: Zoom" << std::endl;
    std::cout << "  - Left-click: Shoot" << std::endl;
    std::cout << "  - Hit target: +1 point" << std::endl;
    std::cout << "  - Hit center: +10 points" << std::endl;
}

void FPSGameManager::Update(float deltaTime) {
    if (m_gameOver) {
        return;
    }

    // Update crouch height smoothly
    float targetHeight = m_isCrouching ? m_crouchHeight : m_normalHeight;
    float heightChangeSpeed = 8.0f;  // Height transition speed
    if (m_currentHeight < targetHeight) {
        m_currentHeight = (std::min)(m_currentHeight + heightChangeSpeed * deltaTime, targetHeight);
    } else if (m_currentHeight > targetHeight) {
        m_currentHeight = (std::max)(m_currentHeight - heightChangeSpeed * deltaTime, targetHeight);
    }
    
    // Update gravity
    if (!m_isGrounded) {
        m_velocity.y += m_gravity * deltaTime;
    }

    // Update camera position (based on velocity)
    glm::vec3 cameraPos = m_camera->GetPosition();
    cameraPos += m_velocity * deltaTime;
    
    // Check ground collision (use current target height as ground level)
    float groundHeight = m_currentHeight;
    if (cameraPos.y < groundHeight) {
        cameraPos.y = groundHeight;
        m_velocity.y = 0.0f;
        m_isGrounded = true;
    } else if (m_isGrounded) {
        // When grounded, smoothly adjust camera height to match crouch/stand state
        // This ensures smooth transition when crouching/standing
        float heightDiff = groundHeight - cameraPos.y;
        if (abs(heightDiff) > 0.01f) {
            float adjustSpeed = heightChangeSpeed;
            cameraPos.y += (heightDiff > 0.0f ? adjustSpeed : -adjustSpeed) * deltaTime;
            // Clamp to prevent overshooting
            if ((heightDiff > 0.0f && cameraPos.y > groundHeight) || 
                (heightDiff < 0.0f && cameraPos.y < groundHeight)) {
                cameraPos.y = groundHeight;
            }
        } else {
            cameraPos.y = groundHeight;
        }
    }

    // Clamp to game area
    cameraPos.x = glm::clamp(cameraPos.x, m_arenaMinX, m_arenaMaxX);
    cameraPos.y = glm::clamp(cameraPos.y, m_arenaMinY, m_arenaMaxY);
    cameraPos.z = glm::clamp(cameraPos.z, m_arenaMinZ, m_arenaMaxZ);

    m_camera->SetPosition(cameraPos);

    // Update target spawning
    m_targetSpawnTimer += deltaTime;
    if (m_targetSpawnTimer >= m_targetSpawnInterval && 
        static_cast<int>(m_targets.size()) < m_maxTargets) {
        SpawnTarget();
        m_targetSpawnTimer = 0.0f;
    }
}

void FPSGameManager::ProcessPlayerInput(float deltaTime, GLFWwindow* window, int windowWidth, int windowHeight) {
    if (m_gameOver) {
        return;
    }

    glm::vec3 moveDirection(0.0f);

    // Get camera direction (for determining movement direction)
    glm::vec3 cameraFront = m_camera->GetFront();
    glm::vec3 cameraRight = m_camera->GetRight();
    
    // Project camera direction onto XZ plane (horizontal movement)
    cameraFront.y = 0.0f;
    cameraRight.y = 0.0f;
    cameraFront = glm::normalize(cameraFront);
    cameraRight = glm::normalize(cameraRight);

    // WASD controls
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

    // Sprint (Shift key)
    m_isSprinting = (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || 
                     glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS);
    
    // Crouch (Ctrl key)
    m_isCrouching = (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS || 
                     glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS);
    
    // Can't sprint while crouching
    if (m_isCrouching) {
        m_isSprinting = false;
    }

    // Jump (can't jump while crouching)
    static bool spaceKeyPressed = false;
    bool spaceKeyDown = glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS;
    if (spaceKeyDown && !spaceKeyPressed && m_isGrounded && !m_isCrouching) {
        m_velocity.y = m_jumpSpeed;
        m_isGrounded = false;
    }
    spaceKeyPressed = spaceKeyDown;

    // Calculate movement speed (sprint multiplier when sprinting)
    float currentSpeed = m_isSprinting ? m_sprintSpeed : m_playerSpeed;
    
    // Normalize movement direction and apply speed
    if (glm::length(moveDirection) > 0.0f) {
        moveDirection = glm::normalize(moveDirection);
        glm::vec3 movement = moveDirection * currentSpeed * deltaTime;
        glm::vec3 cameraPos = m_camera->GetPosition();
        glm::vec3 newPos = cameraPos + movement;
        
        // Check wall collision before moving
        glm::vec3 correctedPos = newPos;
        float playerRadius = 0.3f;  // Player collision radius
        if (CheckWallCollision(cameraPos, playerRadius, newPos, correctedPos)) {
            // Collision detected, use corrected position
            newPos = correctedPos;
        }
        
        m_camera->SetPosition(newPos);
    }

    // Mouse control camera (first-person view, always follow mouse)
    // When using GLFW_CURSOR_DISABLED mode, GLFW automatically handles mouse repositioning
    static bool firstMouse = true;
    static double lastMouseX = 0.0;
    static double lastMouseY = 0.0;
    
    double mouseX, mouseY;
    glfwGetCursorPos(window, &mouseX, &mouseY);
    
    if (firstMouse) {
        lastMouseX = mouseX;
        lastMouseY = mouseY;
        firstMouse = false;
        // Don't process on first frame to avoid sudden camera rotation
    } else {
        float xoffset = static_cast<float>(mouseX - lastMouseX);
        float yoffset = static_cast<float>(lastMouseY - mouseY);
        
        // Clamp mouse movement to avoid sudden large movements (important when mouse is disabled)
        xoffset = glm::clamp(xoffset, -200.0f, 200.0f);
        yoffset = glm::clamp(yoffset, -200.0f, 200.0f);
        
        lastMouseX = mouseX;
        lastMouseY = mouseY;
        
        // Apply mouse movement (higher threshold to avoid tiny movements triggering updates)
        // Only update when movement exceeds threshold to reduce UpdateCameraVectors calls
        const float minMovement = 0.1f;  // Higher threshold to reduce update frequency
        if (abs(xoffset) > minMovement || abs(yoffset) > minMovement) {
            m_camera->ProcessMouseMovement(xoffset, yoffset, true);
        }
    }

    // Right-click zoom
    static bool rightMousePressed = false;
    bool rightMouseDown = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS;
    if (rightMouseDown && !rightMousePressed) {
        m_isZoomed = true;
        m_camera->SetZoom(m_zoomedFov);
    } else if (!rightMouseDown && rightMousePressed) {
        m_isZoomed = false;
        m_camera->SetZoom(m_normalFov);
    }
    rightMousePressed = rightMouseDown;

    // Left-click shoot
    static bool leftMousePressed = false;
    bool leftMouseDown = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
    if (leftMouseDown && !leftMousePressed) {
        ProcessShoot(windowWidth, windowHeight);
    }
    leftMousePressed = leftMouseDown;
}

void FPSGameManager::ProcessShoot(int windowWidth, int windowHeight) {
    // Cast ray from camera position towards screen center
    glm::vec3 rayOrigin = m_camera->GetPosition();
    glm::vec3 cameraFront = m_camera->GetFront();
    glm::vec3 rayDirection = glm::normalize(cameraFront);

    // Perform raycast
    std::shared_ptr<SceneNode> hitNode = Raycast(rayOrigin, rayDirection, 100.0f);

    if (hitNode) {
        // Find hit target
        for (auto& target : m_targets) {
            if (target.node == hitNode && target.isActive) {
                // Calculate intersection point between ray and disk plane
                glm::vec3 targetPos = target.position;
                glm::mat4 targetTransform = target.node->GetWorldTransform();
                
                // Get target normal (disk default normal is Z-axis, transformed)
                glm::vec3 targetNormal = glm::normalize(glm::vec3(targetTransform * glm::vec4(0.0f, 0.0f, 1.0f, 0.0f)));
                
                // Calculate intersection point between ray and plane
                float denom = glm::dot(targetNormal, rayDirection);
                glm::vec3 hitPoint = targetPos;  // Default to center
                
                if (abs(denom) > 0.0001f) {
                    float t = glm::dot(targetPos - rayOrigin, targetNormal) / denom;
                    if (t > 0.0f) {
                        hitPoint = rayOrigin + rayDirection * t;
                    }
                }
                
                // Check if hit center
                bool hitCenter = IsHitCenter(hitPoint, target.position, target.radius);
                
                if (hitCenter) {
                    m_score += 10;
                    std::cout << "Center hit! +10 points (Total: " << m_score << ")" << std::endl;
                } else {
                    m_score += 1;
                    std::cout << "Target hit! +1 point (Total: " << m_score << ")" << std::endl;
                }
                
                // Remove target
                RemoveTarget(target.targetId);
                break;
            }
        }
    }
}

std::shared_ptr<SceneNode> FPSGameManager::Raycast(const glm::vec3& origin, const glm::vec3& direction, float maxDistance) const {
    std::shared_ptr<SceneNode> closestNode = nullptr;
    float closestDistance = maxDistance;

    // Get all nodes
    auto allNodes = m_objectManager->GetAllNodes();

    for (auto& node : allNodes) {
        if (!node) continue;
        
        // Skip ground
        if (node->GetName() == "Ground") continue;
        
        // Skip walls (all wall names start with "Wall_")
        std::string nodeName = node->GetName();
        if (nodeName.find("Wall_") == 0) continue;
        
        // Skip weapon nodes (they shouldn't be shot)
        if (nodeName.find("Weapon") == 0) continue;

        // Get node world transform
        glm::mat4 worldTransform = node->GetWorldTransform();
        glm::vec3 nodePos = glm::vec3(worldTransform[3]);
        glm::vec3 scale = node->GetScale();

        // Simplified ray-sphere intersection (assuming targets are disks, approximated as spheres)
        float targetRadius = (std::max)(scale.x, scale.y) * 0.5f;  // Use larger scale value as radius
        
        float t;
        if (glm::intersectRaySphere(origin, direction, nodePos, targetRadius * targetRadius, t)) {
            if (t > 0.0f && t < closestDistance) {
                closestDistance = t;
                closestNode = node;
            }
        }
    }

    return closestNode;
}

bool FPSGameManager::IsHitCenter(const glm::vec3& hitPoint, const glm::vec3& targetCenter, float targetRadius) const {
    // Calculate distance from hit point to center
    float distance = glm::length(hitPoint - targetCenter);
    // Center small area defined as 30% of radius
    float centerRadius = targetRadius * 0.3f;
    return distance <= centerRadius;
}

void FPSGameManager::SpawnTarget() {
    // Generate random position
    glm::vec3 position = GetRandomPosition(
        m_arenaMinX + 2.0f, m_arenaMaxX - 2.0f,
        2.0f, 8.0f,
        m_arenaMinZ + 2.0f, m_arenaMaxZ - 2.0f
    );

    // Create disk target (red)
    float targetRadius = 1.0f;
    auto targetMesh = std::make_shared<Disk>(targetRadius, 36, glm::vec3(1.0f, 0.0f, 0.0f));  // Red
    auto targetNode = m_objectManager->CreateNode("Target_" + std::to_string(m_nextTargetId), targetMesh);
    targetNode->SetPosition(position);
    
    // Rotate target to face camera direction (make target face player initial position)
    // Disk default is in XY plane (normal is Z-axis), we need to make it face camera
    glm::vec3 cameraPos = m_camera->GetPosition();
    glm::vec3 toCamera = cameraPos - position;
    
    // Calculate Y-axis rotation (horizontal rotation)
    if (glm::length(toCamera) > 0.001f) {
        float yaw = atan2(toCamera.x, toCamera.z) * 180.0f / 3.14159265359f;
        targetNode->SetRotation(0.0f, yaw, 0.0f);
        
        // Calculate X-axis rotation (vertical rotation)
        float horizontalDist = sqrt(toCamera.x * toCamera.x + toCamera.z * toCamera.z);
        float pitch = atan2(-toCamera.y, horizontalDist) * 180.0f / 3.14159265359f;
        targetNode->SetRotation(pitch, yaw, 0.0f);
    }

    Target target;
    target.node = targetNode;
    target.position = position;
    target.radius = targetRadius;
    target.isActive = true;
    target.targetId = m_nextTargetId;

    m_targets.push_back(target);
    m_nextTargetId++;

    std::cout << "Spawned target #" << target.targetId << " at (" 
              << position.x << ", " << position.y << ", " << position.z << ")" << std::endl;
}

void FPSGameManager::RemoveTarget(int targetId) {
    for (auto it = m_targets.begin(); it != m_targets.end(); ++it) {
        if (it->targetId == targetId && it->isActive) {
            it->isActive = false;
            m_objectManager->RemoveNode(it->node);
            m_targets.erase(it);
            std::cout << "Target #" << targetId << " removed" << std::endl;
            break;
        }
    }
}

void FPSGameManager::ResetGame() {
    Initialize();
}

glm::vec3 FPSGameManager::GetRandomPosition(float minX, float maxX, float minY, float maxY, float minZ, float maxZ) {
    std::uniform_real_distribution<float> distX(minX, maxX);
    std::uniform_real_distribution<float> distY(minY, maxY);
    std::uniform_real_distribution<float> distZ(minZ, maxZ);
    
    return glm::vec3(distX(m_gen), distY(m_gen), distZ(m_gen));
}

void FPSGameManager::CreateWalls() {
    // Clear existing walls
    m_walls.clear();
    
    // Arena boundaries (slightly smaller than arena bounds to leave space for walls)
    float wallHeight = 4.0f;
    float wallThickness = 0.5f;
    float arenaSize = 18.0f;  // Half size of arena
    
    // Wall colors (different color for better visibility)
    glm::vec3 wallColor(0.5f, 0.5f, 0.6f);
    
    // Create very rough material for walls (very low shininess = very rough surface)
    // Very rough materials have minimal shininess (1-2) and almost no specular reflection
    auto wallMaterial = std::make_shared<Material>();
    wallMaterial->SetColor(wallColor);  // Sets ambient and diffuse
    wallMaterial->SetSpecular(0.01f, 0.01f, 0.01f);  // Almost no specular reflection for very rough surface
    wallMaterial->SetShininess(1.0f);  // Minimal shininess for very rough texture (rough = 1-10, smooth = 32-128)
    wallMaterial->SetName("VeryRoughWall");
    
    // North wall (positive Z)
    {
        Wall wall;
        wall.size = glm::vec3(arenaSize * 2.0f, wallHeight, wallThickness);
        wall.position = glm::vec3(0.0f, wallHeight / 2.0f, arenaSize);
        wall.min = wall.position - wall.size * 0.5f;
        wall.max = wall.position + wall.size * 0.5f;
        
        auto wallMesh = std::make_shared<Cube>(1.0f, wallColor);
        wall.node = m_objectManager->CreateNode("Wall_North", wallMesh);
        wall.node->SetPosition(wall.position);
        wall.node->SetScale(wall.size.x, wall.size.y, wall.size.z);
        // Apply rough material to wall
        auto sceneNode = std::dynamic_pointer_cast<SceneNode>(wall.node);
        if (sceneNode) {
            sceneNode->SetMaterial(wallMaterial);
        }
        
        m_walls.push_back(wall);
    }
    
    // South wall (negative Z)
    {
        Wall wall;
        wall.size = glm::vec3(arenaSize * 2.0f, wallHeight, wallThickness);
        wall.position = glm::vec3(0.0f, wallHeight / 2.0f, -arenaSize);
        wall.min = wall.position - wall.size * 0.5f;
        wall.max = wall.position + wall.size * 0.5f;
        
        auto wallMesh = std::make_shared<Cube>(1.0f, wallColor);
        wall.node = m_objectManager->CreateNode("Wall_South", wallMesh);
        wall.node->SetPosition(wall.position);
        wall.node->SetScale(wall.size.x, wall.size.y, wall.size.z);
        // Apply rough material to wall
        auto sceneNode = std::dynamic_pointer_cast<SceneNode>(wall.node);
        if (sceneNode) {
            sceneNode->SetMaterial(wallMaterial);
        }
        
        m_walls.push_back(wall);
    }
    
    // East wall (positive X)
    {
        Wall wall;
        wall.size = glm::vec3(wallThickness, wallHeight, arenaSize * 2.0f);
        wall.position = glm::vec3(arenaSize, wallHeight / 2.0f, 0.0f);
        wall.min = wall.position - wall.size * 0.5f;
        wall.max = wall.position + wall.size * 0.5f;
        
        auto wallMesh = std::make_shared<Cube>(1.0f, wallColor);
        wall.node = m_objectManager->CreateNode("Wall_East", wallMesh);
        wall.node->SetPosition(wall.position);
        wall.node->SetScale(wall.size.x, wall.size.y, wall.size.z);
        // Apply rough material to wall
        auto sceneNode = std::dynamic_pointer_cast<SceneNode>(wall.node);
        if (sceneNode) {
            sceneNode->SetMaterial(wallMaterial);
        }
        
        m_walls.push_back(wall);
    }
    
    // West wall (negative X)
    {
        Wall wall;
        wall.size = glm::vec3(wallThickness, wallHeight, arenaSize * 2.0f);
        wall.position = glm::vec3(-arenaSize, wallHeight / 2.0f, 0.0f);
        wall.min = wall.position - wall.size * 0.5f;
        wall.max = wall.position + wall.size * 0.5f;
        
        auto wallMesh = std::make_shared<Cube>(1.0f, wallColor);
        wall.node = m_objectManager->CreateNode("Wall_West", wallMesh);
        wall.node->SetPosition(wall.position);
        wall.node->SetScale(wall.size.x, wall.size.y, wall.size.z);
        // Apply rough material to wall
        auto sceneNode = std::dynamic_pointer_cast<SceneNode>(wall.node);
        if (sceneNode) {
            sceneNode->SetMaterial(wallMaterial);
        }
        
        m_walls.push_back(wall);
    }
    
    // Add some internal walls for more complex gameplay
    // Internal wall 1 (vertical, blocking part of the path)
    {
        Wall wall;
        wall.size = glm::vec3(wallThickness, wallHeight, 6.0f);
        wall.position = glm::vec3(5.0f, wallHeight / 2.0f, 3.0f);
        wall.min = wall.position - wall.size * 0.5f;
        wall.max = wall.position + wall.size * 0.5f;
        
        auto wallMesh = std::make_shared<Cube>(1.0f, wallColor);
        wall.node = m_objectManager->CreateNode("Wall_Internal_1", wallMesh);
        wall.node->SetPosition(wall.position);
        wall.node->SetScale(wall.size.x, wall.size.y, wall.size.z);
        // Apply rough material to wall
        auto sceneNode = std::dynamic_pointer_cast<SceneNode>(wall.node);
        if (sceneNode) {
            sceneNode->SetMaterial(wallMaterial);
        }
        
        m_walls.push_back(wall);
    }
    
    // Internal wall 2 (horizontal)
    {
        Wall wall;
        wall.size = glm::vec3(8.0f, wallHeight, wallThickness);
        wall.position = glm::vec3(-5.0f, wallHeight / 2.0f, -8.0f);
        wall.min = wall.position - wall.size * 0.5f;
        wall.max = wall.position + wall.size * 0.5f;
        
        auto wallMesh = std::make_shared<Cube>(1.0f, wallColor);
        wall.node = m_objectManager->CreateNode("Wall_Internal_2", wallMesh);
        wall.node->SetPosition(wall.position);
        wall.node->SetScale(wall.size.x, wall.size.y, wall.size.z);
        // Apply rough material to wall
        auto sceneNode = std::dynamic_pointer_cast<SceneNode>(wall.node);
        if (sceneNode) {
            sceneNode->SetMaterial(wallMaterial);
        }
        
        m_walls.push_back(wall);
    }
    
    std::cout << "Created " << m_walls.size() << " walls" << std::endl;
}

bool FPSGameManager::CheckWallCollision(const glm::vec3& playerPos, float playerRadius, const glm::vec3& newPos, glm::vec3& correctedPos) const {
    correctedPos = newPos;
    bool hasCollision = false;
    
    // Player AABB (axis-aligned bounding box)
    glm::vec3 playerMin = newPos - glm::vec3(playerRadius);
    glm::vec3 playerMax = newPos + glm::vec3(playerRadius);
    
    // Adjust player AABB for height (player is standing, so consider Y axis)
    playerMin.y = newPos.y - 0.8f;  // Player height from ground
    playerMax.y = newPos.y + 0.8f;  // Player top
    
    for (const auto& wall : m_walls) {
        if (!wall.node) continue;
        
        // Calculate wall AABB from world transform and scale
        // Cube base size is 1.0, so actual size = scale
        glm::vec3 wallScale = wall.node->GetScale();
        glm::mat4 worldTransform = wall.node->GetWorldTransform();
        glm::vec3 wallWorldPos = glm::vec3(worldTransform[3]);  // Extract position (translation) from transform matrix
        
        // Calculate AABB min/max based on actual scaled size
        // Since Cube is created with size 1.0, the actual dimensions are the scale values
        glm::vec3 wallHalfSize = wallScale * 0.5f;
        glm::vec3 wallMin = wallWorldPos - wallHalfSize;
        glm::vec3 wallMax = wallWorldPos + wallHalfSize;
        
        // Check AABB collision
        if (playerMax.x > wallMin.x && playerMin.x < wallMax.x &&
            playerMax.y > wallMin.y && playerMin.y < wallMax.y &&
            playerMax.z > wallMin.z && playerMin.z < wallMax.z) {
            
            hasCollision = true;
            
            // Calculate penetration depth for each axis (how much player overlaps with wall)
            // We need to push player out by this amount
            float penetrationX = 0.0f;
            float penetrationZ = 0.0f;
            
            // Calculate penetration in X axis
            float playerLeftX = newPos.x - playerRadius;
            float playerRightX = newPos.x + playerRadius;
            
            if (playerRightX > wallMin.x && playerLeftX < wallMax.x) {
                // Player AABB overlaps with wall AABB in X
                float overlapLeft = playerRightX - wallMin.x;   // How much player extends into wall from left
                float overlapRight = wallMax.x - playerLeftX;   // How much player extends into wall from right
                penetrationX = (overlapLeft < overlapRight) ? overlapLeft : -overlapRight;
            }
            
            // Calculate penetration in Z axis
            float playerBackZ = newPos.z - playerRadius;
            float playerFrontZ = newPos.z + playerRadius;
            
            if (playerFrontZ > wallMin.z && playerBackZ < wallMax.z) {
                // Player AABB overlaps with wall AABB in Z
                float overlapBack = playerFrontZ - wallMin.z;   // How much player extends into wall from back
                float overlapFront = wallMax.z - playerBackZ;   // How much player extends into wall from front
                penetrationZ = (overlapBack < overlapFront) ? overlapBack : -overlapFront;
            }
            
            // Push player out of wall using minimum translation vector (MTV)
            // Choose the axis with smaller penetration depth
            if (abs(penetrationX) > 0.001f && (abs(penetrationX) < abs(penetrationZ) || abs(penetrationZ) < 0.001f)) {
                correctedPos.x -= penetrationX;
            } else if (abs(penetrationZ) > 0.001f) {
                correctedPos.z -= penetrationZ;
            }
            
            // Recalculate player AABB with corrected position
            playerMin = correctedPos - glm::vec3(playerRadius);
            playerMax = correctedPos + glm::vec3(playerRadius);
            playerMin.y = correctedPos.y - 0.8f;
            playerMax.y = correctedPos.y + 0.8f;
        }
    }
    
    return hasCollision;
}

} // namespace SoulsEngine

