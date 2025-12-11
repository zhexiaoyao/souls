#ifndef NOMINMAX
#define NOMINMAX
#endif

#include "SelectionSystem.h"
#include "Camera.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cmath>
#include <limits>
#include <algorithm>

namespace SoulsEngine {

SelectionSystem::SelectionSystem()
    : m_selectedNode(nullptr)
    , m_isDragging(false)
    , m_rotationMode(false)
    , m_isScaling(false)
    , m_lastMousePos(0.0f, 0.0f)
    , m_dragStartMousePos(0.0f, 0.0f)
    , m_dragStartPosition(0.0f, 0.0f, 0.0f)
    , m_dragStartLocalPosition(0.0f, 0.0f, 0.0f)
    , m_dragStartRotation(0.0f, 0.0f, 0.0f)
    , m_dragStartDistance(5.0f)
    , m_dragStartWorldTransform(1.0f)
    , m_scaleStartScale(1.0f, 1.0f, 1.0f)
    , m_scaleStartIntersection(0.0f, 0.0f, 0.0f)
    , m_scaleStartMouseWorldPos(0.0f, 0.0f, 0.0f)
    , m_scaleStartDistance(1.0f)
{
}

void SelectionSystem::SelectNode(std::shared_ptr<SceneNode> node) {
    m_selectedNode = node;
}

void SelectionSystem::Deselect() {
    m_selectedNode = nullptr;
    m_isDragging = false;
    m_isScaling = false;
}

void SelectionSystem::StartDrag(const glm::vec2& mousePos, const Camera& camera) {
    if (!m_selectedNode) return;
    
    m_isDragging = true;
    m_lastMousePos = mousePos;
    m_dragStartMousePos = mousePos;
    // 获取物体的世界位置（考虑父节点变换）
    m_dragStartPosition = m_selectedNode->LocalToWorld(glm::vec3(0.0f, 0.0f, 0.0f));
    // 保存拖拽开始时的局部位�?
    m_dragStartLocalPosition = m_selectedNode->GetPosition();
    m_dragStartRotation = m_selectedNode->GetRotation();
    // 保存拖拽开始时的世界变换矩阵（用于坐标转换，避免旋转影响）
    m_dragStartWorldTransform = m_selectedNode->GetWorldTransform();
    
    // 计算并保存物体到相机的投影距离（在拖拽过程中保持不变�?
    glm::vec3 cameraPos = camera.GetPosition();
    glm::vec3 cameraFront = camera.GetFront();
    glm::vec3 toObject = m_dragStartPosition - cameraPos;
    m_dragStartDistance = glm::dot(toObject, cameraFront);
    if (m_dragStartDistance < 0.1f) {
        m_dragStartDistance = 5.0f; // 默认距离
    }
}

void SelectionSystem::UpdateDrag(const glm::vec2& mousePos, const Camera& camera, float deltaTime, float aspectRatio) {
    if (!m_isDragging || !m_selectedNode) return;
    
    glm::vec2 deltaMouse = mousePos - m_lastMousePos;
    
    // 如果鼠标位置没有变化，不更新（避免闪烁）
    if (glm::length(deltaMouse) < 0.0001f && !m_rotationMode) {
        return;
    }
    
    if (m_rotationMode) {
        // 旋转模式：根据鼠标移动旋转对象（大幅增加旋转速度�?
        float rotationSpeed = 500.0f;  // �?100.0f增加�?500.0f
        // 不使用deltaTime，直接根据鼠标移动量旋转
        m_selectedNode->RotateY(deltaMouse.x * rotationSpeed);
        m_selectedNode->RotateX(-deltaMouse.y * rotationSpeed);
    } else {
        // 平移模式：让物体在垂直于相机方向的平面上移动
        // 始终使用相机坐标系统，不受物体旋转影�?
        glm::vec3 cameraPos = camera.GetPosition();
        glm::vec3 cameraFront = camera.GetFront();
        glm::vec3 cameraRight = camera.GetRight();
        glm::vec3 cameraUp = camera.GetUp();
        
        // 使用拖拽开始时的距离，避免每次重新计算导致的位置跳�?
        float distanceAlongFront = m_dragStartDistance;
        
        // 计算鼠标在垂直于相机方向的平面上的位�?
        // 将归一化的鼠标坐标转换为世界坐�?
        float screenX = (mousePos.x - 0.5f) * 2.0f; // 归一化到[-1, 1]
        float screenY = (0.5f - mousePos.y) * 2.0f; // 归一化到[-1, 1]，Y轴翻�?
        
        // 使用相机的FOV和距离计算世界空间偏�?
        float fov = camera.GetZoom(); // 使用相机的zoom值（FOV�?
        float tanHalfFov = tanf(glm::radians(fov * 0.5f));
        
        // 计算在垂直于相机方向的平面上的目标位置（使用相机坐标系统�?
        glm::vec3 planeCenter = cameraPos + cameraFront * distanceAlongFront;
        glm::vec3 targetWorldPos = planeCenter;
        targetWorldPos += cameraRight * screenX * distanceAlongFront * tanHalfFov * aspectRatio;
        targetWorldPos += cameraUp * screenY * distanceAlongFront * tanHalfFov;
        
        // 计算世界空间中的偏移量（从拖拽开始位置到目标位置�?
        // 目标位置在垂直于相机方向的平面上
        glm::vec3 worldOffset = targetWorldPos - m_dragStartPosition;
        
        // 关键修复：平移应该始终在垂直于相机方向的平面上，不受物体自身旋转影响
        // 方法：只考虑父节点的变换，不考虑物体自身的旋转和缩放
        // 这样确保平移平面始终垂直于相机方向，不会跟随物体旋转
        Node* parent = m_selectedNode->GetParent();
        glm::vec3 localOffset;
        
        if (parent) {
            // 有父节点：将世界偏移量转换到父节点的局部空�?
            // 只考虑父节点的旋转和缩放，不考虑平移（因为这是方向向量）
            glm::mat4 parentWorldTransform = parent->GetWorldTransform();
            glm::mat4 parentRotationScale = parentWorldTransform;
            parentRotationScale[3] = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);  // 移除平移�?
            glm::mat4 parentRotationScaleInv = glm::inverse(parentRotationScale);
            localOffset = glm::vec3(parentRotationScaleInv * glm::vec4(worldOffset, 0.0f));
        } else {
            // 没有父节点：世界偏移量直接作为局部偏移量
            // 这样平移平面始终垂直于相机方向，不受物体旋转影响
            localOffset = worldOffset;
        }
        
        // 将偏移量加到拖拽开始时的局部位�?
        glm::vec3 newLocalPos = m_dragStartLocalPosition + localOffset;
        m_selectedNode->SetPosition(newLocalPos);
    }
    
