#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace SoulsEngine {

// 第一人称视角相机类
class Camera {
public:
    // 相机移动方向枚举
    enum CameraMovement {
        FORWARD,
        BACKWARD,
        LEFT,
        RIGHT,
        UP,
        DOWN
    };

    // 构造函数
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 3.0f),
           glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
           float yaw = -90.0f,
           float pitch = 0.0f);
    
    // 获取视图矩阵
    glm::mat4 GetViewMatrix() const;
    
    // 获取投影矩阵
    glm::mat4 GetProjectionMatrix(float aspectRatio) const;
    
    // 获取MVP矩阵（Model-View-Projection）
    glm::mat4 GetMVPMatrix(const glm::mat4& model, float aspectRatio) const;
    
    // 相机移动
    void ProcessKeyboard(CameraMovement direction, float deltaTime);
    void ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch = true);
    void ProcessMouseScroll(float yoffset);
    
    // 获取相机属性
    glm::vec3 GetPosition() const { return m_position; }
    glm::vec3 GetFront() const { return m_front; }
    glm::vec3 GetUp() const { return m_up; }
    glm::vec3 GetRight() const { return m_right; }
    float GetZoom() const { return m_zoom; }
    
    // 设置相机属性
    void SetPosition(const glm::vec3& position) { m_position = position; }
    void SetZoom(float zoom) { m_zoom = zoom; }

private:
    // 相机属性
    glm::vec3 m_position;
    glm::vec3 m_front;
    glm::vec3 m_up;
    glm::vec3 m_right;
    glm::vec3 m_worldUp;
    
    // 欧拉角
    float m_yaw;
    float m_pitch;
    
    // 相机选项
    float m_movementSpeed;
    float m_mouseSensitivity;
    float m_zoom;
    
    // 投影参数
    float m_fov;
    float m_nearPlane;
    float m_farPlane;
    
    // 根据欧拉角更新相机向量
    void UpdateCameraVectors();
};

} // namespace SoulsEngine

