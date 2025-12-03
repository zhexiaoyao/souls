#pragma once

#include "Node.h"
#include <memory>

namespace SoulsEngine {

// 前向声明
class Mesh;
class Shader;

// 场景节点（可以附加Mesh）
class SceneNode : public Node {
public:
    SceneNode(const std::string& name = "SceneNode");
    virtual ~SceneNode() = default;

    // 设置Mesh
    void SetMesh(std::shared_ptr<Mesh> mesh) { m_mesh = mesh; }
    std::shared_ptr<Mesh> GetMesh() const { return m_mesh; }

    // 渲染（重写基类方法）
    virtual void Render(const glm::mat4& parentTransform, Shader* shader) override;
    
    // 渲染线框（用于选中高亮）
    void RenderWireframe(const glm::mat4& parentTransform, Shader* shader);

private:
    std::shared_ptr<Mesh> m_mesh;
};

} // namespace SoulsEngine

