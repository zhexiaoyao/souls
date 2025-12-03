#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

namespace SoulsEngine {

// 变换类 - 封装位置、旋转、缩放
class Transform {
public:
    Transform();
    Transform(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale);
    ~Transform() = default;

    // 位置（平移）
    void SetPosition(const glm::vec3& position) { m_position = position; m_dirty = true; }
    void SetPosition(float x, float y, float z) { m_position = glm::vec3(x, y, z); m_dirty = true; }
    glm::vec3 GetPosition() const { return m_position; }
    
    // 相对平移
    void Translate(const glm::vec3& translation) { m_position += translation; m_dirty = true; }
    void Translate(float x, float y, float z) { m_position += glm::vec3(x, y, z); m_dirty = true; }

    // 旋转（欧拉角，单位：度）
    void SetRotation(const glm::vec3& rotation) { m_rotation = rotation; m_dirty = true; }
    void SetRotation(float x, float y, float z) { m_rotation = glm::vec3(x, y, z); m_dirty = true; }
    glm::vec3 GetRotation() const { return m_rotation; }
    
    // 相对旋转
    void Rotate(const glm::vec3& rotation) { m_rotation += rotation; m_dirty = true; }
    void Rotate(float x, float y, float z) { m_rotation += glm::vec3(x, y, z); m_dirty = true; }
    void RotateX(float angle) { m_rotation.x += angle; m_dirty = true; }
    void RotateY(float angle) { m_rotation.y += angle; m_dirty = true; }
    void RotateZ(float angle) { m_rotation.z += angle; m_dirty = true; }

    // 缩放
    void SetScale(const glm::vec3& scale) { m_scale = scale; m_dirty = true; }
    void SetScale(float x, float y, float z) { m_scale = glm::vec3(x, y, z); m_dirty = true; }
    void SetScale(float uniform) { m_scale = glm::vec3(uniform); m_dirty = true; }
    glm::vec3 GetScale() const { return m_scale; }
    
    // 相对缩放
    void Scale(const glm::vec3& scale) { m_scale *= scale; m_dirty = true; }
    void Scale(float uniform) { m_scale *= uniform; m_dirty = true; }

    // 获取变换矩阵（T * R * S）
    glm::mat4 GetMatrix() const;

    // 重置变换
    void Reset();
    void ResetPosition() { m_position = glm::vec3(0.0f); m_dirty = true; }
    void ResetRotation() { m_rotation = glm::vec3(0.0f); m_dirty = true; }
    void ResetScale() { m_scale = glm::vec3(1.0f); m_dirty = true; }

    // 检查是否需要更新
    bool IsDirty() const { return m_dirty; }
    void MarkDirty() { m_dirty = true; }
    void MarkClean() { m_dirty = false; }

private:
    glm::vec3 m_position;  // 位置
    glm::vec3 m_rotation;  // 旋转（欧拉角，度）
    glm::vec3 m_scale;     // 缩放

    mutable glm::mat4 m_matrix;  // 缓存的变换矩阵
    mutable bool m_dirty;        // 是否需要重新计算
};

// 坐标转换工具函数
namespace TransformUtils {
    // 局部坐标转世界坐标
    glm::vec3 LocalToWorld(const glm::vec3& localPoint, const glm::mat4& transformMatrix);
    
    // 世界坐标转局部坐标
    glm::vec3 WorldToLocal(const glm::vec3& worldPoint, const glm::mat4& transformMatrix);
    
    // 局部方向转世界方向（不考虑平移）
    glm::vec3 LocalDirectionToWorld(const glm::vec3& localDirection, const glm::mat4& transformMatrix);
    
    // 世界方向转局部方向（不考虑平移）
    glm::vec3 WorldDirectionToLocal(const glm::vec3& worldDirection, const glm::mat4& transformMatrix);
}

} // namespace SoulsEngine

