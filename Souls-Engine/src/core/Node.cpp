#include "Node.h"
#include <algorithm>
#include <memory>

namespace SoulsEngine {

Node::Node(const std::string& name)
    : m_name(name)
    , m_parent(nullptr)
    , m_position(0.0f, 0.0f, 0.0f)
    , m_rotation(0.0f, 0.0f, 0.0f)
    , m_scale(1.0f, 1.0f, 1.0f)
    , m_localTransform(1.0f)
    , m_transformDirty(true)
{
}

Node::~Node() {
    // 从父节点移除自己
    if (m_parent) {
        m_parent->RemoveChild(this);
    }
    
    // 清除所有子节点的父指针
    for (auto& child : m_children) {
        if (child) {
            child->m_parent = nullptr;
        }
    }
    m_children.clear();
}

void Node::AddChild(std::shared_ptr<Node> child) {
    if (!child) return;
    
    // 如果子节点已有父节点，先从原父节点移除
    if (child->m_parent) {
        child->m_parent->RemoveChild(child);
    }
    
    m_children.push_back(child);
    child->m_parent = this;
}

void Node::RemoveChild(std::shared_ptr<Node> child) {
    if (!child) return;
    RemoveChild(child.get());
}

void Node::RemoveChild(Node* child) {
    if (!child) return;
    
    auto it = std::find_if(m_children.begin(), m_children.end(),
        [child](const std::shared_ptr<Node>& node) {
            return node.get() == child;
        });
    
    if (it != m_children.end()) {
        (*it)->m_parent = nullptr;
        m_children.erase(it);
    }
}

void Node::SetParent(Node* parent) {
    if (parent == m_parent) return;
    
    // 从原父节点移除（通过直接操作父节点的子节点列表）
    if (m_parent) {
        auto& parentChildren = m_parent->m_children;
        parentChildren.erase(
            std::remove_if(parentChildren.begin(), parentChildren.end(),
                [this](const std::shared_ptr<Node>& node) {
                    return node.get() == this;
                }),
            parentChildren.end()
        );
    }
    
    // 设置新父节点
    m_parent = parent;
    // 注意：SetParent不应该自动添加到父节点的子节点列表
    // 应该由外部通过AddChild来管理，避免循环引用
}

glm::mat4 Node::GetLocalTransform() const {
    if (m_transformDirty) {
        // 计算局部变换矩阵：T * R * S
        m_localTransform = glm::mat4(1.0f);
        m_localTransform = glm::scale(m_localTransform, m_scale);
        m_localTransform = glm::rotate(m_localTransform, glm::radians(m_rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
        m_localTransform = glm::rotate(m_localTransform, glm::radians(m_rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
        m_localTransform = glm::rotate(m_localTransform, glm::radians(m_rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
        m_localTransform = glm::translate(m_localTransform, m_position);
        m_transformDirty = false;
    }
    return m_localTransform;
}

glm::mat4 Node::GetWorldTransform() const {
    glm::mat4 worldTransform = GetLocalTransform();
    // 递归计算父节点变换
    Node* parent = m_parent;
    while (parent) {
        worldTransform = parent->GetLocalTransform() * worldTransform;
        parent = parent->m_parent;
    }
    return worldTransform;
}

void Node::Update() {
    // 更新自己的变换矩阵
    GetLocalTransform();
    
    // 更新所有子节点
    for (auto& child : m_children) {
        if (child) {
            child->Update();
        }
    }
}

glm::vec3 Node::LocalToWorld(const glm::vec3& localPoint) const {
    glm::mat4 worldTransform = GetWorldTransform();
    glm::vec4 worldPoint = worldTransform * glm::vec4(localPoint, 1.0f);
    return glm::vec3(worldPoint);
}

glm::vec3 Node::WorldToLocal(const glm::vec3& worldPoint) const {
    glm::mat4 worldTransform = GetWorldTransform();
    glm::mat4 inverseMatrix = glm::inverse(worldTransform);
    glm::vec4 localPoint = inverseMatrix * glm::vec4(worldPoint, 1.0f);
    return glm::vec3(localPoint);
}

glm::vec3 Node::LocalDirectionToWorld(const glm::vec3& localDirection) const {
    // 方向向量不考虑平移
    glm::mat4 worldTransform = GetWorldTransform();
    glm::mat4 rotationScaleMatrix = worldTransform;
    rotationScaleMatrix[3] = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    glm::vec4 worldDirection = rotationScaleMatrix * glm::vec4(localDirection, 0.0f);
    return glm::normalize(glm::vec3(worldDirection));
}

glm::vec3 Node::WorldDirectionToLocal(const glm::vec3& worldDirection) const {
    // 方向向量不考虑平移
    glm::mat4 worldTransform = GetWorldTransform();
    glm::mat4 rotationScaleMatrix = worldTransform;
    rotationScaleMatrix[3] = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    glm::mat4 inverseMatrix = glm::inverse(rotationScaleMatrix);
    glm::vec4 localDirection = inverseMatrix * glm::vec4(worldDirection, 0.0f);
    return glm::normalize(glm::vec3(localDirection));
}

} // namespace SoulsEngine

