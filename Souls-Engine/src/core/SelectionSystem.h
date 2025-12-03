#pragma once

#include "SceneNode.h"
#include <memory>
#include <vector>
#include <glm/glm.hpp>
#include <limits>

namespace SoulsEngine {

// 前向声明
class Camera;

// 选择系统类
class SelectionSystem {
public:
    SelectionSystem();
    ~SelectionSystem() = default;

    // 选择节点
    void SelectNode(std::shared_ptr<SceneNode> node);
    void Deselect();
    
    // 获取当前选中的节点
    std::shared_ptr<SceneNode> GetSelectedNode() const { return m_selectedNode; }
    bool HasSelection() const { return m_selectedNode != nullptr; }

    // 鼠标交互
    void StartDrag(const glm::vec2& mousePos, const Camera& camera);
    void UpdateDrag(const glm::vec2& mousePos, const Camera& camera, float deltaTime, float aspectRatio);
    void EndDrag();
    bool IsDragging() const { return m_isDragging; }

    // 旋转模式
    void SetRotationMode(bool enabled) { m_rotationMode = enabled; }
    bool IsRotationMode() const { return m_rotationMode; }

    // 缩放模式
    void StartScale(const glm::vec2& mousePos, const Camera& camera, float aspectRatio);
    void UpdateScale(const glm::vec2& mousePos, const Camera& camera, float aspectRatio);
    void EndScale();
    bool IsScaling() const { return m_isScaling; }

    // 根据鼠标位置选择最近的节点（简化版射线检测）
    std::shared_ptr<SceneNode> PickNode(const glm::vec2& screenPos, const Camera& camera, 
                                         const std::vector<std::shared_ptr<SceneNode>>& nodes, 
                                         int windowWidth, int windowHeight) const;

private:
    std::shared_ptr<SceneNode> m_selectedNode;
    bool m_isDragging;
    bool m_rotationMode;
    bool m_isScaling;  // 是否处于缩放模式
    
    glm::vec2 m_lastMousePos;
    glm::vec2 m_dragStartMousePos;  // 拖拽开始时的鼠标位置
    glm::vec3 m_dragStartPosition;  // 拖拽开始时的物体世界位置
    glm::vec3 m_dragStartLocalPosition;  // 拖拽开始时的物体局部位置
    glm::vec3 m_dragStartRotation;
    float m_dragStartDistance;  // 拖拽开始时物体到相机的投影距离
    
    // 缩放相关
    glm::vec3 m_scaleStartScale;  // 缩放开始时的缩放值
    glm::vec3 m_scaleStartIntersection;  // 缩放开始时射线与几何体的交点（世界坐标）
    glm::vec3 m_scaleStartMouseWorldPos;  // 缩放开始时鼠标对应的世界坐标
    float m_scaleStartDistance;  // 缩放开始时交点距离几何体中心的距离
    
    // 辅助方法：屏幕坐标转世界坐标（简化版，使用相机平面）
    glm::vec3 ScreenToWorld(const glm::vec2& screenPos, const Camera& camera, float depth = 0.0f) const;
    
    // 计算从几何体中心到鼠标的射线与几何体的交点
    glm::vec3 CalculateRayIntersection(const glm::vec3& objectCenter, const glm::vec3& rayDir, 
                                       const glm::vec3& currentScale) const;
};

} // namespace SoulsEngine

