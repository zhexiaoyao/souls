#pragma once

#include "Node.h"
#include <memory>

namespace SoulsEngine {

// 鍓嶅悜澹版槑
class Mesh;
class Shader;
class Material;

// 鍦烘櫙鑺傜偣锛堝彲浠ラ檮鍔燤esh鍜孧aterial锟�?
class SceneNode : public Node {
public:
    SceneNode(const std::string& name = "SceneNode");
    virtual ~SceneNode() = default;

    // 璁剧疆Mesh
    void SetMesh(std::shared_ptr<Mesh> mesh) { m_mesh = mesh; }
    std::shared_ptr<Mesh> GetMesh() const { return m_mesh; }

    // 璁剧疆鏉愯川
    void SetMaterial(std::shared_ptr<Material> material) { m_material = material; }
    std::shared_ptr<Material> GetMaterial() const { return m_material; }

    // 娓叉煋锛堥噸鍐欏熀绫绘柟娉曪級
    virtual void Render(const glm::mat4& parentTransform, Shader* shader) override;
    
    // 娓叉煋绾挎锛堢敤浜庨€変腑楂樹寒锟�?
    void RenderWireframe(const glm::mat4& parentTransform, Shader* shader);

private:
    std::shared_ptr<Mesh> m_mesh;
    std::shared_ptr<Material> m_material;  // 鏉愯川
};

} // namespace SoulsEngine

