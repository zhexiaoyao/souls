#include "Material.h"

namespace SoulsEngine {

Material::Material()
    : m_name("DefaultMaterial")
    , m_ambient(0.2f, 0.2f, 0.2f)
    , m_diffuse(0.8f, 0.8f, 0.8f)
    , m_specular(1.0f, 1.0f, 1.0f)
    , m_shininess(32.0f)
    , m_alpha(1.0f)
{
}

Material::Material(const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular, float shininess, float alpha)
    : m_name("CustomMaterial")
    , m_ambient(ambient)
    , m_diffuse(diffuse)
    , m_specular(specular)
    , m_shininess(shininess)
    , m_alpha(alpha)
{
}

Material Material::CreateDefault() {
    Material mat;
    mat.SetName("Default");
    return mat;
}

Material Material::CreateEmerald() {
    Material mat(
        glm::vec3(0.0215f, 0.1745f, 0.0215f),  // ambient
        glm::vec3(0.07568f, 0.61424f, 0.07568f),  // diffuse
        glm::vec3(0.633f, 0.727811f, 0.633f),  // specular
        76.8f,  // shininess
        1.0f    // alpha
    );
    mat.SetName("Emerald");
    return mat;
}

Material Material::CreateJade() {
    Material mat(
        glm::vec3(0.135f, 0.2225f, 0.1575f),  // ambient
        glm::vec3(0.54f, 0.89f, 0.63f),  // diffuse
        glm::vec3(0.316228f, 0.316228f, 0.316228f),  // specular
        12.8f,  // shininess
        1.0f    // alpha
    );
    mat.SetName("Jade");
    return mat;
}

Material Material::CreateRuby() {
    Material mat(
        glm::vec3(0.1745f, 0.01175f, 0.01175f),  // ambient
        glm::vec3(0.61424f, 0.04136f, 0.04136f),  // diffuse
        glm::vec3(0.727811f, 0.626959f, 0.626959f),  // specular
        76.8f,  // shininess
        1.0f    // alpha
    );
    mat.SetName("Ruby");
    return mat;
}

Material Material::CreateGold() {
    Material mat(
        glm::vec3(0.24725f, 0.1995f, 0.0745f),  // ambient
        glm::vec3(0.75164f, 0.60648f, 0.22648f),  // diffuse
        glm::vec3(0.628281f, 0.555802f, 0.366065f),  // specular
        51.2f,  // shininess
        1.0f    // alpha
    );
    mat.SetName("Gold");
    return mat;
}

Material Material::CreateSilver() {
    Material mat(
        glm::vec3(0.19225f, 0.19225f, 0.19225f),  // ambient
        glm::vec3(0.50754f, 0.50754f, 0.50754f),  // diffuse
        glm::vec3(0.508273f, 0.508273f, 0.508273f),  // specular
        51.2f,  // shininess
        1.0f    // alpha
    );
    mat.SetName("Silver");
    return mat;
}

Material Material::CreateGlass() {
    // 玻璃材质：低环境光、低漫反射、高镜面反射、蓝绿色调、半透明
    Material mat(
        glm::vec3(0.0f, 0.05f, 0.05f),     // ambient
        glm::vec3(0.1f, 0.3f, 0.35f),      // diffuse
        glm::vec3(0.9f, 0.9f, 0.95f),      // specular
        96.0f,                             // shininess
        0.25f                              // alpha
    );
    mat.SetName("Glass");
    return mat;
}

Material Material::CreateMetal() {
    // 金属材质：非常高的高光，较低的反射，颜色偏冷色调
    Material mat(
        glm::vec3(0.15f, 0.15f, 0.17f),    // ambient
        glm::vec3(0.3f, 0.3f, 0.32f),      // diffuse
        glm::vec3(0.95f, 0.95f, 0.97f),    // specular
        128.0f,                            // shininess
        1.0f                               // alpha
    );
    mat.SetName("Metal");
    return mat;
}

} // namespace SoulsEngine