    m_lastMousePos = mousePos;
}

void SelectionSystem::EndDrag() {
    m_isDragging = false;
}

void SelectionSystem::StartScale(const glm::vec2& mousePos, const Camera& camera, float aspectRatio) {
    if (!m_selectedNode) return;
    
    m_isScaling = true;
    m_lastMousePos = mousePos;
    
    // 保存缩放开始时的缩放�?
    m_scaleStartScale = m_selectedNode->GetScale();
    
    // 获取几何体中心的世界坐标
    glm::vec3 objectCenter = m_selectedNode->LocalToWorld(glm::vec3(0.0f, 0.0f, 0.0f));
    
    // 计算从相机到鼠标的射线方�?
    glm::vec3 cameraPos = camera.GetPosition();
    glm::vec3 cameraFront = camera.GetFront();
    glm::vec3 cameraRight = camera.GetRight();
    glm::vec3 cameraUp = camera.GetUp();
    
    float fov = camera.GetZoom();
    float tanHalfFov = tanf(glm::radians(fov * 0.5f));
    float screenX = (mousePos.x - 0.5f) * 2.0f;
    float screenY = (0.5f - mousePos.y) * 2.0f;
    
    // 计算鼠标对应的世界坐标（在垂直于相机方向的平面上�?
    float distanceToObject = glm::length(objectCenter - cameraPos);
    glm::vec3 mouseWorldPos = cameraPos + cameraFront * distanceToObject;
    mouseWorldPos += cameraRight * screenX * distanceToObject * tanHalfFov * aspectRatio;
    mouseWorldPos += cameraUp * screenY * distanceToObject * tanHalfFov;
    m_scaleStartMouseWorldPos = mouseWorldPos;
    
    // 计算从几何体中心到鼠标的射线方向
    glm::vec3 rayDir = glm::normalize(mouseWorldPos - objectCenter);
    
    // 计算射线与几何体的交点（简化版：使用包围盒�?
    m_scaleStartIntersection = CalculateRayIntersection(objectCenter, rayDir, m_scaleStartScale);
    
    // 计算交点距离几何体中心的距离
    m_scaleStartDistance = glm::length(m_scaleStartIntersection - objectCenter);
}

