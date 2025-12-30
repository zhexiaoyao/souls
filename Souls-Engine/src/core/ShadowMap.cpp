#include "ShadowMap.h"
#include <glad/glad.h>
#include <iostream>

namespace SoulsEngine {

ShadowMap::ShadowMap(unsigned int width, unsigned int height)
    : m_width(width)
    , m_height(height)
    , m_depthMapFBO(0)
    , m_depthTexture(0)
{
}

ShadowMap::~ShadowMap() {
    if (m_depthMapFBO != 0) {
        glDeleteFramebuffers(1, &m_depthMapFBO);
        m_depthMapFBO = 0;
    }
    if (m_depthTexture != 0) {
        glDeleteTextures(1, &m_depthTexture);
        m_depthTexture = 0;
    }
}

bool ShadowMap::Initialize() {
    // 创建帧缓冲对象
    glGenFramebuffers(1, &m_depthMapFBO);

    // 创建深度纹理
    glGenTextures(1, &m_depthTexture);
    glBindTexture(GL_TEXTURE_2D, m_depthTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 
                 m_width, m_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    
    // 设置边界颜色（超出阴影贴图范围的地方不在阴影中）
    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    // 将深度纹理附加到帧缓冲
    glBindFramebuffer(GL_FRAMEBUFFER, m_depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depthTexture, 0);
    
    // 不绘制颜色数据
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    // 检查帧缓冲完整性
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "ERROR::SHADOWMAP:: Framebuffer is not complete!" << std::endl;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        return false;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return true;
}

void ShadowMap::BeginRender() {
    glViewport(0, 0, m_width, m_height);
    glBindFramebuffer(GL_FRAMEBUFFER, m_depthMapFBO);
    glClear(GL_DEPTH_BUFFER_BIT);
    
    // 启用正面剔除（只渲染背面，避免阴影痤疮）
    glCullFace(GL_FRONT);
}

void ShadowMap::EndRender() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glCullFace(GL_BACK);  // 恢复背面剔除
    
    // 恢复视口（需要在调用处设置正确的视口大小）
}

glm::mat4 ShadowMap::GetLightSpaceMatrix(const glm::vec3& lightPos,
                                          const glm::vec3& lightDir,
                                          float nearPlane,
                                          float farPlane,
                                          float orthoSize) const {
    // 计算光源的视图矩阵
    // lightDir是光源方向（从光源指向目标），所以target = lightPos + lightDir
    glm::vec3 lightTarget = lightPos + lightDir * 10.0f;  // 确保target在合理距离
    
    // 计算up向量（确保与lightDir不平行）
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    if (abs(glm::dot(lightDir, up)) > 0.9f) {
        // 如果lightDir几乎与up平行，使用另一个up向量
        up = glm::vec3(0.0f, 0.0f, 1.0f);
    }
    
    glm::mat4 lightView = glm::lookAt(lightPos, lightTarget, up);
    
    // 正交投影矩阵（适合方向光）
    // 使用更大的正交投影范围以覆盖整个场景
    glm::mat4 lightProjection = glm::ortho(-orthoSize, orthoSize, -orthoSize, orthoSize, nearPlane, farPlane);
    
    // 光源空间矩阵 = 投影矩阵 * 视图矩阵
    return lightProjection * lightView;
}

} // namespace SoulsEngine

