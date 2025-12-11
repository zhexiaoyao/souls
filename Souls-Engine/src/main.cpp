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
#include "core/Light.h"
#include "core/LightManager.h"
#include "geometry/Cube.h"
#include "geometry/Sphere.h"
#include "geometry/Cylinder.h"
#include "geometry/Cone.h"
#include "geometry/Prism.h"
#include "geometry/Frustum.h"
#include "geometry/Mesh.h"
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

int main() {
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

    // ?????????
    SoulsEngine::SelectionSystem selectionSystem;
    std::cout << "Selection System created" << std::endl;

    // ???????????
    SoulsEngine::LightManager lightManager;
    std::cout << "Light Manager created" << std::endl;

    // ???ImGui???
    SoulsEngine::ImGuiSystem imguiSystem;
    if (!imguiSystem.Initialize(window.GetGLFWWindow())) {
        std::cerr << "Failed to initialize ImGui" << std::endl;
        window.Shutdown();
        std::cin.get();
        return -1;
    }
    std::cout << "ImGui System initialized" << std::endl;

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
                }
            }
        }
        
        // ??????????????ightManager????????- ?????????????????????
        glm::vec3 lightPos;
        glm::vec3 lightColor(1.0f, 1.0f, 1.0f);
        float lightIntensity = 0.0f;  // ????????
        
        auto firstLight = lightManager.GetFirstLight();
        if (firstLight) {
            lightPos = firstLight->GetPosition();
            lightColor = firstLight->GetColor();
            lightIntensity = firstLight->GetIntensity();
        } else {
            // ???????????????????????0?????????
            lightPos = glm::vec3(0.0f, 0.0f, 0.0f);  // ?????????????????0
            lightIntensity = 0.0f;
        }
        
        glm::vec3 viewPos = camera.GetPosition();
        shader.SetVec3("lightPos", lightPos.x, lightPos.y, lightPos.z);
        shader.SetVec3("lightColor", lightColor.r * lightIntensity, lightColor.g * lightIntensity, lightColor.b * lightIntensity);
        shader.SetVec3("viewPos", viewPos.x, viewPos.y, viewPos.z);

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
        auto lights = lightManager.GetLights();
        for (auto light : lights) {
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
        imguiSystem.RenderSidebar(&objectManager, &selectionSystem, &camera, &lightManager, aspectRatio);
        
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

