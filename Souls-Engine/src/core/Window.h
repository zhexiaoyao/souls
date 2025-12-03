#pragma once

#include <string>

// 前向声明，避免包含 GLFW 头文件（可能间接包含 Windows gl.h）
struct GLFWwindow;
typedef void (*GLFWframebuffersizefun)(GLFWwindow*, int, int);

namespace SoulsEngine {

// GLFW 窗口封装类
class Window {
public:
    Window(int width, int height, const std::string& title);
    ~Window();

    // 禁止拷贝
    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    // 初始化GLFW和创建窗口
    bool Initialize();
    
    // 清理资源
    void Shutdown();

    // 检查窗口是否应该关闭
    bool ShouldClose() const;

    // 交换缓冲区并处理事件
    void SwapBuffers();
    void PollEvents();

    // 获取窗口尺寸
    int GetWidth() const { return m_width; }
    int GetHeight() const { return m_height; }

    // 获取GLFW窗口指针
    GLFWwindow* GetGLFWWindow() const { return m_window; }

    // 设置窗口大小回调
    void SetFramebufferSizeCallback(GLFWframebuffersizefun callback);

private:
    int m_width;
    int m_height;
    std::string m_title;
    GLFWwindow* m_window;

    // 初始化GLFW库
    static bool InitializeGLFW();
    static void ShutdownGLFW();
    static int s_glfwRefCount;
};

} // namespace SoulsEngine

