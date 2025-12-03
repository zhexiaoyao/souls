#include "Shader.h"
#include <fstream>
#include <sstream>
#include <iostream>

namespace SoulsEngine {

Shader::Shader() : m_programID(0) {
}

Shader::~Shader() {
    if (m_programID != 0) {
        glDeleteProgram(m_programID);
        m_programID = 0;
    }
}

bool Shader::LoadFromFiles(const std::string& vertexPath, const std::string& fragmentPath) {
    std::string vertexCode = ReadFile(vertexPath);
    std::string fragmentCode = ReadFile(fragmentPath);
    
    if (vertexCode.empty() || fragmentCode.empty()) {
        std::cerr << "Failed to read shader files" << std::endl;
        return false;
    }
    
    return LoadFromSource(vertexCode, fragmentCode);
}

bool Shader::LoadFromSource(const std::string& vertexSource, const std::string& fragmentSource) {
    // 编译顶点着色器
    GLuint vertexShader = CompileShader(GL_VERTEX_SHADER, vertexSource);
    if (vertexShader == 0) {
        return false;
    }
    
    // 编译片段着色器
    GLuint fragmentShader = CompileShader(GL_FRAGMENT_SHADER, fragmentSource);
    if (fragmentShader == 0) {
        glDeleteShader(vertexShader);
        return false;
    }
    
    // 链接程序
    bool success = LinkProgram(vertexShader, fragmentShader);
    
    // 删除着色器（已经链接到程序中，不再需要）
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    
    return success;
}

void Shader::Use() const {
    if (m_programID != 0) {
        glUseProgram(m_programID);
    }
}

GLuint Shader::CompileShader(GLenum type, const std::string& source) {
    GLuint shader = glCreateShader(type);
    if (shader == 0) {
        std::cerr << "Failed to create shader" << std::endl;
        return 0;
    }
    
    const char* sourceCStr = source.c_str();
    glShaderSource(shader, 1, &sourceCStr, nullptr);
    glCompileShader(shader);
    
    // 检查编译错误
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLchar infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        const char* shaderType = (type == GL_VERTEX_SHADER) ? "VERTEX" : "FRAGMENT";
        std::cerr << "ERROR::SHADER::" << shaderType << "::COMPILATION_FAILED\n" << infoLog << std::endl;
        glDeleteShader(shader);
        return 0;
    }
    
    return shader;
}

bool Shader::LinkProgram(GLuint vertexShader, GLuint fragmentShader) {
    m_programID = glCreateProgram();
    if (m_programID == 0) {
        std::cerr << "Failed to create shader program" << std::endl;
        return false;
    }
    
    glAttachShader(m_programID, vertexShader);
    glAttachShader(m_programID, fragmentShader);
    glLinkProgram(m_programID);
    
    // 检查链接错误
    GLint success;
    glGetProgramiv(m_programID, GL_LINK_STATUS, &success);
    if (!success) {
        GLchar infoLog[512];
        glGetProgramInfoLog(m_programID, 512, nullptr, infoLog);
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
        glDeleteProgram(m_programID);
        m_programID = 0;
        return false;
    }
    
    return true;
}

std::string Shader::ReadFile(const std::string& filepath) {
    std::ifstream file;
    file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    
    try {
        file.open(filepath);
        std::stringstream stream;
        stream << file.rdbuf();
        file.close();
        return stream.str();
    }
    catch (std::ifstream::failure& e) {
        std::cerr << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << filepath << std::endl;
        std::cerr << "Exception: " << e.what() << std::endl;
        return "";
    }
}

GLint Shader::GetUniformLocation(const std::string& name) const {
    // 检查缓存
    auto it = m_uniformLocationCache.find(name);
    if (it != m_uniformLocationCache.end()) {
        return it->second;
    }
    
    // 获取位置并缓存
    GLint location = glGetUniformLocation(m_programID, name.c_str());
    if (location == -1) {
        std::cerr << "Warning: Uniform '" << name << "' doesn't exist!" << std::endl;
    }
    m_uniformLocationCache[name] = location;
    return location;
}

void Shader::SetBool(const std::string& name, bool value) const {
    glUniform1i(GetUniformLocation(name), static_cast<int>(value));
}

void Shader::SetInt(const std::string& name, int value) const {
    glUniform1i(GetUniformLocation(name), value);
}

void Shader::SetFloat(const std::string& name, float value) const {
    glUniform1f(GetUniformLocation(name), value);
}

void Shader::SetVec3(const std::string& name, float x, float y, float z) const {
    glUniform3f(GetUniformLocation(name), x, y, z);
}

void Shader::SetVec4(const std::string& name, float x, float y, float z, float w) const {
    glUniform4f(GetUniformLocation(name), x, y, z, w);
}

void Shader::SetMat4(const std::string& name, const float* value) const {
    glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, value);
}

} // namespace SoulsEngine