void SelectionSystem::UpdateScale(const glm::vec2& mousePos, const Camera& camera, float aspectRatio) {
    if (!m_isScaling || !m_selectedNode) return;
    
    glm::vec2 deltaMouse = mousePos - m_lastMousePos;
    
    // 如果鼠标位置没有变化，不更新
    if (glm::length(deltaMouse) < 0.0001f) {
        return;
    }
    
    // 获取几何体中心的世界坐标
    glm::vec3 objectCenter = m_selectedNode->LocalToWorld(glm::vec3(0.0f, 0.0f, 0.0f));
    
    // 计算从相机到鼠标的射线方�?
    glm::vec3 cameraPos = camera.GetPosition();
    glm::vec3 cameraFront = camera.GetFront();
    glm::vec3 cameraRight = camera.GetRight();
    glm::vec3 cameraUp = camera.GetUp();
    
    float fov = camera.GetZoom();
    float tanHalfFov = tanf(glm::radians(fov * 0.5f));
    float screenX = (mousePos.x - 0.5f) * 2.0f;
    float screenY = (0.5f - mousePos.y) * 2.0f;
    
    // 计算鼠标对应的世界坐标（在垂直于相机方向的平面上�?
    float distanceToObject = glm::length(objectCenter - cameraPos);
    glm::vec3 mouseWorldPos = cameraPos + cameraFront * distanceToObject;
    mouseWorldPos += cameraRight * screenX * distanceToObject * tanHalfFov * aspectRatio;
    mouseWorldPos += cameraUp * screenY * distanceToObject * tanHalfFov;
    
    // 计算从几何体中心到鼠标的射线方向
    glm::vec3 rayDir = glm::normalize(mouseWorldPos - objectCenter);
    
    // 计算当前鼠标位置对应的交点应该在哪里（保持与中心的距离）
    glm::vec3 targetIntersection = objectCenter + rayDir * m_scaleStartDistance;
    
    // 计算缩放比例：新距离 / 原始距离
    // 使用交点相对于几何体中心的方向来计算缩放
    glm::vec3 startDir = glm::normalize(m_scaleStartIntersection - objectCenter);
    glm::vec3 currentDir = glm::normalize(targetIntersection - objectCenter);
    
    // 计算缩放因子：基于鼠标移动的距离
    float startDistance = glm::length(m_scaleStartMouseWorldPos - objectCenter);
    float currentDistance = glm::length(mouseWorldPos - objectCenter);
    float scaleFactor = currentDistance / startDistance;
    
    // 应用缩放（均匀缩放�?
    glm::vec3 newScale = m_scaleStartScale * scaleFactor;
    
    // 限制缩放范围（避免过小或过大�?
    if (newScale.x < 0.1f) newScale = glm::vec3(0.1f);
    if (newScale.x > 10.0f) newScale = glm::vec3(10.0f);
    
    m_selectedNode->SetScale(newScale);
    
    m_lastMousePos = mousePos;
}

void SelectionSystem::EndScale() {
    m_isScaling = false;
}

glm::vec3 SelectionSystem::CalculateRayIntersection(const glm::vec3& objectCenter, const glm::vec3& rayDir, 
                                                      const glm::vec3& currentScale) const {
    // 简化版：假设几何体是一个单位立方体，根据缩放值计算包围盒
    // 计算射线与包围盒的交�?
    
    // 使用一个简化的方法：假设几何体的最大尺寸为1.0（单位立方体�?
    // 根据缩放值计算实际的包围盒尺�?
    float maxScale = glm::max(glm::max(currentScale.x, currentScale.y), currentScale.z);
    
    // 简化的包围盒（单位立方体，中心在原点）
    glm::vec3 boxMin = glm::vec3(-0.5f) * maxScale;
    glm::vec3 boxMax = glm::vec3(0.5f) * maxScale;
    
    // 射线-包围盒相交检测（简化版�?
    // 使用射线参数方程：P = origin + t * direction
    glm::vec3 invDir = 1.0f / rayDir;
    glm::vec3 t1 = (boxMin - objectCenter) * invDir;
    glm::vec3 t2 = (boxMax - objectCenter) * invDir;
    
    glm::vec3 tMin = glm::min(t1, t2);
    glm::vec3 tMax = glm::max(t1, t2);
    
    float tNear = glm::max(glm::max(tMin.x, tMin.y), tMin.z);
    float tFar = glm::min(glm::min(tMax.x, tMax.y), tMax.z);
    
    if (tNear > tFar || tFar < 0.0f) {
        // 没有交点，返回一个默认�?
        return objectCenter + rayDir * maxScale;
    }
    
    // 计算交点（使用较近的交点�?
    glm::vec3 intersection = objectCenter + rayDir * tNear;
    
    return intersection;
}

