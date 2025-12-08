#include "SceneNode.h"
#include "Shader.h"
// 材质系统已删除
// #include "Material.h"
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

    // 计算世界变换矩阵
    glm::mat4 worldTransform = parentTransform * GetLocalTransform();

    // 设置模型矩阵到Shader
    shader->SetMat4("model", glm::value_ptr(worldTransform));

    // 材质系统已删除，始终使用默认材质
    // if (m_material) {
    //     glm::vec3 ambient = m_material->GetAmbient();
    //     glm::vec3 diffuse = m_material->GetDiffuse();
    //     glm::vec3 specular = m_material->GetSpecular();
    //     float shininess = m_material->GetShininess();
    //
    //     shader->SetVec3("material.ambient", ambient.r, ambient.g, ambient.b);
    //     shader->SetVec3("material.diffuse", diffuse.r, diffuse.g, diffuse.b);
    //     shader->SetVec3("material.specular", specular.r, specular.g, specular.b);
    //     shader->SetFloat("material.shininess", shininess);
    // } else {
    // 使用默认材质
    shader->SetVec3("material.ambient", 0.2f, 0.2f, 0.2f);
    shader->SetVec3("material.diffuse", 0.8f, 0.8f, 0.8f);
    shader->SetVec3("material.specular", 1.0f, 1.0f, 1.0f);
    shader->SetFloat("material.shininess", 32.0f);
    // }

    // 渲染Mesh
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

    // 计算世界变换矩阵
    glm::mat4 worldTransform = parentTransform * GetLocalTransform();
    
    // 稍微放大模型以实现边框效果
    glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.02f, 1.02f, 1.02f));
    glm::mat4 scaledTransform = worldTransform * scaleMatrix;

    // 设置模型矩阵到Shader
    shader->SetMat4("model", glm::value_ptr(scaledTransform));

    // 渲染边框（使用黑色覆盖颜色）
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

