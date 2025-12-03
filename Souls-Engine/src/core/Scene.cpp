#include "Scene.h"
#include "Shader.h"
#include <algorithm>

namespace SoulsEngine {

Scene::Scene() {
    m_root = std::make_shared<Node>("Root");
}

Scene::~Scene() {
    m_root.reset();
}

void Scene::AddNode(std::shared_ptr<Node> node) {
    if (node && m_root) {
        m_root->AddChild(node);
    }
}

void Scene::RemoveNode(std::shared_ptr<Node> node) {
    if (node && m_root) {
        m_root->RemoveChild(node);
    }
}

void Scene::Update() {
    if (m_root) {
        m_root->Update();
    }
}

void Scene::Render(Shader* shader) {
    if (!shader || !m_root) return;

    glm::mat4 identity = glm::mat4(1.0f);

    // 遍历所有子节点并渲染
    for (auto& child : m_root->GetChildren()) {
        auto sceneNode = std::dynamic_pointer_cast<SceneNode>(child);
        if (sceneNode) {
            sceneNode->Render(identity, shader);
        }
    }
}

std::shared_ptr<Node> Scene::FindNodeByName(const std::string& name) const {
    if (!m_root) return nullptr;
    
    // 先检查根节点
    if (m_root->GetName() == name) {
        return m_root;
    }
    
    // 递归查找子节点
    return FindNodeRecursive(m_root, name);
}

std::shared_ptr<Node> Scene::FindNodeRecursive(std::shared_ptr<Node> node, const std::string& name) const {
    if (!node) return nullptr;
    
    // 检查当前节点
    if (node->GetName() == name) {
        return node;
    }
    
    // 递归检查子节点
    for (auto& child : node->GetChildren()) {
        auto result = FindNodeRecursive(child, name);
        if (result) {
            return result;
        }
    }
    
    return nullptr;
}

} // namespace SoulsEngine

