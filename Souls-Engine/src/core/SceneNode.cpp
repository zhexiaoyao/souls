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

    // 璁＄畻涓栫晫鍙樻崲鐭╅樀
    glm::mat4 worldTransform = parentTransform * GetLocalTransform();

    // 璁剧疆妯″瀷鐭╅樀鍒癝hader
    shader->SetMat4("model", glm::value_ptr(worldTransform));

    // 应用材质参数，若有实实的结构，则使用自定义按�?
    const Material* activeMaterial = m_material ? m_material.get() : nullptr;
    if (!activeMaterial) {
        static Material defaultMat = Material::CreateDefault();
        activeMaterial = &defaultMat;
    }

    glm::vec3 ambient = activeMaterial->GetAmbient();
    glm::vec3 diffuse = activeMaterial->GetDiffuse();
    glm::vec3 specular = activeMaterial->GetSpecular();
    float shininess = activeMaterial->GetShininess();

    shader->SetVec3("material.ambient", ambient.r, ambient.g, ambient.b);
    shader->SetVec3("material.diffuse", diffuse.r, diffuse.g, diffuse.b);
    shader->SetVec3("material.specular", specular.r, specular.g, specular.b);
    shader->SetFloat("material.shininess", shininess);

    // 娓叉煋Mesh
    m_mesh->Draw();

    // 閫掑綊娓叉煋鎵€鏈夊瓙鑺傜偣
    for (auto& child : GetChildren()) {
        auto sceneNode = std::dynamic_pointer_cast<SceneNode>(child);
        if (sceneNode) {
            sceneNode->Render(worldTransform, shader);
        }
    }
}

void SceneNode::RenderWireframe(const glm::mat4& parentTransform, Shader* shader) {
    if (!m_mesh || !shader) return;

    // 璁＄畻涓栫晫鍙樻崲鐭╅樀
    glm::mat4 worldTransform = parentTransform * GetLocalTransform();
    
    // 绋嶅井鏀惧ぇ妯″瀷浠ュ疄鐜拌竟妗嗘晥鏋�
    glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.02f, 1.02f, 1.02f));
    glm::mat4 scaledTransform = worldTransform * scaleMatrix;

    // 璁剧疆妯″瀷鐭╅樀鍒癝hader
    shader->SetMat4("model", glm::value_ptr(scaledTransform));

    // 娓叉煋杈规锛堜娇鐢ㄩ粦鑹茶鐩栭鑹诧�?
    m_mesh->DrawWireframe();

    // 閫掑綊娓叉煋鎵€鏈夊瓙鑺傜偣鐨勭嚎妗�?
    for (auto& child : GetChildren()) {
        auto sceneNode = std::dynamic_pointer_cast<SceneNode>(child);
        if (sceneNode) {
            sceneNode->RenderWireframe(worldTransform, shader);
        }
    }
}

} // namespace SoulsEngine

