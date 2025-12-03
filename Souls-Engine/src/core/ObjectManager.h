#pragma once

#include "Scene.h"
#include "SceneNode.h"
#include <memory>
#include <string>
#include <unordered_map>

namespace SoulsEngine {

// 前向声明
class Mesh;

// 对象管理器类
class ObjectManager {
public:
    ObjectManager();
    ~ObjectManager();

    // 禁止拷贝
    ObjectManager(const ObjectManager&) = delete;
    ObjectManager& operator=(const ObjectManager&) = delete;

    // 获取场景
    Scene* GetScene() { return &m_scene; }
    const Scene* GetScene() const { return &m_scene; }

    // 创建场景节点（带Mesh）
    std::shared_ptr<SceneNode> CreateNode(const std::string& name, std::shared_ptr<Mesh> mesh);

    // 添加节点到场景
    void AddNode(std::shared_ptr<SceneNode> node);

    // 移除节点
    void RemoveNode(const std::string& name);
    void RemoveNode(std::shared_ptr<SceneNode> node);

    // 查找节点
    std::shared_ptr<SceneNode> FindNode(const std::string& name) const;

    // 获取所有节点
    std::vector<std::shared_ptr<SceneNode>> GetAllNodes() const;

    // 清空所有节点
    void Clear();

    // 更新场景
    void Update();

    // 渲染场景
    void Render(class Shader* shader);

private:
    Scene m_scene;
    std::unordered_map<std::string, std::shared_ptr<SceneNode>> m_nodes;
};

} // namespace SoulsEngine

