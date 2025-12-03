#include "Mesh.h"
#include <glad/glad.h>

namespace SoulsEngine {

Mesh::Mesh() : m_VAO(0), m_VBO(0), m_vertexCount(0) {
}

Mesh::~Mesh() {
    if (m_VBO != 0) {
        glDeleteBuffers(1, &m_VBO);
        m_VBO = 0;
    }
    if (m_VAO != 0) {
        glDeleteVertexArrays(1, &m_VAO);
        m_VAO = 0;
    }
}

void Mesh::SetupMesh(const std::vector<float>& vertices) {
    m_vertexCount = vertices.size() / 6; // 每个顶点6个float

    // 创建VAO和VBO
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);

    // 绑定VAO
    glBindVertexArray(m_VAO);

    // 绑定VBO并上传数据
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, 
                 static_cast<GLsizeiptr>(vertices.size() * sizeof(float)), 
                 vertices.data(), 
                 GL_STATIC_DRAW);

    // 设置顶点属性
    // 位置属性 (location = 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    // 颜色属性 (location = 1)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // 解绑
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Mesh::Draw() const {
    if (m_VAO != 0 && m_vertexCount > 0) {
        glBindVertexArray(m_VAO);
        glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(m_vertexCount));
        glBindVertexArray(0);
    }
}

void Mesh::DrawWireframe() const {
    // 线框效果通过 SceneNode::RenderWireframe 实现
    if (m_VAO != 0 && m_vertexCount > 0) {
        // 注意：glPolygonMode(GL_LINE) 在 OpenGL Core Profile 中不可用
        // 我们使用一个简化的方法：直接渲染填充模式，边框效果通过
        // SceneNode::RenderWireframe 中的缩放变换和shader的覆盖颜色来实现
        
        // 渲染边框（使用填充模式，但通过shader的覆盖颜色来实现黑色边框效果）
        // 边框效果通过稍微放大对象（在SceneNode中实现）和黑色覆盖颜色实现
        glBindVertexArray(m_VAO);
        glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(m_vertexCount));
        glBindVertexArray(0);
    }
}

} // namespace SoulsEngine

