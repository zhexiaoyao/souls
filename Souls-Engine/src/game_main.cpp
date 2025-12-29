// 游戏主程序 - 3D收集游戏
#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include "core/Window.h"
#include "core/OpenGLContext.h"
#include "core/Shader.h"
#include "core/Camera.h"
#include "core/ObjectManager.h"
#include "core/GameManager.h"
#include "core/Light.h"
#include "core/LightManager.h"
#include "core/ImGuiSystem.h"
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <windows.h>

// 检查文件是否存在
bool FileExists(const std::string& path) {
    std::ifstream file(path);
    return file.good();
}

int main() {
    // 设置控制台输出为UTF-8（Windows）
    #ifdef _WIN32
    SetConsoleOutputCP(65001);
    #endif

    std::cout << "=== 3D收集游戏 ===" << std::endl;
    
    // 获取当前工作目录（Windows）
    char buffer[256];
    GetCurrentDirectoryA(256, buffer);
    std::cout << "当前工作目录: " << buffer << std::endl;

    // 查找Shader文件路径
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
        
        std::cout << "检查Shader文件: " << basePath << std::endl;
        
        if (FileExists(vertexPath) && FileExists(fragmentPath)) {
            std::cout << "找到Shader文件: " << basePath << std::endl;
            shaderFilesFound = true;
            break;
        }
    }
    
    if (!shaderFilesFound) {
        std::cerr << "错误: 找不到Shader文件！" << std::endl;
        for (const auto& path : shaderPaths) {
            std::cerr << "  - " << path << std::endl;
        }
        std::cout << "按Enter键退出..." << std::endl;
        std::cin.get();
        return -1;
    }

    // 创建窗口
    SoulsEngine::Window window(1280, 720, "3D收集游戏 - Souls Engine");
    std::cout << "窗口创建完成" << std::endl;

    // 初始化窗口
    if (!window.Initialize()) {
        std::cerr << "错误: 窗口初始化失败" << std::endl;
        std::cout << "按Enter键退出..." << std::endl;
        std::cin.get();
        return -1;
    }
    std::cout << "窗口初始化成功" << std::endl;

    // 初始化OpenGL上下文
    if (!SoulsEngine::OpenGLContext::Initialize(window.GetGLFWWindow())) {
        std::cerr << "错误: OpenGL上下文初始化失败" << std::endl;
        window.Shutdown();
        std::cout << "按Enter键退出..." << std::endl;
        std::cin.get();
        return -1;
    }
    std::cout << "OpenGL上下文初始化成功" << std::endl;

    // 设置窗口大小回调
    window.SetFramebufferSizeCallback([](GLFWwindow* win, int width, int height) {
        glViewport(0, 0, width, height);
    });

    // 设置视口
    glViewport(0, 0, window.GetWidth(), window.GetHeight());

    // 加载Shader
    SoulsEngine::Shader shader;
    std::cout << "从以下路径加载Shader: " << vertexPath << " 和 " << fragmentPath << std::endl;
    if (!shader.LoadFromFiles(vertexPath, fragmentPath)) {
        std::cerr << "错误: Shader编译/链接失败！" << std::endl;
        window.Shutdown();
        std::cout << "按Enter键退出..." << std::endl;
        std::cin.get();
        return -1;
    }
    std::cout << "Shader加载并编译成功！" << std::endl;

    // 创建相机（第三人称视角，跟随玩家）
    SoulsEngine::Camera camera(glm::vec3(0.0f, 5.0f, 10.0f));
    float aspectRatio = static_cast<float>(window.GetWidth()) / static_cast<float>(window.GetHeight());
    
    // 设置鼠标滚轮回调
    glfwSetScrollCallback(window.GetGLFWWindow(), [](GLFWwindow* w, double xoffset, double yoffset) {
        void* userPtr = glfwGetWindowUserPointer(w);
        if (userPtr) {
            SoulsEngine::Camera* cam = static_cast<SoulsEngine::Camera*>(userPtr);
            cam->ProcessMouseScroll(static_cast<float>(yoffset));
        }
    });
    
    glfwSetWindowUserPointer(window.GetGLFWWindow(), &camera);

    // 创建对象管理器
    SoulsEngine::ObjectManager objectManager;
    std::cout << "对象管理器创建完成" << std::endl;

    // 创建光源管理器
    SoulsEngine::LightManager lightManager;
    
    // 添加一个环境光源（使用LightManager的AddLight方法）
    auto light = lightManager.AddLight(
        glm::vec3(0.0f, 10.0f, 0.0f),  // 位置
        glm::vec3(1.0f, 1.0f, 1.0f),  // 颜色
        1.5f,                          // 强度
        360.0f                         // 角度
    );
    if (light) {
        light->SetName("MainLight");
    }
    std::cout << "光源创建完成" << std::endl;

    // 创建游戏管理器
    SoulsEngine::GameManager gameManager(&objectManager, &camera);
    gameManager.Initialize();
    std::cout << "游戏管理器初始化完成" << std::endl;

    // 初始化ImGui（用于游戏UI）
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window.GetGLFWWindow(), true);
    ImGui_ImplOpenGL3_Init("#version 330");
    std::cout << "ImGui初始化完成" << std::endl;

    std::cout << "\n=== 游戏开始 ===" << std::endl;
    std::cout << "控制说明:" << std::endl;
    std::cout << "  - WASD: 移动玩家" << std::endl;
    std::cout << "  - 鼠标移动: 旋转相机视角" << std::endl;
    std::cout << "  - 鼠标滚轮: 缩放视角" << std::endl;
    std::cout << "  - ESC: 退出游戏" << std::endl;
    std::cout << "  - R: 重新开始游戏" << std::endl;
    std::cout << "\n目标: 收集黄色立方体，避开红色圆柱体！" << std::endl;
    
    // 游戏循环
    float lastTime = 0.0f;
    bool mouseButtonPressed = false;
    double lastMouseX = 0.0, lastMouseY = 0.0;
    
    while (!window.ShouldClose()) {
        // 计算deltaTime
        float currentTime = static_cast<float>(glfwGetTime());
        float deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        // 处理事件
        window.PollEvents();

        // ESC退出
        if (glfwGetKey(window.GetGLFWWindow(), GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window.GetGLFWWindow(), true);
        }

        // R键重新开始
        static bool rKeyPressed = false;
        bool rKeyDown = glfwGetKey(window.GetGLFWWindow(), GLFW_KEY_R) == GLFW_PRESS;
        if (rKeyDown && !rKeyPressed) {
            gameManager.ResetGame();
            std::cout << "游戏重新开始！" << std::endl;
        }
        rKeyPressed = rKeyDown;

        // 处理玩家输入
        gameManager.ProcessPlayerInput(deltaTime, window.GetGLFWWindow());

        // 鼠标控制相机
        double mouseX, mouseY;
        glfwGetCursorPos(window.GetGLFWWindow(), &mouseX, &mouseY);
        
        bool leftMouseDown = glfwGetMouseButton(window.GetGLFWWindow(), GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
        
        if (leftMouseDown && mouseButtonPressed) {
            float xoffset = static_cast<float>(mouseX - lastMouseX);
            float yoffset = static_cast<float>(lastMouseY - mouseY);
            camera.ProcessMouseMovement(xoffset, yoffset, true);
        }
        
        lastMouseX = mouseX;
        lastMouseY = mouseY;
        mouseButtonPressed = leftMouseDown;

        // 更新游戏逻辑
        gameManager.Update(deltaTime);

        // 更新相机位置（跟随玩家，但保持一定距离）
        auto playerNode = objectManager.FindNode("Player");
        if (playerNode) {
            glm::vec3 playerPos = playerNode->GetPosition();
            // 相机跟随玩家，保持一定距离和高度
            glm::vec3 cameraOffset = glm::vec3(0.0f, 5.0f, 10.0f);
            camera.SetPosition(playerPos + cameraOffset);
        }

        // 清除缓冲区
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // 使用Shader
        shader.Use();

        // 计算视图和投影矩阵
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = camera.GetProjectionMatrix(aspectRatio);
        
        // 传递矩阵到Shader
        shader.SetMat4("view", glm::value_ptr(view));
        shader.SetMat4("projection", glm::value_ptr(projection));
        
        // 更新场景
        objectManager.Update();
        
        // 设置光照参数 - 支持多光源（符合标准Phong光照模型）
        glm::vec3 viewPos = camera.GetPosition();
        auto lights = lightManager.GetLights();
        int numLights = std::min(static_cast<int>(lights.size()), 8);  // 最多8个光源
        
        shader.SetInt("numLights", numLights);
        // 全局环境光（独立于光源，不受距离衰减影响）
        shader.SetVec3("globalAmbient", 0.2f, 0.2f, 0.2f);  // I_a: 全局环境光强度
        shader.SetVec3("viewPos", viewPos.x, viewPos.y, viewPos.z);
        
        // 设置每个光源的属性
        for (int i = 0; i < numLights; i++) {
            auto light = lights[i];
            glm::vec3 lightPos = light->GetPosition();
            glm::vec3 lightColor = light->GetColor();
            float lightIntensity = light->GetIntensity();
            
            std::string lightPrefix = "lights[" + std::to_string(i) + "].";
            shader.SetVec3(lightPrefix + "position", lightPos.x, lightPos.y, lightPos.z);
            shader.SetVec3(lightPrefix + "color", lightColor.r, lightColor.g, lightColor.b);
            shader.SetFloat(lightPrefix + "intensity", lightIntensity);
            
            // 设置距离衰减参数
            shader.SetFloat(lightPrefix + "constant", 1.0f);
            shader.SetFloat(lightPrefix + "linear", 0.09f);
            shader.SetFloat(lightPrefix + "quadratic", 0.032f);
        }

        // 渲染场景
        objectManager.Render(&shader);

        // 渲染游戏UI（使用ImGui）
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // 游戏UI窗口
        {
            ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Always);
            ImGui::SetNextWindowSize(ImVec2(300, 200), ImGuiCond_Always);
            ImGui::Begin("游戏信息", nullptr, 
                         ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | 
                         ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);
            
            // 显示得分
            ImGui::Text("得分: %d", gameManager.GetScore());
            ImGui::Separator();
            
            // 显示剩余时间
            float timeRemaining = gameManager.GetTimeRemaining();
            ImGui::Text("剩余时间: %.1f 秒", timeRemaining);
            
            // 时间进度条
            float timeProgress = timeRemaining / 60.0f;
            ImGui::ProgressBar(timeProgress, ImVec2(280, 20), "");
            ImGui::Separator();
            
            // 游戏状态
            if (gameManager.IsGameOver()) {
                ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "游戏结束！");
                ImGui::Text("最终得分: %d", gameManager.GetScore());
                ImGui::Text("按 R 键重新开始");
            } else if (gameManager.IsGameWon()) {
                ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "恭喜获胜！");
            } else {
                ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "游戏中...");
            }
            
            ImGui::Separator();
            ImGui::Text("控制:");
            ImGui::BulletText("WASD - 移动");
            ImGui::BulletText("鼠标 - 旋转视角");
            ImGui::BulletText("R - 重新开始");
            ImGui::BulletText("ESC - 退出");
            
            ImGui::End();
        }

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        
        // 交换缓冲区
        window.SwapBuffers();

        // 检查OpenGL错误
        GL_CHECK_ERROR();
    }

    // 清理资源
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    objectManager.Clear();

    std::cout << "游戏结束，正在关闭..." << std::endl;
    
    return 0;
}

