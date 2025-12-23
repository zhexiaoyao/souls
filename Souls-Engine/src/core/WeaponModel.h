#pragma once

#include "SceneNode.h"
#include <memory>
#include <glm/glm.hpp>

namespace SoulsEngine {

// 前向声明
class ObjectManager;
class Shader;
class Camera;

// 武器模型类（用于FPS游戏中的枪械显示）
class WeaponModel {
public:
    WeaponModel();
    ~WeaponModel();

    // 创建武器模型（简单的几何体组合）
    void CreateWeapon(ObjectManager* objectManager);

    // 更新武器位置（根据是否放大）
    void Update(bool isZoomed, int windowWidth, int windowHeight);

    // 渲染武器（在屏幕空间）
    void Render(Shader* shader, const Camera& camera, float aspectRatio);

    // Get weapon node
    std::shared_ptr<SceneNode> GetWeaponNode() const { return m_weaponNode; }

    // Get scope node
    std::shared_ptr<SceneNode> GetScopeNode() const { return m_scopeNode; }

private:
    std::shared_ptr<SceneNode> m_weaponNode;
    std::shared_ptr<SceneNode> m_scopeNode;  // Scope node reference
    glm::vec3 m_normalPosition;  // 正常位置（屏幕右下角）
    glm::vec3 m_zoomedPosition;  // 放大位置（准心附近）
    float m_animationProgress;   // 动画进度（0-1）
};

} // namespace SoulsEngine

