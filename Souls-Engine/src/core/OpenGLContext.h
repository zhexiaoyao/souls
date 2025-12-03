#pragma once

#include <glad/glad.h>
#define GLFW_INCLUDE_NONE  // 防止 GLFW 包含 OpenGL 头文件
#include <GLFW/glfw3.h>
#include <iostream>

namespace SoulsEngine {

class OpenGLContext {
public:
    // 初始化GLAD
    static bool Initialize(GLFWwindow* window);

    // 配置OpenGL状态
    static void ConfigureState();

    // 检查OpenGL错误
    static void CheckError(const char* file, int line);

    // 获取OpenGL版本信息
    static void PrintVersionInfo();
};

} // namespace SoulsEngine

// 宏定义用于检查OpenGL错误（必须在命名空间外部）
#define GL_CHECK_ERROR() SoulsEngine::OpenGLContext::CheckError(__FILE__, __LINE__)

