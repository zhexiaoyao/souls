#include "OpenGLContext.h"
#include <glad/glad.h>

namespace SoulsEngine {

bool OpenGLContext::Initialize(GLFWwindow* window) {
    // GLAD加载函数
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return false;
    }

    PrintVersionInfo();
    ConfigureState();

    return true;
}

void OpenGLContext::ConfigureState() {
    // 启用深度测试
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // 设置清除颜色（深灰色背景）
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
}

void OpenGLContext::CheckError(const char* file, int line) {
    GLenum errorCode = glGetError();
    if (errorCode != 0) {  // GL_NO_ERROR = 0
        const char* errorString = nullptr;
        switch (errorCode) {
            case 0x0500: // GL_INVALID_ENUM
                errorString = "GL_INVALID_ENUM";
                break;
            case 0x0501: // GL_INVALID_VALUE
                errorString = "GL_INVALID_VALUE";
                break;
            case 0x0502: // GL_INVALID_OPERATION
                errorString = "GL_INVALID_OPERATION";
                break;
            case 0x0505: // GL_OUT_OF_MEMORY
                errorString = "GL_OUT_OF_MEMORY";
                break;
            case 0x0506: // GL_INVALID_FRAMEBUFFER_OPERATION
                errorString = "GL_INVALID_FRAMEBUFFER_OPERATION";
                break;
            default:
                errorString = "UNKNOWN_ERROR";
                break;
        }
        std::cerr << "OpenGL Error: " << errorString 
                  << " (0x" << std::hex << errorCode << std::dec << ") at " << file << ":" << line << std::endl;
    }
}

void OpenGLContext::PrintVersionInfo() {
    const GLubyte* vendor = glGetString(0x1F00);  // GL_VENDOR
    const GLubyte* renderer = glGetString(0x1F01);  // GL_RENDERER
    const GLubyte* version = glGetString(0x1F02);  // GL_VERSION
    const GLubyte* glslVersion = glGetString(0x8B8C);  // GL_SHADING_LANGUAGE_VERSION
    
    std::cout << "OpenGL Info:" << std::endl;
    if (vendor) std::cout << "  Vendor: " << (const char*)vendor << std::endl;
    if (renderer) std::cout << "  Renderer: " << (const char*)renderer << std::endl;
    if (version) std::cout << "  Version: " << (const char*)version << std::endl;
    if (glslVersion) std::cout << "  GLSL Version: " << (const char*)glslVersion << std::endl;
}

} // namespace SoulsEngine

