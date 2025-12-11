#include "Texture.h"
#include <glad/glad.h>
#include <iostream>
#include <filesystem>

// 包含stb_image头文�?
// 注意：需要从 https://github.com/nothings/stb 下载完整�? stb_image.h
// 并替�? extern/stb/stb_image.h
#include "stb_image.h"

namespace SoulsEngine {

Texture::Texture()
    : m_textureID(0)
    , m_width(0)
    , m_height(0)
    , m_channels(0)
{
}

Texture::~Texture() {
    if (m_textureID != 0) {
        glDeleteTextures(1, &m_textureID);
        m_textureID = 0;
    }
}

bool Texture::LoadFromFile(const std::string& path, bool flipVertically) {
    // 如果已经加载过，先删除旧的纹�?
    if (m_textureID != 0) {
        glDeleteTextures(1, &m_textureID);
        m_textureID = 0;
    }

    // 构建完整路径
    std::string fullPath = "assets/textures/" + path;
    
    // 垂直翻转图像（OpenGL的UV原点在左下角，而图像通常原点在左上角�?
    // 注意：需要完整的stb_image.h才能使用此函�?
    extern "C" void stbi_set_flip_vertically_on_load(int flag);
    stbi_set_flip_vertically_on_load(flipVertically ? 1 : 0);

    // 加载图像
    unsigned char* data = stbi_load(fullPath.c_str(), &m_width, &m_height, &m_channels, 0);
    
    if (!data) {
        std::cerr << "Failed to load texture: " << fullPath << std::endl;
        std::cerr << "Reason: " << stbi_failure_reason() << std::endl;
        return false;
    }

    // 创建OpenGL纹理
    CreateTexture(data, m_width, m_height, m_channels);

    // 释放stb_image数据
    stbi_image_free(data);

    m_path = path;
    std::cout << "Texture loaded successfully: " << fullPath 
              << " (" << m_width << "x" << m_height << ", " << m_channels << " channels)" << std::endl;

    return true;
}

void Texture::CreateTexture(unsigned char* data, int width, int height, int channels) {
    // 生成纹理对象
    glGenTextures(1, &m_textureID);
    glBindTexture(GL_TEXTURE_2D, m_textureID);

    // 根据通道数确定格�?
    GLenum format = GL_RGB;
    if (channels == 1) {
        format = GL_RED;
    } else if (channels == 3) {
        format = GL_RGB;
    } else if (channels == 4) {
        format = GL_RGBA;
    }

    // 上传纹理数据
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);

    // 生成Mipmap（用于纹理缩小时的平滑过渡）
    glGenerateMipmap(GL_TEXTURE_2D);

    // 设置默认纹理参数
    // 缩小过滤：使用Mipmap线性过�?
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    // 放大过滤：线性过�?
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // 环绕模式：重�?
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // 解绑
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::Bind(unsigned int unit) const {
    if (m_textureID == 0) {
        return;
    }

    // 激活纹理单�?
    glActiveTexture(GL_TEXTURE0 + unit);
    // 绑定纹理
    glBindTexture(GL_TEXTURE_2D, m_textureID);
}

void Texture::Unbind() const {
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::SetWrapMode(GLenum wrapS, GLenum wrapT) {
    if (m_textureID == 0) {
        return;
    }

    glBindTexture(GL_TEXTURE_2D, m_textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapT);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::SetFilterMode(GLenum minFilter, GLenum magFilter) {
    if (m_textureID == 0) {
        return;
    }

    glBindTexture(GL_TEXTURE_2D, m_textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
    glBindTexture(GL_TEXTURE_2D, 0);
}

} // namespace SoulsEngine
