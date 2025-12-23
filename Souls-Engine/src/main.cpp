// ???????? GLFW ?????? GLAD?????? Windows gl.h ???
#include <glad/glad.h>
#define GLFW_INCLUDE_NONE  // ??? GLFW ??? OpenGL ?????
#include "core/Window.h"
#include "core/OpenGLContext.h"
#include "core/Shader.h"
#include "core/Camera.h"
#include "core/ObjectManager.h"
#include "core/SceneNode.h"
#include "core/Node.h"
#include "core/SelectionSystem.h"
// ???????????
// #include "core/Material.h"
#include "core/ImGuiSystem.h"
#include "core/FragmentEffectManager.h"
#include "core/Light.h"
#include "core/LightManager.h"
#include "core/ShadowMap.h"
#include "geometry/Cube.h"
#include "geometry/Sphere.h"
#include "geometry/Cylinder.h"
#include "geometry/Cone.h"
#include "geometry/Prism.h"
#include "geometry/Frustum.h"
#include "geometry/GridPlane.h"
#include "geometry/Mesh.h"
#include "io/AssimpLoader.h"
#include "io/OBJLoader.h"
#include <GLFW/glfw3.h>
#include <imgui.h> // ?? ImGui ??????????
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <memory>
#include <windows.h>

// ??????????????????
bool FileExists(const std::string& path) {
    std::ifstream file(path);
    return file.good();
}