glm::vec3 SelectionSystem::ScreenToWorld(const glm::vec2& screenPos, const Camera& camera, float depth) const {
    // 简化版：将屏幕坐标转换为世界坐�?
    // 这里使用一个简化的方法，假设在相机前方的某个平面上
    // 实际应该使用射线检测，但为了简化，我们使用固定深度
    
    // 获取相机位置和方�?
    glm::vec3 cameraPos = camera.GetPosition();
    glm::vec3 cameraFront = camera.GetFront();
    glm::vec3 cameraRight = camera.GetRight();
    glm::vec3 cameraUp = camera.GetUp();
    
    // 假设在相机前方depth距离的平面上
    glm::vec3 worldPos = cameraPos + cameraFront * depth;
    
    // 根据屏幕坐标偏移（简化处理）
    float aspectRatio = 1.0f; // 这里应该传入实际的宽高比
    float fov = 45.0f; // 这里应该使用相机的FOV
    
    // 简化的屏幕到世界坐标转�?
    float screenX = (screenPos.x - 0.5f) * 2.0f; // 归一化到[-1, 1]
    float screenY = (0.5f - screenPos.y) * 2.0f; // 归一化到[-1, 1]，Y轴翻�?
    
    float tanHalfFov = tanf(glm::radians(fov * 0.5f));
    float offsetX = screenX * depth * tanHalfFov * aspectRatio;
    float offsetY = screenY * depth * tanHalfFov;
    
    worldPos += cameraRight * offsetX + cameraUp * offsetY;
    
    return worldPos;
}

std::shared_ptr<SceneNode> SelectionSystem::PickNode(const glm::vec2& screenPos, const Camera& camera,
                                                     const std::vector<std::shared_ptr<SceneNode>>& nodes,
                                                     int windowWidth, int windowHeight) const {
    if (nodes.empty()) return nullptr;
    
    // 计算从相机到鼠标的射�?
    glm::vec3 cameraPos = camera.GetPosition();
    glm::vec3 cameraFront = camera.GetFront();
    glm::vec3 cameraRight = camera.GetRight();
    glm::vec3 cameraUp = camera.GetUp();
    
    float fov = camera.GetZoom();
    float tanHalfFov = tanf(glm::radians(fov * 0.5f));
    float aspectRatio = static_cast<float>(windowWidth) / static_cast<float>(windowHeight);
    
    // 将屏幕坐标转换为世界空间中的方向向量
    float screenX = (screenPos.x - 0.5f) * 2.0f; // 归一化到[-1, 1]
    float screenY = (0.5f - screenPos.y) * 2.0f; // 归一化到[-1, 1]，Y轴翻�?
    
    // 计算射线方向（在近平面上的点�?
    glm::vec3 rayDir = cameraFront;
    rayDir += cameraRight * screenX * tanHalfFov * aspectRatio;
    rayDir += cameraUp * screenY * tanHalfFov;
    rayDir = glm::normalize(rayDir);
    
    // 射线起点是相机位�?
    glm::vec3 rayOrigin = cameraPos;
    
    // 遍历所有节点，找到与射线相交的最近节�?
    std::shared_ptr<SceneNode> closestNode = nullptr;
    float closestT = std::numeric_limits<float>::max();
    
    for (auto& node : nodes) {
        if (!node) continue;
        
        // 获取节点的世界变换矩�?
        glm::mat4 worldTransform = node->GetWorldTransform();
        
        // 计算节点的包围盒（在局部空间中，假设是单位立方体）
        // 考虑节点的缩�?
        glm::vec3 scale = node->GetScale();
        glm::vec3 boxMin = glm::vec3(-0.5f) * scale;
        glm::vec3 boxMax = glm::vec3(0.5f) * scale;
        
        // 使用变换矩阵的逆矩阵将射线转换到局部空�?
        // 这样可以正确处理旋转和缩�?
        glm::mat4 invTransform = glm::inverse(worldTransform);
        glm::vec3 localRayOrigin = glm::vec3(invTransform * glm::vec4(rayOrigin, 1.0f));
        glm::vec3 localRayDir = glm::vec3(invTransform * glm::vec4(rayDir, 0.0f));
        
        // 归一化方向向量（注意：在局部空间中，方向向量可能被缩放影响�?
        float dirLength = glm::length(localRayDir);
        if (dirLength < 0.0001f) continue; // 避免除零
        localRayDir = localRayDir / dirLength;
        
        // 在局部空间中进行射线-AABB相交检�?
        glm::vec3 invDir = 1.0f / localRayDir;
        glm::vec3 t1 = (boxMin - localRayOrigin) * invDir;
        glm::vec3 t2 = (boxMax - localRayOrigin) * invDir;
        
        glm::vec3 tMin = glm::min(t1, t2);
        glm::vec3 tMax = glm::max(t1, t2);
        
        float tNear = glm::max(glm::max(tMin.x, tMin.y), tMin.z);
        float tFar = glm::min(glm::min(tMax.x, tMax.y), tMax.z);
        
        // 检查是否相�?
        if (tNear <= tFar && tFar >= 0.0f) {
            // 相交，使用较近的交点
            float t = (tNear >= 0.0f) ? tNear : tFar;
            
            // 将t转换回世界空间（乘以方向向量的原始长度）
            float worldT = t * dirLength;
            
            // 只考虑相机前方的交�?
            if (worldT >= 0.0f && worldT < closestT) {
                closestT = worldT;
                closestNode = node;
            }
        }
    }
    
    return closestNode;
}

} // namespace SoulsEngine

