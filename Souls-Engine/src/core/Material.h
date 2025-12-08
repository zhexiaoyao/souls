#pragma once

#include <glm/glm.hpp>
#include <string>

namespace SoulsEngine {

// 材质类 - 封装材质属性
class Material {
public:
    Material();
    Material(const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular, float shininess);
    ~Material() = default;

    // 环境光颜色（Ambient）
    void SetAmbient(const glm::vec3& ambient) { m_ambient = ambient; }
    void SetAmbient(float r, float g, float b) { m_ambient = glm::vec3(r, g, b); }
    glm::vec3 GetAmbient() const { return m_ambient; }

    // 漫反射颜色（Diffuse）
    void SetDiffuse(const glm::vec3& diffuse) { m_diffuse = diffuse; }
    void SetDiffuse(float r, float g, float b) { m_diffuse = glm::vec3(r, g, b); }
    glm::vec3 GetDiffuse() const { return m_diffuse; }

    // 镜面反射颜色（Specular）
    void SetSpecular(const glm::vec3& specular) { m_specular = specular; }
    void SetSpecular(float r, float g, float b) { m_specular = glm::vec3(r, g, b); }
    glm::vec3 GetSpecular() const { return m_specular; }

    // 光泽度（Shininess）
    void SetShininess(float shininess) { m_shininess = shininess; }
    float GetShininess() const { return m_shininess; }

    // 设置基础颜色（同时设置环境光和漫反射）
    void SetColor(const glm::vec3& color) {
        m_ambient = color * 0.2f;  // 环境光通常是基础颜色的20%
        m_diffuse = color;
    }
    void SetColor(float r, float g, float b) {
        SetColor(glm::vec3(r, g, b));
    }
    glm::vec3 GetColor() const { return m_diffuse; }

    // 材质名称
    void SetName(const std::string& name) { m_name = name; }
    std::string GetName() const { return m_name; }

    // 创建预设材质
    static Material CreateDefault();
    static Material CreateEmerald();
    static Material CreateJade();
    static Material CreateRuby();
    static Material CreateGold();
    static Material CreateSilver();

private:
    std::string m_name;
    glm::vec3 m_ambient;      // 环境光颜色
    glm::vec3 m_diffuse;      // 漫反射颜色
    glm::vec3 m_specular;     // 镜面反射颜色
    float m_shininess;        // 光泽度（0-128）
};

} // namespace SoulsEngine

