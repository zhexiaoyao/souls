#include "Window.h"
#define GLFW_INCLUDE_NONE  // 防止 GLFW 包含 OpenGL 头文件
#include <GLFW/glfw3.h>
#include <iostream>

namespace SoulsEngine {

int Window::s_glfwRefCount = 0;

Window::Window(int width, int height, const std::string& title)
    : m_width(width), m_height(height), m_title(title), m_window(nullptr) {
}

Window::~Window() {
    Shutdown();
}

bool Window::Initialize() {
    // 初始化GLFW
    if (!InitializeGLFW()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return false;
    }

    // 配置OpenGL上下文
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // 创建窗口
    m_window = glfwCreateWindow(m_width, m_height, m_title.c_str(), nullptr, nullptr);
    if (m_window == nullptr) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        ShutdownGLFW();
        return false;
    }

    // 设置当前上下文
    glfwMakeContextCurrent(m_window);

    // 设置视口回调
    glfwSetFramebufferSizeCallback(m_window, [](GLFWwindow* window, int width, int height) {
        // 这里会在SetFramebufferSizeCallback中设置用户回调
    });

    // 启用垂直同步
    glfwSwapInterval(1);

    return true;
}

void Window::Shutdown() {
    if (m_window != nullptr) {
        glfwDestroyWindow(m_window);
        m_window = nullptr;
    }
    ShutdownGLFW();
}

bool Window::ShouldClose() const {
    return m_window != nullptr && glfwWindowShouldClose(m_window);
}

void Window::SwapBuffers() {
    if (m_window != nullptr) {
        glfwSwapBuffers(m_window);
    }
}

void Window::PollEvents() {
    glfwPollEvents();
}

void Window::SetFramebufferSizeCallback(GLFWframebuffersizefun callback) {
    if (m_window != nullptr) {
        glfwSetFramebufferSizeCallback(m_window, callback);
    }
}

bool Window::InitializeGLFW() {
    if (s_glfwRefCount == 0) {
        if (!glfwInit()) {
            return false;
        }
    }
    s_glfwRefCount++;
    return true;
}

void Window::ShutdownGLFW() {
    if (s_glfwRefCount > 0) {
        s_glfwRefCount--;
        if (s_glfwRefCount == 0) {
            glfwTerminate();
        }
    }
}

} // namespace SoulsEngine

