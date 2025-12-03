// 必须在包含 GLFW 之前包含 GLAD，以防止 Windows gl.h 冲突
#include <glad/glad.h>
#define GLFW_INCLUDE_NONE  // 防止 GLFW 包含 OpenGL 头文件
#include "core/Window.h"
#include "core/OpenGLContext.h"
#include "core/Shader.h"
#include "core/Camera.h"
#include "core/ObjectManager.h"
#include "core/SceneNode.h"
#include "core/SelectionSystem.h"
#include "geometry/Cube.h"
#include "geometry/Sphere.h"
#include "geometry/Cylinder.h"
#include "geometry/Cone.h"
#include "geometry/Prism.h"
#include "geometry/Frustum.h"
#include "geometry/Mesh.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <memory>
#include <windows.h>

// 简单的文件存在检查函数
bool FileExists(const std::string& path) {
    std::ifstream file(path);
    return file.good();
}

int main() {
    // 设置控制台输出为UTF-8（Windows）
    #ifdef _WIN32
    SetConsoleOutputCP(65001);
    #endif

    std::cout << "=== Souls Engine Starting ===" << std::endl;
    
    // 获取当前工作目录（Windows）
    char buffer[256];
    GetCurrentDirectoryA(256, buffer);
    std::cout << "Current working directory: " << buffer << std::endl;

    // 先检查Shader文件是否存在（在创建窗口之前）
    std::vector<std::string> shaderPaths = {
        "assets/shaders/",
        "../assets/shaders/",
        "../../assets/shaders/",
        "build/bin/Debug/assets/shaders/",
        "build/bin/Release/assets/shaders/"
    };
    
    std::string vertexPath, fragmentPath;
    bool shaderFilesFound = false;
    
    for (const auto& basePath : shaderPaths) {
        vertexPath = basePath + "basic.vert";
        fragmentPath = basePath + "basic.frag";
        
        std::cout << "Checking shader files at: " << basePath << std::endl;
        
        if (FileExists(vertexPath) && FileExists(fragmentPath)) {
            std::cout << "Found shader files at: " << basePath << std::endl;
            shaderFilesFound = true;
            break;
        }
    }
    
    if (!shaderFilesFound) {
        std::cerr << "ERROR: Shader files not found in any of these paths:" << std::endl;
        for (const auto& path : shaderPaths) {
            std::cerr << "  - " << path << std::endl;
        }
        std::cerr << "Please ensure shader files exist in one of these locations." << std::endl;
        std::cout << "Press Enter to exit..." << std::endl;
        std::cin.get();
        return -1;
    }

    // 创建窗口
    SoulsEngine::Window window(1280, 720, "Souls Engine - Day 3-4: Basic Rendering System");
    std::cout << "Window created" << std::endl;

    // 初始化窗口
    if (!window.Initialize()) {
        std::cerr << "ERROR: Failed to initialize window" << std::endl;
        std::cout << "Press Enter to exit..." << std::endl;
        std::cin.get();
        return -1;
    }
    std::cout << "Window initialized successfully" << std::endl;

    // 初始化GLAD和OpenGL上下文
    if (!SoulsEngine::OpenGLContext::Initialize(window.GetGLFWWindow())) {
        std::cerr << "ERROR: Failed to initialize OpenGL context" << std::endl;
        window.Shutdown();
        std::cout << "Press Enter to exit..." << std::endl;
        std::cin.get();
        return -1;
    }
    std::cout << "OpenGL context initialized successfully" << std::endl;

    // 设置视口大小回调
    window.SetFramebufferSizeCallback([](GLFWwindow* win, int width, int height) {
        glViewport(0, 0, width, height);
    });

    // 设置初始视口
    glViewport(0, 0, window.GetWidth(), window.GetHeight());

    // 加载Shader
    SoulsEngine::Shader shader;
    std::cout << "Loading shaders from: " << vertexPath << " and " << fragmentPath << std::endl;
    if (!shader.LoadFromFiles(vertexPath, fragmentPath)) {
        std::cerr << "ERROR: Failed to compile/link shaders!" << std::endl;
        window.Shutdown();
        std::cout << "Press Enter to exit..." << std::endl;
        std::cin.get();
        return -1;
    }
    std::cout << "Shaders loaded and compiled successfully!" << std::endl;

    // 创建相机
    SoulsEngine::Camera camera(glm::vec3(0.0f, 2.0f, 8.0f));
    float aspectRatio = static_cast<float>(window.GetWidth()) / static_cast<float>(window.GetHeight());
    
    // 设置鼠标滚轮回调（用于缩放视角）
    glfwSetScrollCallback(window.GetGLFWWindow(), [](GLFWwindow* w, double xoffset, double yoffset) {
        // 使用用户指针存储相机指针
        void* userPtr = glfwGetWindowUserPointer(w);
        if (userPtr) {
            SoulsEngine::Camera* cam = static_cast<SoulsEngine::Camera*>(userPtr);
            cam->ProcessMouseScroll(static_cast<float>(yoffset));
        }
    });
    
    // 将相机指针存储到窗口用户数据中，以便回调函数访问
    glfwSetWindowUserPointer(window.GetGLFWWindow(), &camera);

    // 创建对象管理器（场景图系统）
    SoulsEngine::ObjectManager objectManager;
    std::cout << "Object Manager created" << std::endl;

    // 创建选择系统
    SoulsEngine::SelectionSystem selectionSystem;
    std::cout << "Selection System created" << std::endl;

    // 几何体计数器（用于命名）
    int geometryCounter = 0;

    std::cout << "Souls Engine initialized successfully!" << std::endl;
    std::cout << "Window size: " << window.GetWidth() << "x" << window.GetHeight() << std::endl;
    std::cout << "Week 2 Day 1-3: Scene Graph System completed!" << std::endl;
    std::cout << "  - Scene Node structure implemented" << std::endl;
    std::cout << "  - Parent-child node relationships implemented" << std::endl;
    std::cout << "  - Scene traversal implemented" << std::endl;
    std::cout << "  - Object Manager implemented" << std::endl;
    std::cout << "Week 2 Day 4-5: Transform System completed!" << std::endl;
    std::cout << "  - Rotation transform implemented" << std::endl;
    std::cout << "  - Translation transform implemented" << std::endl;
    std::cout << "  - Scale transform implemented" << std::endl;
    std::cout << "  - Transform matrix calculation implemented" << std::endl;
    std::cout << "  - Local/World coordinate transform implemented" << std::endl;
    std::cout << "Geometry objects in scene:" << std::endl;
    std::cout << "  - Cube (Red)" << std::endl;
    std::cout << "  - Sphere (Green)" << std::endl;
    std::cout << "  - Cylinder (Blue)" << std::endl;
    std::cout << "  - Cone (Yellow)" << std::endl;
    std::cout << "  - Prism (Magenta)" << std::endl;
    std::cout << "  - Frustum (Cyan)" << std::endl;
    std::cout << "=== Interactive Editor Mode ===" << std::endl;
    std::cout << "Controls:" << std::endl;
    std::cout << "  - ESC: Exit" << std::endl;
    std::cout << "  - WASD: Move camera (forward/backward/left/right)" << std::endl;
    std::cout << "  - Q/E: Move camera (down/up)" << std::endl;
    std::cout << "  - 1: Create Cube (Red)" << std::endl;
    std::cout << "  - 2: Create Sphere (Green)" << std::endl;
    std::cout << "  - 3: Create Cylinder (Blue)" << std::endl;
    std::cout << "  - 4: Create Cone (Yellow)" << std::endl;
    std::cout << "  - 5: Create Prism (Magenta)" << std::endl;
    std::cout << "  - 6: Create Frustum (Cyan)" << std::endl;
    std::cout << "  - Left Click: Select object" << std::endl;
    std::cout << "  - Double Click: Scale object (intersection point follows mouse)" << std::endl;
    std::cout << "  - Drag Mouse: Move selected object" << std::endl;
    std::cout << "  - Left Click + Drag (no selection): Rotate camera horizontally" << std::endl;
    std::cout << "  - Mouse Wheel: Zoom in/out" << std::endl;
    std::cout << "  - R: Toggle rotation mode" << std::endl;
    
    // 计算时间
    float lastTime = 0.0f;
    
    // 主渲染循环
    while (!window.ShouldClose()) {
        // 计算deltaTime
        float currentTime = static_cast<float>(glfwGetTime());
        float deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        // 处理输入事件
        window.PollEvents();

        // ESC键退出
        if (glfwGetKey(window.GetGLFWWindow(), GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window.GetGLFWWindow(), true);
        }

        // 相机控制（WASD + QE）
        if (glfwGetKey(window.GetGLFWWindow(), GLFW_KEY_W) == GLFW_PRESS)
            camera.ProcessKeyboard(SoulsEngine::Camera::FORWARD, deltaTime);
        if (glfwGetKey(window.GetGLFWWindow(), GLFW_KEY_S) == GLFW_PRESS)
            camera.ProcessKeyboard(SoulsEngine::Camera::BACKWARD, deltaTime);
        if (glfwGetKey(window.GetGLFWWindow(), GLFW_KEY_A) == GLFW_PRESS)
            camera.ProcessKeyboard(SoulsEngine::Camera::LEFT, deltaTime);
        if (glfwGetKey(window.GetGLFWWindow(), GLFW_KEY_D) == GLFW_PRESS)
            camera.ProcessKeyboard(SoulsEngine::Camera::RIGHT, deltaTime);
        if (glfwGetKey(window.GetGLFWWindow(), GLFW_KEY_Q) == GLFW_PRESS)
            camera.ProcessKeyboard(SoulsEngine::Camera::DOWN, deltaTime);
        if (glfwGetKey(window.GetGLFWWindow(), GLFW_KEY_E) == GLFW_PRESS)
            camera.ProcessKeyboard(SoulsEngine::Camera::UP, deltaTime);

        // 键盘快捷键：创建几何体（1-6）
        static bool keyPressed[6] = { false, false, false, false, false, false };
        for (int i = 0; i < 6; ++i) {
            int key = GLFW_KEY_1 + i;
            bool isPressed = glfwGetKey(window.GetGLFWWindow(), key) == GLFW_PRESS;
            if (isPressed && !keyPressed[i]) {
                // 按键刚按下，创建几何体
                std::shared_ptr<SoulsEngine::SceneNode> newNode;
                std::string name;
                glm::vec3 color;
                
                switch (i) {
                    case 0: { // Cube
                        auto mesh = std::make_shared<SoulsEngine::Cube>(1.0f, glm::vec3(1.0f, 0.0f, 0.0f));
                        name = "Cube_" + std::to_string(geometryCounter++);
                        newNode = objectManager.CreateNode(name, mesh);
                        break;
                    }
                    case 1: { // Sphere
                        auto mesh = std::make_shared<SoulsEngine::Sphere>(0.8f, 36, 18, glm::vec3(0.0f, 1.0f, 0.0f));
                        name = "Sphere_" + std::to_string(geometryCounter++);
                        newNode = objectManager.CreateNode(name, mesh);
                        break;
                    }
                    case 2: { // Cylinder
                        auto mesh = std::make_shared<SoulsEngine::Cylinder>(0.7f, 1.5f, 36, glm::vec3(0.0f, 0.0f, 1.0f));
                        name = "Cylinder_" + std::to_string(geometryCounter++);
                        newNode = objectManager.CreateNode(name, mesh);
                        break;
                    }
                    case 3: { // Cone
                        auto mesh = std::make_shared<SoulsEngine::Cone>(0.7f, 1.5f, 36, glm::vec3(1.0f, 1.0f, 0.0f));
                        name = "Cone_" + std::to_string(geometryCounter++);
                        newNode = objectManager.CreateNode(name, mesh);
                        break;
                    }
                    case 4: { // Prism
                        auto mesh = std::make_shared<SoulsEngine::Prism>(6, 0.7f, 1.5f, glm::vec3(1.0f, 0.0f, 1.0f));
                        name = "Prism_" + std::to_string(geometryCounter++);
                        newNode = objectManager.CreateNode(name, mesh);
                        break;
                    }
                    case 5: { // Frustum
                        auto mesh = std::make_shared<SoulsEngine::Frustum>(6, 0.4f, 0.7f, 1.5f, glm::vec3(0.0f, 1.0f, 1.0f));
                        name = "Frustum_" + std::to_string(geometryCounter++);
                        newNode = objectManager.CreateNode(name, mesh);
                        break;
                    }
                }
                
                if (newNode) {
                    // 在相机前方创建对象
                    glm::vec3 cameraPos = camera.GetPosition();
                    glm::vec3 cameraFront = camera.GetFront();
                    newNode->SetPosition(cameraPos + cameraFront * 5.0f);
                    selectionSystem.SelectNode(newNode);
                    std::cout << "Created: " << name << std::endl;
                }
            }
            keyPressed[i] = isPressed;
        }

        // R键：切换旋转模式
        static bool rKeyPressed = false;
        bool rKeyDown = glfwGetKey(window.GetGLFWWindow(), GLFW_KEY_R) == GLFW_PRESS;
        if (rKeyDown && !rKeyPressed) {
            selectionSystem.SetRotationMode(!selectionSystem.IsRotationMode());
            std::cout << "Rotation mode: " << (selectionSystem.IsRotationMode() ? "ON" : "OFF") << std::endl;
        }
        rKeyPressed = rKeyDown;

        // 鼠标输入处理
        double mouseX, mouseY;
        glfwGetCursorPos(window.GetGLFWWindow(), &mouseX, &mouseY);
        glm::vec2 mousePos(static_cast<float>(mouseX), static_cast<float>(mouseY));
        
        // 归一化鼠标坐标到[0, 1]
        glm::vec2 normalizedMousePos(
            mousePos.x / static_cast<float>(window.GetWidth()),
            mousePos.y / static_cast<float>(window.GetHeight())
        );

        // 鼠标左键点击：选择对象
        static bool leftMousePressed = false;
        static double lastClickTime = 0.0;
        static glm::vec2 lastClickPos(0.0f, 0.0f);
        const double DOUBLE_CLICK_TIME = 0.3; // 双击时间间隔（秒）
        const float DOUBLE_CLICK_DISTANCE = 0.01f; // 双击位置容差
        
        bool leftMouseDown = glfwGetMouseButton(window.GetGLFWWindow(), GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
        double clickTime = static_cast<double>(currentTime); // 重用已存在的currentTime变量
        
        // 用于相机旋转的鼠标位置记录（静态变量，在函数作用域内保持）
        static float cameraRotateLastMouseX = -1.0f;
        static float cameraRotateLastMouseY = -1.0f;
        
        if (leftMouseDown && !leftMousePressed) {
            // 鼠标刚按下
            auto allNodes = objectManager.GetAllNodes();
            auto pickedNode = selectionSystem.PickNode(normalizedMousePos, camera, allNodes, 
                                                       window.GetWidth(), window.GetHeight());
            
            // 检测双击
            bool isDoubleClick = false;
            if (pickedNode && pickedNode == selectionSystem.GetSelectedNode()) {
                double timeSinceLastClick = clickTime - lastClickTime;
                float distanceSinceLastClick = glm::length(normalizedMousePos - lastClickPos);
                if (timeSinceLastClick < DOUBLE_CLICK_TIME && distanceSinceLastClick < DOUBLE_CLICK_DISTANCE) {
                    isDoubleClick = true;
                }
            }
            
            if (isDoubleClick) {
                // 双击：进入缩放模式
                selectionSystem.StartScale(normalizedMousePos, camera, aspectRatio);
                std::cout << "Scale mode activated for: " << pickedNode->GetName() << std::endl;
            } else {
                // 单击：选择对象
                if (pickedNode) {
                    selectionSystem.SelectNode(pickedNode);
                    selectionSystem.StartDrag(normalizedMousePos, camera);
                    std::cout << "Selected: " << pickedNode->GetName() << std::endl;
                } else {
                    selectionSystem.Deselect();
                    std::cout << "Deselected" << std::endl;
                    // 没有选中对象时，记录鼠标位置用于相机旋转（点击时保持视角不变）
                    cameraRotateLastMouseX = static_cast<float>(mouseX);
                    cameraRotateLastMouseY = static_cast<float>(mouseY);
                }
            }
            
            lastClickTime = clickTime;
            lastClickPos = normalizedMousePos;
        } else if (!leftMouseDown && leftMousePressed) {
            // 鼠标刚释放
            if (selectionSystem.IsDragging()) {
                selectionSystem.EndDrag();
            }
            if (selectionSystem.IsScaling()) {
                selectionSystem.EndScale();
            }
        } else if (leftMouseDown) {
            // 鼠标按下中
            if (selectionSystem.IsScaling()) {
                // 缩放模式
                selectionSystem.UpdateScale(normalizedMousePos, camera, aspectRatio);
            } else if (selectionSystem.IsDragging()) {
                // 拖拽模式
                selectionSystem.UpdateDrag(normalizedMousePos, camera, deltaTime, aspectRatio);
            } else if (!selectionSystem.HasSelection()) {
                // 没有选中几何体时，旋转相机（三维旋转）
                // 如果还没有记录鼠标位置，先记录（点击时保持视角不变）
                if (cameraRotateLastMouseX < 0.0f || cameraRotateLastMouseY < 0.0f) {
                    cameraRotateLastMouseX = static_cast<float>(mouseX);
                    cameraRotateLastMouseY = static_cast<float>(mouseY);
                } else {
                    // 计算鼠标偏移量
                    // 取反方向：鼠标向右时视角向右，鼠标向上时视角向上
                    float xoffset = static_cast<float>(mouseX) - cameraRotateLastMouseX;
                    // 屏幕坐标Y轴向下，所以鼠标向上时mouseY减小，需要反转
                    float yoffset = cameraRotateLastMouseY - static_cast<float>(mouseY);
                    
                    // 只有当鼠标移动时才旋转相机
                    if (xoffset != 0.0f || yoffset != 0.0f) {
                        // 三维旋转：允许水平和垂直旋转
                        camera.ProcessMouseMovement(xoffset, yoffset, true);
                    }
                    
                    // 更新鼠标位置
                    cameraRotateLastMouseX = static_cast<float>(mouseX);
                    cameraRotateLastMouseY = static_cast<float>(mouseY);
                }
            }
        } else {
            // 鼠标未按下时，重置相机旋转的鼠标位置记录
            cameraRotateLastMouseX = -1.0f;
            cameraRotateLastMouseY = -1.0f;
        }
        leftMousePressed = leftMouseDown;

        // 清除颜色和深度缓冲区
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // 使用Shader
        shader.Use();

        // 获取视图和投影矩阵
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = camera.GetProjectionMatrix(aspectRatio);
        
        // 设置视图和投影矩阵到Shader
        shader.SetMat4("view", glm::value_ptr(view));
        shader.SetMat4("projection", glm::value_ptr(projection));

        // 更新场景（计算变换矩阵）
        objectManager.Update();

        // 使用场景图系统渲染所有对象
        objectManager.Render(&shader);

        // 为选中的对象渲染黑色边框线
        if (auto selectedNode = selectionSystem.GetSelectedNode()) {
            // 启用覆盖颜色
            shader.SetBool("useOverrideColor", true);
            shader.SetVec3("overrideColor", 0.0f, 0.0f, 0.0f);  // 黑色
            
            // 渲染选中对象的线框
            glm::mat4 identity = glm::mat4(1.0f);
            selectedNode->RenderWireframe(identity, &shader);
            
            // 恢复颜色设置
            shader.SetBool("useOverrideColor", false);
        }

        // 交换前后缓冲区
        window.SwapBuffers();

        // 检查OpenGL错误
        GL_CHECK_ERROR();
    }

    // 清理资源（场景图系统会自动清理）
    objectManager.Clear();

    std::cout << "Shutting down..." << std::endl;
    
    return 0;
}

