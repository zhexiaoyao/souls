#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace SoulsEngine {

// 阴影映射类 - 管理阴影贴图的生成和使用
class ShadowMap {
public:
    ShadowMap(unsigned int width = 2048, unsigned int height = 2048);
    ~ShadowMap();

    // 禁止拷贝
    ShadowMap(const ShadowMap&) = delete;
    ShadowMap& operator=(const ShadowMap&) = delete;

    // 初始化阴影贴图
    bool Initialize();

    // 开始渲染到阴影贴图
    void BeginRender();

    // 结束渲染到阴影贴图
    void EndRender();

    // 获取深度纹理ID
    GLuint GetDepthTexture() const { return m_depthTexture; }

    // 获取阴影贴图尺寸
    unsigned int GetWidth() const { return m_width; }
    unsigned int GetHeight() const { return m_height; }

    // 计算光源空间矩阵（从光源视角的视图投影矩阵）
    glm::mat4 GetLightSpaceMatrix(const glm::vec3& lightPos, 
                                   const glm::vec3& lightDir,
                                   float nearPlane = 0.1f,
                                   float farPlane = 100.0f,
                                   float orthoSize = 20.0f) const;

private:
    unsigned int m_width;
    unsigned int m_height;
    GLuint m_depthMapFBO;      // 帧缓冲对象
    GLuint m_depthTexture;     // 深度纹理
};

} // namespace SoulsEngine








