#pragma once

#include "Node.h"
#include "SceneNode.h"
#include <memory>
#include <vector>

namespace SoulsEngine {

// 前向声明
class Shader;

// 场景图类
class Scene {
public:
    Scene();
    ~Scene();

    // 禁止拷贝
    Scene(const Scene&) = delete;
    Scene& operator=(const Scene&) = delete;

    // 获取根节点
    std::shared_ptr<Node> GetRoot() const { return m_root; }

    // 添加节点到根节点
    void AddNode(std::shared_ptr<Node> node);
    void RemoveNode(std::shared_ptr<Node> node);

    // 场景遍历
    void Update();
    void Render(Shader* shader);

    // 查找节点
    std::shared_ptr<Node> FindNodeByName(const std::string& name) const;

private:
    std::shared_ptr<Node> m_root;

    // 递归查找节点
    std::shared_ptr<Node> FindNodeRecursive(std::shared_ptr<Node> node, const std::string& name) const;
};

} // namespace SoulsEngine