int main(int argc, char** argv) {
    // ????????????UTF-8??indows??
    #ifdef _WIN32
    SetConsoleOutputCP(65001);
    #endif

    std::cout << "=== Souls Engine Starting ===" << std::endl;
    
    // ??????????????indows??
    char buffer[256];
    GetCurrentDirectoryA(256, buffer);
    std::cout << "Current working directory: " << buffer << std::endl;

    // ?????hader???????????????????????
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

    // ??????
    SoulsEngine::Window window(1280, 720, "Souls Engine - Day 3-4: Basic Rendering System");
    std::cout << "Window created" << std::endl;

    // ????????
    if (!window.Initialize()) {
        std::cerr << "ERROR: Failed to initialize window" << std::endl;
        std::cout << "Press Enter to exit..." << std::endl;
        std::cin.get();
        return -1;
    }
    std::cout << "Window initialized successfully" << std::endl;

    // ?????LAD??penGL?????
    if (!SoulsEngine::OpenGLContext::Initialize(window.GetGLFWWindow())) {
        std::cerr << "ERROR: Failed to initialize OpenGL context" << std::endl;
        window.Shutdown();
        std::cout << "Press Enter to exit..." << std::endl;
        std::cin.get();
        return -1;
    }
    std::cout << "OpenGL context initialized successfully" << std::endl;

    // ????????????
    window.SetFramebufferSizeCallback([](GLFWwindow* win, int width, int height) {
        glViewport(0, 0, width, height);
    });

    // ?????????
    glViewport(0, 0, window.GetWidth(), window.GetHeight());

    // ???Shader
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

    // ??????
    SoulsEngine::Camera camera(glm::vec3(0.0f, 2.0f, 8.0f));
    float aspectRatio = static_cast<float>(window.GetWidth()) / static_cast<float>(window.GetHeight());
    
    // ????????????????????????
    glfwSetScrollCallback(window.GetGLFWWindow(), [](GLFWwindow* w, double xoffset, double yoffset) {
        // ??????????????????
        void* userPtr = glfwGetWindowUserPointer(w);
        if (userPtr) {
            SoulsEngine::Camera* cam = static_cast<SoulsEngine::Camera*>(userPtr);
            cam->ProcessMouseScroll(static_cast<float>(yoffset));
        }
    });
    
    // ????????????????????????????????????
    glfwSetWindowUserPointer(window.GetGLFWWindow(), &camera);

    // ?????????????????????
    SoulsEngine::ObjectManager objectManager;
    std::cout << "Object Manager created" << std::endl;

    // 默认环境：黑白棋盘格地平面
    {
        auto gridMesh = std::make_shared<SoulsEngine::GridPlane>(40.0f, 40);
        auto gridNode = objectManager.CreateNode("Ground", gridMesh);
        gridNode->SetPosition(glm::vec3(0.0f, 0.0f, 0.0f));
    }

    // 如果传入了命令行参数，尝试加载指定模型文件（优先使用 AssimpLoader，如果可用）
    if (argc > 1) {
        std::string modelPath = argv[1];
        std::unique_ptr<SoulsEngine::Mesh> importedMesh;
        // 尝试使用 Assimp（如果启用）
        importedMesh = SoulsEngine::LoadModelWithAssimp(modelPath);
        if (!importedMesh) {
            // 回退到 OBJ 加载
            importedMesh = SoulsEngine::LoadOBJFromFile(modelPath);
        }
        if (importedMesh) {
            // 将 unique_ptr 转为 shared_ptr（安全地释放 unique_ptr 的所有权）
            std::shared_ptr<SoulsEngine::Mesh> meshShared;
            meshShared.reset(importedMesh.release());
            auto node = objectManager.CreateNode("ImportedModel", meshShared);
            node->SetPosition(glm::vec3(0.0f, 0.5f, 0.0f));
            std::cout << "Imported model: " << modelPath << std::endl;
        } else {
            std::cout << "Failed to import model: " << modelPath << std::endl;
        }
    } else {
        // 如果没有提供参数，尝试加载内置示例模型（assets/models/examples/triangle.obj）
        const std::string examplePath = "assets/models/examples/triangle.obj";
        if (FileExists(examplePath)) {
            auto mesh = SoulsEngine::LoadOBJFromFile(examplePath);
            if (mesh) {
                std::shared_ptr<SoulsEngine::Mesh> meshShared;
                meshShared.reset(mesh.release());
                auto node = objectManager.CreateNode("ExampleTriangle", meshShared);
                node->SetPosition(glm::vec3(0.0f, 0.5f, 0.0f));
                std::cout << "Loaded example model: " << examplePath << std::endl;
            }
        }
    }

    // ?????????
    SoulsEngine::SelectionSystem selectionSystem;
    std::cout << "Selection System created" << std::endl;

    // ???????????
    SoulsEngine::LightManager lightManager;
    std::cout << "Light Manager created" << std::endl;

    // 默认环境光源：在场景中心上方放置一个点光源，便于展示地面与物体
    {
        glm::vec3 defaultLightPos(0.0f, 5.0f, 0.0f);
        glm::vec3 defaultLightColor(1.0f, 1.0f, 1.0f);
        float defaultLightIntensity = 3.0f;
        float defaultLightAngle = 360.0f;
        auto defaultLight = lightManager.AddLight(defaultLightPos, defaultLightColor, defaultLightIntensity, defaultLightAngle);

        // 创建太阳球体（橙红色），便于在场景中看到光源位置
        glm::vec3 sunColor(1.0f, 0.5f, 0.0f);  // 橙红色 (RGB: 255, 128, 0)
        auto sunMesh = std::make_shared<SoulsEngine::Sphere>(0.6f, 32, 16, sunColor);  // 较大的球体（半径0.6）作为太阳
        auto sunName = "Sun_" + defaultLight->GetName();
        auto sunNode = objectManager.CreateNode(sunName, sunMesh);
        sunNode->SetPosition(defaultLightPos);
        
        // 保留原有的黄色指示器球体（较小，用于精确定位）
        auto lightIndicatorMesh = std::make_shared<SoulsEngine::Sphere>(0.2f, 16, 8, glm::vec3(1.0f, 1.0f, 0.0f));
        auto indicatorName = "LightIndicator_" + defaultLight->GetName();
        auto indicatorNode = objectManager.CreateNode(indicatorName, lightIndicatorMesh);
        indicatorNode->SetPosition(defaultLightPos);
    }

    // 创建阴影映射
    SoulsEngine::ShadowMap shadowMap(2048, 2048);
    if (!shadowMap.Initialize()) {
        std::cerr << "ERROR: Failed to initialize shadow map!" << std::endl;
    } else {
        std::cout << "Shadow Map initialized successfully!" << std::endl;
    }

    // 创建深度着色器（用于生成阴影贴图）
    SoulsEngine::Shader depthShader;
    std::vector<std::string> depthShaderPaths = {
        "assets/shaders/",
        "../assets/shaders/",
        "../../assets/shaders/",
        "../../../assets/shaders/",
        "build/bin/Debug/assets/shaders/",
        "build/bin/Release/assets/shaders/"
    };
    std::string depthVertPath, depthFragPath;
    bool depthShaderFound = false;
    for (const auto& basePath : depthShaderPaths) {
        depthVertPath = basePath + "depth.vert";
        depthFragPath = basePath + "depth.frag";
        if (FileExists(depthVertPath) && FileExists(depthFragPath)) {
            depthShaderFound = true;
            break;
        }
    }
    if (depthShaderFound && depthShader.LoadFromFiles(depthVertPath, depthFragPath)) {
        std::cout << "Depth shader loaded successfully!" << std::endl;
    } else {
        std::cerr << "Warning: Depth shader not found, shadows will be disabled" << std::endl;
    }

    // ???ImGui???
    SoulsEngine::ImGuiSystem imguiSystem;
    if (!imguiSystem.Initialize(window.GetGLFWWindow())) {
        std::cerr << "Failed to initialize ImGui" << std::endl;
        window.Shutdown();
        std::cin.get();
        return -1;
    }
    std::cout << "ImGui System initialized" << std::endl;

    // 创建碎裂效果管理器
    SoulsEngine::FragmentEffectManager fragmentEffectManager(&objectManager);
    std::cout << "Fragment Effect Manager created" << std::endl;

    // ??????????????????
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
    std::cout << "Week 2 Day 6-7: Material System completed!" << std::endl;
    std::cout << "  - Material data structure implemented" << std::endl;
    std::cout << "  - Basic shader parameters implemented" << std::endl;
    std::cout << "  - Material property modification implemented" << std::endl;
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
    
    // ??????
    float lastTime = 0.0f;
    
    // ????????
    while (!window.ShouldClose()) {
        // ???deltaTime
        float currentTime = static_cast<float>(glfwGetTime());
        float deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        // 更新碎裂效果
        fragmentEffectManager.Update(deltaTime);

        // ?????????
        window.PollEvents();

        // ESC??????
        if (glfwGetKey(window.GetGLFWWindow(), GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window.GetGLFWWindow(), true);
        }

        // ????????ASD + QE??
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

        // ??????????????????1-6??
        static bool keyPressed[6] = { false, false, false, false, false, false };
        for (int i = 0; i < 6; ++i) {
            int key = GLFW_KEY_1 + i;
            bool isPressed = glfwGetKey(window.GetGLFWWindow(), key) == GLFW_PRESS;
            if (isPressed && !keyPressed[i]) {
                // ?????????????????
                std::shared_ptr<SoulsEngine::SceneNode> newNode;
                std::string name;
                glm::vec3 color;
                
                switch (i) {
                    case 0: { // Cube
                        auto mesh = std::make_shared<SoulsEngine::Cube>(1.0f, glm::vec3(1.0f, 0.0f, 0.0f));
                        name = "Cube_" + std::to_string(geometryCounter++);
                        newNode = objectManager.CreateNode(name, mesh);
                        // ??????????????????????????
                        // auto material = std::make_shared<SoulsEngine::Material>();
                        // material->SetColor(1.0f, 0.0f, 0.0f);
                        // material->SetSpecular(0.5f, 0.5f, 0.5f);
                        // material->SetShininess(32.0f);
                        // material->SetName("RedMaterial");
                        // newNode->SetMaterial(material);
                        break;
                    }
                    case 1: { // Sphere
                        auto mesh = std::make_shared<SoulsEngine::Sphere>(0.8f, 36, 18, glm::vec3(0.0f, 1.0f, 0.0f));
                        name = "Sphere_" + std::to_string(geometryCounter++);
                        newNode = objectManager.CreateNode(name, mesh);
                        // ??????????????????????????
                        // auto material = std::make_shared<SoulsEngine::Material>(SoulsEngine::Material::CreateEmerald());
                        // newNode->SetMaterial(material);
                        break;
                    }
                    case 2: { // Cylinder
                        auto mesh = std::make_shared<SoulsEngine::Cylinder>(0.7f, 1.5f, 36, glm::vec3(0.0f, 0.0f, 1.0f));
                        name = "Cylinder_" + std::to_string(geometryCounter++);
                        newNode = objectManager.CreateNode(name, mesh);
                        // ??????????????????????????
                        // auto material = std::make_shared<SoulsEngine::Material>();
                        // material->SetColor(0.0f, 0.0f, 1.0f);
                        // material->SetSpecular(0.7f, 0.7f, 0.7f);
                        // material->SetShininess(64.0f);
                        // material->SetName("BlueMaterial");
                        // newNode->SetMaterial(material);
                        break;
                    }
                    case 3: { // Cone
                        auto mesh = std::make_shared<SoulsEngine::Cone>(0.7f, 1.5f, 36, glm::vec3(1.0f, 1.0f, 0.0f));
                        name = "Cone_" + std::to_string(geometryCounter++);
                        newNode = objectManager.CreateNode(name, mesh);
                        // ??????????????????????????
                        // auto material = std::make_shared<SoulsEngine::Material>(SoulsEngine::Material::CreateGold());
                        // newNode->SetMaterial(material);
                        break;
                    }
                    case 4: { // Prism
                        auto mesh = std::make_shared<SoulsEngine::Prism>(6, 0.7f, 1.5f, glm::vec3(1.0f, 0.0f, 1.0f));
                        name = "Prism_" + std::to_string(geometryCounter++);
                        newNode = objectManager.CreateNode(name, mesh);
                        // ??????????????????????????
                        // auto material = std::make_shared<SoulsEngine::Material>();
                        // material->SetColor(1.0f, 0.0f, 1.0f);
                        // material->SetSpecular(0.8f, 0.8f, 0.8f);
                        // material->SetShininess(48.0f);
                        // material->SetName("MagentaMaterial");
                        // newNode->SetMaterial(material);
                        break;
                    }
                    case 5: { // Frustum
                        auto mesh = std::make_shared<SoulsEngine::Frustum>(6, 0.4f, 0.7f, 1.5f, glm::vec3(0.0f, 1.0f, 1.0f));
                        name = "Frustum_" + std::to_string(geometryCounter++);
                        newNode = objectManager.CreateNode(name, mesh);
                        // ??????????????????????????
                        // auto material = std::make_shared<SoulsEngine::Material>(SoulsEngine::Material::CreateJade());
                        // newNode->SetMaterial(material);
                        break;
                    }
                }
                
                if (newNode) {
                    // ??????????????
                    glm::vec3 cameraPos = camera.GetPosition();
                    glm::vec3 cameraFront = camera.GetFront();
                    newNode->SetPosition(cameraPos + cameraFront * 5.0f);
                    selectionSystem.SelectNode(newNode);
                    std::cout << "Created: " << name << std::endl;
                }
            }
            keyPressed[i] = isPressed;
        }

        // R????????????
        static bool rKeyPressed = false;
        bool rKeyDown = glfwGetKey(window.GetGLFWWindow(), GLFW_KEY_R) == GLFW_PRESS;
        if (rKeyDown && !rKeyPressed) {
            selectionSystem.SetRotationMode(!selectionSystem.IsRotationMode());
            std::cout << "Rotation mode: " << (selectionSystem.IsRotationMode() ? "ON" : "OFF") << std::endl;
        }
        rKeyPressed = rKeyDown;

        // ?????????
        double mouseX, mouseY;
        glfwGetCursorPos(window.GetGLFWWindow(), &mouseX, &mouseY);
        glm::vec2 mousePos(static_cast<float>(mouseX), static_cast<float>(mouseY));
        
        // ????????????[0, 1]
        glm::vec2 normalizedMousePos(
            mousePos.x / static_cast<float>(window.GetWidth()),
            mousePos.y / static_cast<float>(window.GetHeight())
        );

        // ?????????????????
        static bool leftMousePressed = false;
        static double lastClickTime = 0.0;
        static glm::vec2 lastClickPos(0.0f, 0.0f);
        const double DOUBLE_CLICK_TIME = 0.3; // ??????????????
        const float DOUBLE_CLICK_DISTANCE = 0.01f; // ?????????
        
        bool leftMouseDown = glfwGetMouseButton(window.GetGLFWWindow(), GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
        double clickTime = static_cast<double>(currentTime); // ?????????currentTime???
        bool imguiWantsMouse = ImGui::GetIO().WantCaptureMouse; // ImGui?????????
        
        // ????????????????????????????????????????????
        static float cameraRotateLastMouseX = -1.0f;
        static float cameraRotateLastMouseY = -1.0f;
        
        if (!imguiWantsMouse && leftMouseDown && !leftMousePressed) {
            // ????????
            auto allNodes = objectManager.GetAllNodes();
            auto pickedNode = selectionSystem.PickNode(normalizedMousePos, camera, allNodes, 
                                                       window.GetWidth(), window.GetHeight());
            
            // ???????
            bool isDoubleClick = false;
            if (pickedNode && pickedNode == selectionSystem.GetSelectedNode()) {
                double timeSinceLastClick = clickTime - lastClickTime;
                float distanceSinceLastClick = glm::length(normalizedMousePos - lastClickPos);
                if (timeSinceLastClick < DOUBLE_CLICK_TIME && distanceSinceLastClick < DOUBLE_CLICK_DISTANCE) {
                    isDoubleClick = true;
                }
            }
            
            if (isDoubleClick) {
                // ??????????????
                selectionSystem.StartScale(normalizedMousePos, camera, aspectRatio);
                std::cout << "Scale mode activated for: " << pickedNode->GetName() << std::endl;
            } else {
                // ???????????
                if (pickedNode) {
                    selectionSystem.SelectNode(pickedNode);
                    selectionSystem.StartDrag(normalizedMousePos, camera);
                    std::cout << "Selected: " << pickedNode->GetName() << std::endl;
                } else {
                    // ??????????????????????????????????????????????
                    auto previouslySelected = selectionSystem.GetSelectedNode();
                    if (previouslySelected) {
                        std::string nodeName = previouslySelected->GetName();
                        if (nodeName.find("LightIndicator_") == 0) {
                            std::string lightName = nodeName.substr(15); // "LightIndicator_" ?????15
                            auto light = lightManager.FindLightByName(lightName);
                            if (light) {
                                glm::vec3 worldPos = previouslySelected->LocalToWorld(glm::vec3(0.0f, 0.0f, 0.0f));
                                light->SetPosition(worldPos);
                                std::cout << "Light position synced before deselect: (" << worldPos.x << ", " << worldPos.y << ", " << worldPos.z << ")" << std::endl;
                            }
                        } else if (nodeName.find("Sun_") == 0) {
                            std::string lightName = nodeName.substr(4); // "Sun_" ?????4
                            auto light = lightManager.FindLightByName(lightName);
                            if (light) {
                                glm::vec3 worldPos = previouslySelected->LocalToWorld(glm::vec3(0.0f, 0.0f, 0.0f));
                                light->SetPosition(worldPos);
                                std::cout << "Light position synced (from Sun) before deselect: (" << worldPos.x << ", " << worldPos.y << ", " << worldPos.z << ")" << std::endl;
                            }
                        }
                    }
                    selectionSystem.Deselect();
                    std::cout << "Deselected" << std::endl;
                    // ???????????????????????????????????????????????
                    cameraRotateLastMouseX = static_cast<float>(mouseX);
                    cameraRotateLastMouseY = static_cast<float>(mouseY);
                }
            }
            
            lastClickTime = clickTime;
            lastClickPos = normalizedMousePos;
        } else if (!imguiWantsMouse && !leftMouseDown && leftMousePressed) {
            // ????????
            if (selectionSystem.IsDragging()) {
                // ??????????????????????????????????????
                auto selectedNode = selectionSystem.GetSelectedNode();
                if (selectedNode) {
                    std::string nodeName = selectedNode->GetName();
                    if (nodeName.find("LightIndicator_") == 0) {
                        // ?????????????? "LightIndicator_" ?????
                        std::string lightName = nodeName.substr(15); // "LightIndicator_" ?????15
                        auto light = lightManager.FindLightByName(lightName);
                        if (light) {
                            // ??????????????????????????
                            glm::vec3 worldPos = selectedNode->LocalToWorld(glm::vec3(0.0f, 0.0f, 0.0f));
                            light->SetPosition(worldPos);
                        }
                    } else if (nodeName.find("Sun_") == 0) {
                        // ?????????????? "Sun_" ?????
                        std::string lightName = nodeName.substr(4); // "Sun_" ?????4
                        auto light = lightManager.FindLightByName(lightName);
                        if (light) {
                            // ??????????????????????????
                            glm::vec3 worldPos = selectedNode->LocalToWorld(glm::vec3(0.0f, 0.0f, 0.0f));
                            light->SetPosition(worldPos);
                        }
                    }
                }
                selectionSystem.EndDrag();
            }
            if (selectionSystem.IsScaling()) {
                selectionSystem.EndScale();
            }
        } else if (leftMouseDown) {
            // ????????
            if (imguiWantsMouse) {
                // ImGui????????????????? UI ????????
            } else if (selectionSystem.IsScaling()) {
                // ??????
                selectionSystem.UpdateScale(normalizedMousePos, camera, aspectRatio);
            } else if (selectionSystem.IsDragging()) {
                // ??????
                selectionSystem.UpdateDrag(normalizedMousePos, camera, deltaTime, aspectRatio);
                
                // ??????????????????????????????
                auto selectedNode = selectionSystem.GetSelectedNode();
                if (selectedNode) {
                    std::string nodeName = selectedNode->GetName();
                    if (nodeName.find("LightIndicator_") == 0) {
                        // ?????????????? "LightIndicator_" ?????
                        std::string lightName = nodeName.substr(15); // "LightIndicator_" ?????15
                        auto light = lightManager.FindLightByName(lightName);
                        if (light) {
                            // ??????????????????????????
                            glm::vec3 worldPos = selectedNode->LocalToWorld(glm::vec3(0.0f, 0.0f, 0.0f));
                            light->SetPosition(worldPos);
                            // ??????
                            std::cout << "Light position updated to: (" << worldPos.x << ", " << worldPos.y << ", " << worldPos.z << ")" << std::endl;
                        } else {
                            std::cout << "Warning: Light not found with name: " << lightName << std::endl;
                        }
                    } else if (nodeName.find("Sun_") == 0) {
                        // ?????????????? "Sun_" ?????
                        std::string lightName = nodeName.substr(4); // "Sun_" ?????4
                        auto light = lightManager.FindLightByName(lightName);
                        if (light) {
                            // ??????????????????????????
                            glm::vec3 worldPos = selectedNode->LocalToWorld(glm::vec3(0.0f, 0.0f, 0.0f));
                            light->SetPosition(worldPos);
                            // ??????
                            std::cout << "Light position updated (from Sun) to: (" << worldPos.x << ", " << worldPos.y << ", " << worldPos.z << ")" << std::endl;
                        } else {
                            std::cout << "Warning: Light not found with name: " << lightName << std::endl;
                        }
                    }
                }
            } else if (!selectionSystem.HasSelection()) {
                // ?????????????????????????????
                // ???????????????????????????????????????
                if (cameraRotateLastMouseX < 0.0f || cameraRotateLastMouseY < 0.0f) {
                    cameraRotateLastMouseX = static_cast<float>(mouseX);
                    cameraRotateLastMouseY = static_cast<float>(mouseY);
                } else {
                    // ???????????
                    // ????????????????????????????????????
                    float xoffset = static_cast<float>(mouseX) - cameraRotateLastMouseX;
                    // ??????Y?????????????????mouseY???????????
                    float yoffset = cameraRotateLastMouseY - static_cast<float>(mouseY);
                    
                    // ????????????????????
                    if (xoffset != 0.0f || yoffset != 0.0f) {
                        // ?????????????????????
                        camera.ProcessMouseMovement(xoffset, yoffset, true);
                    }
                    
                    // ?????????
                    cameraRotateLastMouseX = static_cast<float>(mouseX);
                    cameraRotateLastMouseY = static_cast<float>(mouseY);
                }
            }
        } else {
            // ??????????????????????????????
            cameraRotateLastMouseX = -1.0f;
            cameraRotateLastMouseY = -1.0f;

            // ImGui ????????????????????????
            if (imguiWantsMouse && leftMousePressed) {
                if (selectionSystem.IsDragging()) {
                    selectionSystem.EndDrag();
                }
                if (selectionSystem.IsScaling()) {
                    selectionSystem.EndScale();
                }
            }
        }
        leftMousePressed = leftMouseDown;

        // ImGui???
        imguiSystem.BeginFrame();

        // ========== 第一遍渲染：生成阴影贴图（从光源视角） ==========
        auto lights = lightManager.GetLights();
        glm::mat4 lightSpaceMatrix;
        bool shadowsEnabled = false;
        if (depthShaderFound && !lights.empty()) {
            auto firstLight = lights[0];
            glm::vec3 lightPos = firstLight->GetPosition();
            // 计算光源方向（假设光源向下照射）
            glm::vec3 lightDir = glm::normalize(glm::vec3(0.0f) - lightPos);
            
            // 计算光源空间矩阵
            lightSpaceMatrix = shadowMap.GetLightSpaceMatrix(lightPos, lightDir, 0.1f, 100.0f, 25.0f);
            
            // 开始渲染到阴影贴图
            shadowMap.BeginRender();
            
            // 使用深度着色器
            depthShader.Use();
            depthShader.SetMat4("lightSpaceMatrix", glm::value_ptr(lightSpaceMatrix));
            
            // 渲染场景到阴影贴图
            objectManager.Render(&depthShader);
            
            // 结束阴影贴图渲染
            shadowMap.EndRender();
            
            // 恢复视口
            glViewport(0, 0, window.GetWidth(), window.GetHeight());
            
            shadowsEnabled = true;
        }

        // ========== 第二遍渲染：正常渲染（使用阴影贴图） ==========
        // ???????????????
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // ???Shader
        shader.Use();

        // ??????????????
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = camera.GetProjectionMatrix(aspectRatio);
        
        // ???????????????Shader
        shader.SetMat4("view", glm::value_ptr(view));
        shader.SetMat4("projection", glm::value_ptr(projection));
        shader.SetMat4("lightSpaceMatrix", glm::value_ptr(lightSpaceMatrix));
        
        // 设置阴影相关uniform
        shader.SetBool("useShadows", shadowsEnabled);
        if (shadowsEnabled) {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, shadowMap.GetDepthTexture());
            shader.SetInt("shadowMap", 0);
        }
        
        // ??????????????????- ????????????????????????????????????
        objectManager.Update();
        
        // ??????????????????????????????????????????
        if (selectionSystem.IsDragging()) {
            auto selectedNode = selectionSystem.GetSelectedNode();
            if (selectedNode) {
                std::string nodeName = selectedNode->GetName();
                if (nodeName.find("LightIndicator_") == 0) {
                    std::string lightName = nodeName.substr(15); // "LightIndicator_" ?????15
                    auto light = lightManager.FindLightByName(lightName);
                    if (light) {
                        glm::vec3 worldPos = selectedNode->LocalToWorld(glm::vec3(0.0f, 0.0f, 0.0f));
                        light->SetPosition(worldPos);
                        // ???????????????????????
                    } else {
                        std::cout << "Warning: Light not found with name: " << lightName << " (before render)" << std::endl;
                    }
                } else if (nodeName.find("Sun_") == 0) {
                    std::string lightName = nodeName.substr(4); // "Sun_" ?????4
                    auto light = lightManager.FindLightByName(lightName);
                    if (light) {
                        glm::vec3 worldPos = selectedNode->LocalToWorld(glm::vec3(0.0f, 0.0f, 0.0f));
                        light->SetPosition(worldPos);
                        // ???????????????????????
                    } else {
                        std::cout << "Warning: Light not found with name: " << lightName << " (before render, from Sun)" << std::endl;
                    }
                }
            }
        }
        
        // 设置光照参数 - 支持多光源（符合标准Phong光照模型）
        glm::vec3 viewPos = camera.GetPosition();
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
            
            // 设置距离衰减参数（根据光源强度调整）
            // 衰减公式: 1.0 / (constant + linear * d + quadratic * d^2)
            // 使用常见的衰减值组合
            shader.SetFloat(lightPrefix + "constant", 1.0f);
            shader.SetFloat(lightPrefix + "linear", 0.09f);
            shader.SetFloat(lightPrefix + "quadratic", 0.032f);
        }

        // ????????????????????
        objectManager.Render(&shader);

        // ????????????????????
        if (auto selectedNode = selectionSystem.GetSelectedNode()) {
            // ?????????
            shader.SetBool("useOverrideColor", true);
            shader.SetVec3("overrideColor", 0.0f, 0.0f, 0.0f);  // ???
            
            // ??????????????
            glm::mat4 identity = glm::mat4(1.0f);
            selectedNode->RenderWireframe(identity, &shader);
            
            // ?????????
            shader.SetBool("useOverrideColor", false);
        }
        
        // ?????????????????????
        for (auto light : lights) {
            glm::vec3 lightPos = light->GetPosition();
            
            // 同步太阳球体位置
            std::string sunName = "Sun_" + light->GetName();
            auto sunNode = objectManager.FindNode(sunName);
            if (sunNode) {
                auto selectedNode = selectionSystem.GetSelectedNode();
                if (!selectedNode || selectedNode->GetName() != sunName) {
                    // 如果太阳没有被选中，则同步位置
                    sunNode->SetPosition(lightPos);
                }
            }
            
            // 同步指示器球体位置
            std::string indicatorName = "LightIndicator_" + light->GetName();
            auto indicatorNode = objectManager.FindNode(indicatorName);
            if (indicatorNode) {
                // ????????????????????????????????????????
                // ????????????????????????????????????????????
                // ????????????????????
                auto selectedNode = selectionSystem.GetSelectedNode();
                bool isSelected = (selectedNode == indicatorNode);
                
                if (!isSelected) {
                    // ??????????????????????????
                    // ?????????????????????????????????
                    glm::vec3 lightWorldPos = light->GetPosition();
                    // ??????????????????
                    glm::vec3 indicatorWorldPos = indicatorNode->LocalToWorld(glm::vec3(0.0f, 0.0f, 0.0f));
                    // ????????????????????????????????????????????
                    float distance = glm::length(lightWorldPos - indicatorWorldPos);
                    if (distance > 0.01f) {  // ?????????0.01?????????
                        SoulsEngine::Node* parent = indicatorNode->GetParent();
                        if (parent) {
                            // ??????????????????????????
                            glm::mat4 parentWorldInv = glm::inverse(parent->GetWorldTransform());
                            glm::vec3 localPos = glm::vec3(parentWorldInv * glm::vec4(lightWorldPos, 1.0f));
                            indicatorNode->SetPosition(localPos);
                        } else {
                            // ?????????????????????
                            indicatorNode->SetPosition(lightWorldPos);
                        }
                    }
                }
                // ?????????????????????????????????????????
                
                // ????????????????????
                shader.SetBool("useOverrideColor", true);
                shader.SetVec3("overrideColor", 1.0f, 1.0f, 0.0f);  // ???
                
                glm::mat4 identity = glm::mat4(1.0f);
                indicatorNode->RenderWireframe(identity, &shader);
                
                shader.SetBool("useOverrideColor", false);
            }
        }

        // ???ImGui???
        imguiSystem.RenderSidebar(&objectManager, &selectionSystem, &camera, &lightManager, aspectRatio, nullptr, &fragmentEffectManager);
        
        // ImGui?????????
        imguiSystem.EndFrame();

        // ???????????
        window.SwapBuffers();

        // ????penGL???
        GL_CHECK_ERROR();
    }

    // ????????????????????????
    imguiSystem.Shutdown();
    objectManager.Clear();

    std::cout << "Shutting down..." << std::endl;
    
    return 0;
}

