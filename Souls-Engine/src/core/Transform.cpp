#include "Transform.h"

namespace SoulsEngine {

Transform::Transform()
    : m_position(0.0f, 0.0f, 0.0f)
    , m_rotation(0.0f, 0.0f, 0.0f)
    , m_scale(1.0f, 1.0f, 1.0f)
    , m_matrix(1.0f)
    , m_dirty(true)
{
}

Transform::Transform(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale)
    : m_position(position)
    , m_rotation(rotation)
    , m_scale(scale)
    , m_matrix(1.0f)
    , m_dirty(true)
{
}

glm::mat4 Transform::GetMatrix() const {
    if (m_dirty) {
        // 计算变换矩阵：T * R * S
        m_matrix = glm::mat4(1.0f);
        m_matrix = glm::scale(m_matrix, m_scale);
        m_matrix = glm::rotate(m_matrix, glm::radians(m_rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
        m_matrix = glm::rotate(m_matrix, glm::radians(m_rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
        m_matrix = glm::rotate(m_matrix, glm::radians(m_rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
        m_matrix = glm::translate(m_matrix, m_position);
        m_dirty = false;
    }
    return m_matrix;
}

void Transform::Reset() {
    m_position = glm::vec3(0.0f);
    m_rotation = glm::vec3(0.0f);
    m_scale = glm::vec3(1.0f);
    m_dirty = true;
}

// 坐标转换工具函数实现
namespace TransformUtils {

glm::vec3 LocalToWorld(const glm::vec3& localPoint, const glm::mat4& transformMatrix) {
    glm::vec4 worldPoint = transformMatrix * glm::vec4(localPoint, 1.0f);
    return glm::vec3(worldPoint);
}

glm::vec3 WorldToLocal(const glm::vec3& worldPoint, const glm::mat4& transformMatrix) {
    glm::mat4 inverseMatrix = glm::inverse(transformMatrix);
    glm::vec4 localPoint = inverseMatrix * glm::vec4(worldPoint, 1.0f);
    return glm::vec3(localPoint);
}

glm::vec3 LocalDirectionToWorld(const glm::vec3& localDirection, const glm::mat4& transformMatrix) {
    // 方向向量不考虑平移
    glm::mat4 rotationScaleMatrix = transformMatrix;
    rotationScaleMatrix[3] = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    glm::vec4 worldDirection = rotationScaleMatrix * glm::vec4(localDirection, 0.0f);
    return glm::normalize(glm::vec3(worldDirection));
}

glm::vec3 WorldDirectionToLocal(const glm::vec3& worldDirection, const glm::mat4& transformMatrix) {
    // 方向向量不考虑平移
    glm::mat4 rotationScaleMatrix = transformMatrix;
    rotationScaleMatrix[3] = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    glm::mat4 inverseMatrix = glm::inverse(rotationScaleMatrix);
    glm::vec4 localDirection = inverseMatrix * glm::vec4(worldDirection, 0.0f);
    return glm::normalize(glm::vec3(localDirection));
}

} // namespace TransformUtils

} // namespace SoulsEngine

