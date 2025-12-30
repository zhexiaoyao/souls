#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>

namespace SoulsEngine {

// 网格基类，管理 OpenGL 顶点数据
class Mesh {
public:
    Mesh();
    virtual ~Mesh();

    // 禁止拷贝
    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;

    // 渲染网格
    void Draw() const;
    
    // 渲染线框（用于选中高亮）
    void DrawWireframe() const;

    // 获取顶点数量
    size_t GetVertexCount() const { return m_vertexCount; }

protected:
    GLuint m_VAO;              // 顶点数组对象
    GLuint m_VBO;              // 顶点缓冲对象
    size_t m_vertexCount;      // 顶点数量

    // 初始化网格数据（由子类调用）
    // 顶点格式：每个顶点6个float（3个位置 + 3个颜色）
    void SetupMesh(const std::vector<float>& vertices);
};

} // namespace SoulsEngine

