#pragma once

#include <glad/glad.h>
#include <string>

namespace SoulsEngine {

// 纹理�? - 封装OpenGL纹理对象
class Texture {
public:
    Texture();
    ~Texture();

    // 禁止拷贝
    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;

    // 从文件加载纹�?
    // path: 纹理文件路径（相对于assets/textures/�?
    // flipVertically: 是否垂直翻转（默认true，因为OpenGL的UV原点在左下角�?
    bool LoadFromFile(const std::string& path, bool flipVertically = true);

    // 绑定纹理到指定的纹理单元
    // unit: 纹理单元索引�?0, 1, 2...），默认0
    void Bind(unsigned int unit = 0) const;

    // 解绑纹理
    void Unbind() const;

    // 设置纹理参数
    void SetWrapMode(GLenum wrapS, GLenum wrapT);
    void SetFilterMode(GLenum minFilter, GLenum magFilter);

    // 获取纹理ID
    GLuint GetID() const { return m_textureID; }

    // 获取纹理尺寸
    int GetWidth() const { return m_width; }
    int GetHeight() const { return m_height; }
    int GetChannels() const { return m_channels; }

    // 检查纹理是否已加载
    bool IsLoaded() const { return m_textureID != 0; }

    // 获取文件路径
    std::string GetPath() const { return m_path; }

private:
    GLuint m_textureID;      // OpenGL纹理ID
    int m_width;             // 纹理宽度
    int m_height;            // 纹理高度
    int m_channels;          // 颜色通道数（3=RGB, 4=RGBA�?
    std::string m_path;      // 纹理文件路径

    // 创建OpenGL纹理对象
    void CreateTexture(unsigned char* data, int width, int height, int channels);
};

} // namespace SoulsEngine
