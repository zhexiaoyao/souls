#pragma once

#include <glad/glad.h>
#include <string>
#include <unordered_map>

namespace SoulsEngine {

// Shader 程序管理类
class Shader {
public:
    Shader();
    ~Shader();

    // 禁止拷贝
    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;

    // 从文件加载并编译Shader
    bool LoadFromFiles(const std::string& vertexPath, const std::string& fragmentPath);
    
    // 从源代码编译Shader
    bool LoadFromSource(const std::string& vertexSource, const std::string& fragmentSource);

    // 使用Shader程序
    void Use() const;

    // 获取Shader程序ID
    GLuint GetProgramID() const { return m_programID; }

    // 设置Uniform变量
    void SetBool(const std::string& name, bool value) const;
    void SetInt(const std::string& name, int value) const;
    void SetFloat(const std::string& name, float value) const;
    void SetVec3(const std::string& name, float x, float y, float z) const;
    void SetVec4(const std::string& name, float x, float y, float z, float w) const;
    void SetMat4(const std::string& name, const float* value) const;

private:
    GLuint m_programID;
    mutable std::unordered_map<std::string, GLint> m_uniformLocationCache;

    // 编译单个Shader
    GLuint CompileShader(GLenum type, const std::string& source);
    
    // 链接Shader程序
    bool LinkProgram(GLuint vertexShader, GLuint fragmentShader);
    
    // 从文件读取内容
    std::string ReadFile(const std::string& filepath);
    
    // 获取Uniform位置（带缓存）
    GLint GetUniformLocation(const std::string& name) const;
};

} // namespace SoulsEngine

