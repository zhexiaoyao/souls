#include "SceneNode.h"
#include "Shader.h"
#include "Material.h"
#include "../geometry/Mesh.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace SoulsEngine {

SceneNode::SceneNode(const std::string& name)
    : Node(name)
    , m_mesh(nullptr)
    , m_material(nullptr)
{
}

void SceneNode::Render(const glm::mat4& parentTransform, Shader* shader) {
    if (!m_mesh || !shader) return;

    // 计算世界变换矩阵（父节点变换 * 局部变换）
    glm::mat4 worldTransform = parentTransform * GetLocalTransform();

    // 设置世界变换矩阵到Shader
    shader->SetMat4("model", glm::value_ptr(worldTransform));

    // 获取材质信息（如果有材质则使用，否则使用默认材质）
    const Material* activeMaterial = m_material ? m_material.get() : nullptr;
    if (!activeMaterial) {
        static Material defaultMat = Material::CreateDefault();
        activeMaterial = &defaultMat;
    }

    glm::vec3 ambient = activeMaterial->GetAmbient();
    glm::vec3 diffuse = activeMaterial->GetDiffuse();
    glm::vec3 specular = activeMaterial->GetSpecular();
    float shininess = activeMaterial->GetShininess();
    float alpha = activeMaterial->GetAlpha();

    shader->SetVec3("material.ambient", ambient.r, ambient.g, ambient.b);
    shader->SetVec3("material.diffuse", diffuse.r, diffuse.g, diffuse.b);
    shader->SetVec3("material.specular", specular.r, specular.g, specular.b);
    shader->SetFloat("material.shininess", shininess);
    shader->SetFloat("material.alpha", alpha);

    // 渲染网格
    m_mesh->Draw();

    // 递归渲染所有子节点
    for (auto& child : GetChildren()) {
        auto sceneNode = std::dynamic_pointer_cast<SceneNode>(child);
        if (sceneNode) {
            sceneNode->Render(worldTransform, shader);
        }
    }
}

void SceneNode::RenderWireframe(const glm::mat4& parentTransform, Shader* shader) {
    if (!m_mesh || !shader) return;

    // 计算世界变换矩阵（父节点变换 * 局部变换）
    glm::mat4 worldTransform = parentTransform * GetLocalTransform();
    
    // 稍微放大一点以便线框显示在实体模型外面
    glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.02f, 1.02f, 1.02f));
    glm::mat4 scaledTransform = worldTransform * scaleMatrix;

    // 设置缩放后的变换矩阵到Shader
    shader->SetMat4("model", glm::value_ptr(scaledTransform));

    // 渲染线框（使用线框模式绘制网格）
    m_mesh->DrawWireframe();

    // 递归渲染所有子节点的线框
    for (auto& child : GetChildren()) {
        auto sceneNode = std::dynamic_pointer_cast<SceneNode>(child);
        if (sceneNode) {
            sceneNode->RenderWireframe(worldTransform, shader);
        }
    }
}

} // namespace SoulsEngine

