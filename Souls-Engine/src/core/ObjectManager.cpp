#include "ObjectManager.h"
#include "Shader.h"
#include "../geometry/Mesh.h"

namespace SoulsEngine {

ObjectManager::ObjectManager() {
}

ObjectManager::~ObjectManager() {
    Clear();
}

std::shared_ptr<SceneNode> ObjectManager::CreateNode(const std::string& name, std::shared_ptr<Mesh> mesh) {
    auto node = std::make_shared<SceneNode>(name);
    node->SetMesh(mesh);
    
    // 添加到场景和映射表
    m_scene.AddNode(node);
    m_nodes[name] = node;
    
    return node;
}

void ObjectManager::AddNode(std::shared_ptr<SceneNode> node) {
    if (!node) return;
    
    std::string name = node->GetName();
    m_scene.AddNode(node);
    m_nodes[name] = node;
}

void ObjectManager::RemoveNode(const std::string& name) {
    auto it = m_nodes.find(name);
    if (it != m_nodes.end()) {
        m_scene.RemoveNode(it->second);
        m_nodes.erase(it);
    }
}

void ObjectManager::RemoveNode(std::shared_ptr<SceneNode> node) {
    if (!node) return;
    
    std::string name = node->GetName();
    m_scene.RemoveNode(node);
    m_nodes.erase(name);
}

std::shared_ptr<SceneNode> ObjectManager::FindNode(const std::string& name) const {
    auto it = m_nodes.find(name);
    if (it != m_nodes.end()) {
        return it->second;
    }
    
    // 如果映射表中没有，尝试从场景中查找
    auto sceneNode = m_scene.FindNodeByName(name);
    return std::dynamic_pointer_cast<SceneNode>(sceneNode);
}

std::vector<std::shared_ptr<SceneNode>> ObjectManager::GetAllNodes() const {
    std::vector<std::shared_ptr<SceneNode>> nodes;
    nodes.reserve(m_nodes.size());
    
    for (const auto& pair : m_nodes) {
        nodes.push_back(pair.second);
    }
    
    return nodes;
}

void ObjectManager::Clear() {
    m_scene.GetRoot()->GetChildren().clear();
    m_nodes.clear();
}

void ObjectManager::Update() {
    m_scene.Update();
}

void ObjectManager::Render(Shader* shader) {
    m_scene.Render(shader);
}

} // namespace SoulsEngine

