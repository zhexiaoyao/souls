#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>
#include <vector>
#include <memory>

namespace SoulsEngine {

// 前向声明
class Mesh;

// 场景节点类
class Node {
public:
    Node(const std::string& name = "Node");
    virtual ~Node();

    // 禁止拷贝
    Node(const Node&) = delete;
    Node& operator=(const Node&) = delete;

    // 节点名称
    std::string GetName() const { return m_name; }
    void SetName(const std::string& name) { m_name = name; }

    // 父子关系
    void AddChild(std::shared_ptr<Node> child);
    void RemoveChild(std::shared_ptr<Node> child);
    void RemoveChild(Node* child);
    void SetParent(Node* parent);
    Node* GetParent() const { return m_parent; }
    const std::vector<std::shared_ptr<Node>>& GetChildren() const { return m_children; }
    std::vector<std::shared_ptr<Node>>& GetChildren() { return m_children; }

    // 变换 - 设置
    void SetPosition(const glm::vec3& position) { m_position = position; m_transformDirty = true; }
    void SetPosition(float x, float y, float z) { m_position = glm::vec3(x, y, z); m_transformDirty = true; }
    void SetRotation(const glm::vec3& rotation) { m_rotation = rotation; m_transformDirty = true; }
    void SetRotation(float x, float y, float z) { m_rotation = glm::vec3(x, y, z); m_transformDirty = true; }
    void SetScale(const glm::vec3& scale) { m_scale = scale; m_transformDirty = true; }
    void SetScale(float x, float y, float z) { m_scale = glm::vec3(x, y, z); m_transformDirty = true; }
    void SetScale(float uniform) { m_scale = glm::vec3(uniform); m_transformDirty = true; }
    
    // 变换 - 获取
    glm::vec3 GetPosition() const { return m_position; }
    glm::vec3 GetRotation() const { return m_rotation; }
    glm::vec3 GetScale() const { return m_scale; }
    
    // 变换 - 相对变换（增量）
    void Translate(const glm::vec3& translation) { m_position += translation; m_transformDirty = true; }
    void Translate(float x, float y, float z) { m_position += glm::vec3(x, y, z); m_transformDirty = true; }
    void Rotate(const glm::vec3& rotation) { m_rotation += rotation; m_transformDirty = true; }
    void Rotate(float x, float y, float z) { m_rotation += glm::vec3(x, y, z); m_transformDirty = true; }
    void RotateX(float angle) { m_rotation.x += angle; m_transformDirty = true; }
    void RotateY(float angle) { m_rotation.y += angle; m_transformDirty = true; }
    void RotateZ(float angle) { m_rotation.z += angle; m_transformDirty = true; }
    void Scale(const glm::vec3& scale) { m_scale *= scale; m_transformDirty = true; }
    void Scale(float uniform) { m_scale *= uniform; m_transformDirty = true; }
    
    // 坐标转换
    glm::vec3 LocalToWorld(const glm::vec3& localPoint) const;
    glm::vec3 WorldToLocal(const glm::vec3& worldPoint) const;
    glm::vec3 LocalDirectionToWorld(const glm::vec3& localDirection) const;
    glm::vec3 WorldDirectionToLocal(const glm::vec3& worldDirection) const;

    // 变换矩阵
    glm::mat4 GetLocalTransform() const;
    glm::mat4 GetWorldTransform() const;

    // 更新和渲染
    virtual void Update();
    virtual void Render(const glm::mat4& parentTransform, class Shader* shader) {}

protected:
    std::string m_name;
    Node* m_parent;
    mutable std::vector<std::shared_ptr<Node>> m_children;

    // 局部变换
    glm::vec3 m_position;
    glm::vec3 m_rotation;  // 欧拉角（度）
    glm::vec3 m_scale;

    // 变换矩阵缓存
    mutable glm::mat4 m_localTransform;
    mutable bool m_transformDirty;
};

} // namespace SoulsEngine

