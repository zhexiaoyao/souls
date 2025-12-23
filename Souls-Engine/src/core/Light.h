#pragma once

#include <glm/glm.hpp>
#include <string>

namespace SoulsEngine {

// 光源类
class Light {
public:
    Light();
    Light(const glm::vec3& position, const glm::vec3& color, float intensity, float angle);
    ~Light() = default;

    // 位置
    void SetPosition(const glm::vec3& position) { m_position = position; }
    void SetPosition(float x, float y, float z) { m_position = glm::vec3(x, y, z); }
    glm::vec3 GetPosition() const { return m_position; }

    // 颜色
    void SetColor(const glm::vec3& color) { m_color = color; }
    void SetColor(float r, float g, float b) { m_color = glm::vec3(r, g, b); }
    glm::vec3 GetColor() const { return m_color; }

    // 强度
    void SetIntensity(float intensity) { m_intensity = intensity; }
    float GetIntensity() const { return m_intensity; }

    // 光照张角（0-360度）
    void SetAngle(float angle) { m_angle = angle; }
    float GetAngle() const { return m_angle; }

    // 光源名称
    void SetName(const std::string& name) { m_name = name; }
    std::string GetName() const { return m_name; }

private:
    std::string m_name;
    glm::vec3 m_position;    // 光源位置
    glm::vec3 m_color;       // 光源颜色
    float m_intensity;       // 光照强度
    float m_angle;           // 光照张角（0-360度）
};

} // namespace SoulsEngine










