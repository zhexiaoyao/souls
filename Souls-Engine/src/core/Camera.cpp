#include "Camera.h"
#include <algorithm>

namespace SoulsEngine {

Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch)
    : m_position(position)
    , m_worldUp(up)
    , m_yaw(yaw)
    , m_pitch(pitch)
    , m_front(glm::vec3(0.0f, 0.0f, -1.0f))
    , m_movementSpeed(2.5f)
    , m_mouseSensitivity(0.1f)
    , m_zoom(45.0f)
    , m_fov(45.0f)
    , m_nearPlane(0.1f)
    , m_farPlane(100.0f)
{
    UpdateCameraVectors();
}

glm::mat4 Camera::GetViewMatrix() const {
    return glm::lookAt(m_position, m_position + m_front, m_up);
}

glm::mat4 Camera::GetProjectionMatrix(float aspectRatio) const {
    return glm::perspective(glm::radians(m_zoom), aspectRatio, m_nearPlane, m_farPlane);
}

glm::mat4 Camera::GetMVPMatrix(const glm::mat4& model, float aspectRatio) const {
    glm::mat4 view = GetViewMatrix();
    glm::mat4 projection = GetProjectionMatrix(aspectRatio);
    return projection * view * model;
}

void Camera::ProcessKeyboard(CameraMovement direction, float deltaTime) {
    float velocity = m_movementSpeed * deltaTime;
    
    if (direction == FORWARD)
        m_position += m_front * velocity;
    if (direction == BACKWARD)
        m_position -= m_front * velocity;
    if (direction == LEFT)
        m_position -= m_right * velocity;
    if (direction == RIGHT)
        m_position += m_right * velocity;
    if (direction == UP)
        m_position += m_worldUp * velocity;
    if (direction == DOWN)
        m_position -= m_worldUp * velocity;
}

void Camera::ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch) {
    xoffset *= m_mouseSensitivity;
    yoffset *= m_mouseSensitivity;
    
    m_yaw += xoffset;
    m_pitch += yoffset;
    
    // 限制俯仰角
    if (constrainPitch) {
        if (m_pitch > 89.0f)
            m_pitch = 89.0f;
        if (m_pitch < -89.0f)
            m_pitch = -89.0f;
    }
    
    UpdateCameraVectors();
}

void Camera::ProcessMouseScroll(float yoffset) {
    m_zoom -= yoffset;
    if (m_zoom < 1.0f)
        m_zoom = 1.0f;
    if (m_zoom > 45.0f)
        m_zoom = 45.0f;
}

void Camera::UpdateCameraVectors() {
    // 根据欧拉角计算前向量
    // 优化：缓存弧度值，避免重复计算
    float yawRad = glm::radians(m_yaw);
    float pitchRad = glm::radians(m_pitch);
    float cosPitch = cos(pitchRad);
    
    glm::vec3 front;
    front.x = cos(yawRad) * cosPitch;
    front.y = sin(pitchRad);
    front.z = sin(yawRad) * cosPitch;
    m_front = glm::normalize(front);
    
    // 计算右向量和上向量
    m_right = glm::normalize(glm::cross(m_front, m_worldUp));
    m_up = glm::normalize(glm::cross(m_right, m_front));
}

} // namespace SoulsEngine

