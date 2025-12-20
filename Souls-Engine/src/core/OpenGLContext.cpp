#include "OpenGLContext.h"

// glad 当前头文件中未定义混合相关常量和函数，这里做最小声明，避免包含系统 gl.h 带来的大量重复声明 warning
#ifndef GL_BLEND
#define GL_BLEND 0x0BE2
#endif
#ifndef GL_SRC_ALPHA
#define GL_SRC_ALPHA 0x0302
#endif
#ifndef GL_ONE_MINUS_SRC_ALPHA
#define GL_ONE_MINUS_SRC_ALPHA 0x0303
#endif

extern "C" void APIENTRY glBlendFunc(GLenum sfactor, GLenum dfactor);

namespace SoulsEngine {

bool OpenGLContext::Initialize(GLFWwindow* window) {
    // GLAD鍔犺浇鍑芥暟
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return false;
    }

    PrintVersionInfo();
    ConfigureState();

    return true;
}

void OpenGLContext::ConfigureState() {
    // 鍚敤娣卞害娴嬭瘯
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // 启用 Alpha 混合，用于玻璃等透明材质
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // 璁剧疆娓呴櫎棰滆壊锛堟繁鐏拌壊鑳屾櫙锛?
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

