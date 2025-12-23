// FPS Shooter Game Main Program
// First-person shooter game: target shooting

#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include "../src/core/Window.h"
#include "../src/core/OpenGLContext.h"
#include "../src/core/Shader.h"
#include "../src/core/Camera.h"
#include "../src/core/ObjectManager.h"
#include "../src/core/FPSGameManager.h"
#include "../src/core/WeaponModel.h"
#include "../src/core/Light.h"
#include "../src/core/LightManager.h"
#include "../src/core/Material.h"
#include "../src/core/ShadowMap.h"
#include "../src/core/SceneNode.h"
#include "../src/geometry/Mesh.h"
#include "../src/geometry/Sphere.h"
#include "../src/io/OBJExporter.h"
#include "../src/core/OpenGLContext.h"  // For GL_CHECK_ERROR macro
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
#include <vector>
#include <windows.h>

// Check if file exists
bool FileExists(const std::string& path) {
    std::ifstream file(path);
    return file.good();
}

// Draw crosshair using ImGui (optimized version, reduced calculations)
void DrawCrosshairImGui(int windowWidth, int windowHeight) {
    // Only call GetBackgroundDrawList after ImGui::NewFrame()
    ImDrawList* drawList = ImGui::GetBackgroundDrawList();
    if (!drawList) return;  // Safety check
    
    // Cache calculation results
    static float cachedCenterX = -1.0f;
    static float cachedCenterY = -1.0f;
    static int cachedWidth = -1;
    static int cachedHeight = -1;
    
    // Only recalculate center point when window size changes
    if (cachedWidth != windowWidth || cachedHeight != windowHeight) {
        cachedCenterX = windowWidth / 2.0f;
        cachedCenterY = windowHeight / 2.0f;
        cachedWidth = windowWidth;
        cachedHeight = windowHeight;
    }
    
    const float size = 15.0f;
    const float thickness = 2.0f;
    const ImU32 color = IM_COL32(255, 255, 255, 255);  // White
    
    // Draw crosshair
    // Horizontal line
    drawList->AddLine(
        ImVec2(cachedCenterX - size, cachedCenterY),
        ImVec2(cachedCenterX + size, cachedCenterY),
        color,
        thickness
    );
    // Vertical line
    drawList->AddLine(
        ImVec2(cachedCenterX, cachedCenterY - size),
        ImVec2(cachedCenterX, cachedCenterY + size),
        color,
        thickness
    );
    
    // Draw center point
    drawList->AddCircleFilled(
        ImVec2(cachedCenterX, cachedCenterY),
        2.0f,
        color
    );
}

int main() {
    // Set console output to UTF-8 (Windows)
    #ifdef _WIN32
    SetConsoleOutputCP(65001);
    #endif

    try {
        std::cout << "=== FPS Shooter Game ===" << std::endl;
    
    // Get current working directory (Windows)
    char buffer[256];
    GetCurrentDirectoryA(256, buffer);
    std::cout << "Current working directory: " << buffer << std::endl;

    // Find Shader file paths
    std::vector<std::string> shaderPaths = {
        "assets/shaders/",
        "../assets/shaders/",
        "../../assets/shaders/",
        "../../../assets/shaders/",
        "build/bin/Debug/assets/shaders/",
        "build/bin/Release/assets/shaders/"
    };
    
    std::string vertexPath, fragmentPath;
    bool shaderFilesFound = false;
    
    for (const auto& basePath : shaderPaths) {
        vertexPath = basePath + "basic.vert";
        fragmentPath = basePath + "basic.frag";
        
        std::cout << "Checking Shader files: " << basePath << std::endl;
        
        if (FileExists(vertexPath) && FileExists(fragmentPath)) {
            std::cout << "Found Shader files: " << basePath << std::endl;
            shaderFilesFound = true;
            break;
        }
    }
    
    if (!shaderFilesFound) {
        std::cerr << "Error: Shader files not found!" << std::endl;
        for (const auto& path : shaderPaths) {
            std::cerr << "  - " << path << std::endl;
        }
        std::cout << "Press Enter to exit..." << std::endl;
        std::cin.get();
        return -1;
    }

    // Create window
    SoulsEngine::Window window(1280, 720, "FPS Shooter Game");
    std::cout << "Window created" << std::endl;

    // Initialize window
    if (!window.Initialize()) {
        std::cerr << "Error: Window initialization failed" << std::endl;
        std::cout << "Press Enter to exit..." << std::endl;
        std::cin.get();
        return -1;
    }
    std::cout << "Window initialized successfully" << std::endl;

    // Initialize OpenGL context
    if (!SoulsEngine::OpenGLContext::Initialize(window.GetGLFWWindow())) {
        std::cerr << "Error: OpenGL context initialization failed" << std::endl;
        window.Shutdown();
        std::cout << "Press Enter to exit..." << std::endl;
        std::cin.get();
        return -1;
    }
    std::cout << "OpenGL context initialized successfully" << std::endl;

    // Set window size callback
    window.SetFramebufferSizeCallback([](GLFWwindow* win, int width, int height) {
        glViewport(0, 0, width, height);
    });

    // Set viewport
    glViewport(0, 0, window.GetWidth(), window.GetHeight());

    // Load Shader
    SoulsEngine::Shader shader;
    std::cout << "Loading Shader from: " << vertexPath << " and " << fragmentPath << std::endl;
    if (!shader.LoadFromFiles(vertexPath, fragmentPath)) {
        std::cerr << "Error: Shader compilation/linking failed!" << std::endl;
        window.Shutdown();
        std::cout << "Press Enter to exit..." << std::endl;
        std::cin.get();
        return -1;
    }
    std::cout << "Shader loaded and compiled successfully!" << std::endl;

    // Create camera (first-person view)
    SoulsEngine::Camera camera(glm::vec3(0.0f, 1.6f, 0.0f));
    float aspectRatio = static_cast<float>(window.GetWidth()) / static_cast<float>(window.GetHeight());
    
    // Set mouse mode to disabled (lock mouse and hide cursor, suitable for FPS games)
    glfwSetInputMode(window.GetGLFWWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    // Set mouse scroll callback
    glfwSetScrollCallback(window.GetGLFWWindow(), [](GLFWwindow* w, double xoffset, double yoffset) {
        void* userPtr = glfwGetWindowUserPointer(w);
        if (userPtr) {
            SoulsEngine::Camera* cam = static_cast<SoulsEngine::Camera*>(userPtr);
            cam->ProcessMouseScroll(static_cast<float>(yoffset));
        }
    });
    
    glfwSetWindowUserPointer(window.GetGLFWWindow(), &camera);

    // Create object manager
    SoulsEngine::ObjectManager objectManager;
    std::cout << "Object manager created" << std::endl;

    // Create light manager
    SoulsEngine::LightManager lightManager;
    
    // Add a main light (directional light from above)
    auto light = lightManager.AddLight(
        glm::vec3(0.0f, 15.0f, 0.0f),  // Position (higher up for better coverage)
        glm::vec3(1.0f, 1.0f, 1.0f),  // Color (white)
        3.0f,                          // Intensity (increased for better visibility)
        360.0f                         // Angle (360 = omnidirectional)
    );
    if (light) {
        light->SetName("MainLight");
        glm::vec3 lightPos = light->GetPosition();
        std::cout << "Light created at (" << lightPos.x << ", " << lightPos.y << ", " << lightPos.z 
                  << ") with intensity " << light->GetIntensity() << std::endl;
        
        // Create sun sphere (orange-red colored sphere) at light position
        glm::vec3 sunColor(1.0f, 0.5f, 0.0f);  // Orange-red color (RGB: 255, 128, 0)
        auto sunMesh = std::make_shared<SoulsEngine::Sphere>(0.6f, 32, 16, sunColor);  // Larger sphere (0.6 radius) for sun
        auto sunName = "Sun_" + light->GetName();
        auto sunNode = objectManager.CreateNode(sunName, sunMesh);
        sunNode->SetPosition(lightPos);
        std::cout << "Sun sphere created at light position" << std::endl;
    } else {
        std::cerr << "Warning: Failed to create light!" << std::endl;
    }

    // Create FPS game manager
    std::cout << "Creating FPS game manager..." << std::endl;
    SoulsEngine::FPSGameManager fpsGameManager(&objectManager, &camera);
    std::cout << "FPS game manager created, initializing..." << std::endl;
    fpsGameManager.Initialize();
    std::cout << "FPS game manager initialized" << std::endl;

    // Create weapon model
    SoulsEngine::WeaponModel weaponModel;
    weaponModel.CreateWeapon(&objectManager);
    
    // Save weapon model to file
    auto weaponNode = weaponModel.GetWeaponNode();
    if (weaponNode) {
        // Create assets/models directory if it doesn't exist
        std::string weaponModelPath = "assets/models/weapon.obj";
        if (SoulsEngine::OBJExporter::ExportWeaponModel(weaponNode, weaponModelPath)) {
            std::cout << "Weapon model saved to: " << weaponModelPath << std::endl;
        } else {
            std::cout << "Warning: Failed to save weapon model" << std::endl;
        }
    }
    weaponModel.CreateWeapon(&objectManager);
    std::cout << "Weapon model created" << std::endl;

    // Create shadow map
    SoulsEngine::ShadowMap shadowMap(2048, 2048);
    bool shadowMapInitialized = false;
    if (shadowMap.Initialize()) {
        shadowMapInitialized = true;
        std::cout << "Shadow Map initialized successfully!" << std::endl;
    } else {
        std::cerr << "Warning: Failed to initialize shadow map!" << std::endl;
    }

    // Create depth shader (for generating shadow map)
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
        depthShaderFound = false;
    }

    // Initialize ImGui (for game UI)
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window.GetGLFWWindow(), true);
    ImGui_ImplOpenGL3_Init("#version 330");
    std::cout << "ImGui initialized" << std::endl;

    std::cout << "\n=== Game Start ===" << std::endl;
    std::cout << "Entering game loop..." << std::endl;
    
    // Game loop
    float lastTime = static_cast<float>(glfwGetTime());  // Initialize lastTime to avoid large deltaTime on first frame
    
    while (!window.ShouldClose()) {
        try {
        // Calculate deltaTime
        float currentTime = static_cast<float>(glfwGetTime());
        float deltaTime = currentTime - lastTime;
        lastTime = currentTime;
        
        // Clamp deltaTime to avoid sudden large jumps (e.g., after program pause)
        if (deltaTime > 0.1f) {
            deltaTime = 0.1f;
        }

        // Process events
        window.PollEvents();

        // ESC to exit (also restore mouse)
        if (glfwGetKey(window.GetGLFWWindow(), GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetInputMode(window.GetGLFWWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            glfwSetWindowShouldClose(window.GetGLFWWindow(), true);
        }

        // Process player input (including movement, mouse control, shooting, etc.)
        fpsGameManager.ProcessPlayerInput(deltaTime, window.GetGLFWWindow(), 
                                          window.GetWidth(), window.GetHeight());

        // Update game logic
        fpsGameManager.Update(deltaTime);

        // Update weapon model position (based on zoom state)
        weaponModel.Update(fpsGameManager.IsZoomed(), window.GetWidth(), window.GetHeight());

        // ========== First Pass: Render Shadow Map (from light's perspective) ==========
        auto lights = lightManager.GetLights();
        glm::mat4 lightSpaceMatrix;
        bool shadowsEnabled = false;
        if (shadowMapInitialized && depthShaderFound && !lights.empty()) {
            auto firstLight = lights[0];
            glm::vec3 lightPos = firstLight->GetPosition();
            // Calculate light direction (light shines downward toward scene center)
            // For directional light, use downward direction
            glm::vec3 lightDir = glm::normalize(glm::vec3(0.0f, -1.0f, 0.0f));
            
            // Calculate light space matrix
            // Increase orthoSize to cover the entire arena (arena size is ~36 units, so use 40+ for safety)
            // Also increase farPlane to ensure walls are included
            lightSpaceMatrix = shadowMap.GetLightSpaceMatrix(lightPos, lightDir, 0.1f, 150.0f, 45.0f);
            
            // Begin rendering to shadow map
            shadowMap.BeginRender();
            
            // Use depth shader
            depthShader.Use();
            depthShader.SetMat4("lightSpaceMatrix", glm::value_ptr(lightSpaceMatrix));
            
            // Render scene to shadow map
            objectManager.Render(&depthShader);
            
            // End shadow map rendering
            shadowMap.EndRender();
            
            // Restore viewport
            glViewport(0, 0, window.GetWidth(), window.GetHeight());
            
            shadowsEnabled = true;
        }

        // ========== Second Pass: Normal Rendering (with shadow map) ==========
        // Clear buffers
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Use Shader
        shader.Use();

        // Calculate view and projection matrices (recalculate each frame as view may change)
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = camera.GetProjectionMatrix(aspectRatio);
        
        // Pass matrices to Shader
        shader.SetMat4("view", glm::value_ptr(view));
        shader.SetMat4("projection", glm::value_ptr(projection));
        shader.SetMat4("lightSpaceMatrix", glm::value_ptr(lightSpaceMatrix));
        
        // Set shadow-related uniforms
        shader.SetBool("useShadows", shadowsEnabled);
        if (shadowsEnabled) {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, shadowMap.GetDepthTexture());
            shader.SetInt("shadowMap", 0);
        }
        
        // Update scene
        objectManager.Update();
        
        // Set light parameters - ensure only one light (the sun)
        glm::vec3 viewPos = camera.GetPosition();
        
        // Ensure only one light exists (remove any extra lights that might have been added)
        while (lights.size() > 1) {
            lightManager.RemoveLight(lights.back());
            lights = lightManager.GetLights();
        }
        
        int numLights = std::min(static_cast<int>(lights.size()), 1);  // Only one light (the sun)
        
        shader.SetInt("numLights", numLights);
        // Global ambient light (independent of light sources, not affected by distance attenuation)
        // Reduced ambient light to make direct lighting more visible
        shader.SetVec3("globalAmbient", 0.1f, 0.1f, 0.1f);  // I_a: Global ambient light intensity (reduced)
        shader.SetVec3("viewPos", viewPos.x, viewPos.y, viewPos.z);
        
        // Set properties for the single light (the sun)
        if (numLights > 0) {
            auto light = lights[0];
            glm::vec3 lightPos = light->GetPosition();
            glm::vec3 lightColor = light->GetColor();
            float lightIntensity = light->GetIntensity();
            
            // Update sun sphere position to match light position
            std::string sunName = "Sun_" + light->GetName();
            auto sunNode = objectManager.FindNode(sunName);
            if (sunNode) {
                sunNode->SetPosition(lightPos);
            }
            
            std::string lightPrefix = "lights[0].";
            shader.SetVec3(lightPrefix + "position", lightPos.x, lightPos.y, lightPos.z);
            shader.SetVec3(lightPrefix + "color", lightColor.r, lightColor.g, lightColor.b);
            shader.SetFloat(lightPrefix + "intensity", lightIntensity);
            
            // Set distance attenuation parameters (reduced attenuation for better light reach)
            // For a light at y=15, these values allow good coverage of the arena
            shader.SetFloat(lightPrefix + "constant", 1.0f);
            shader.SetFloat(lightPrefix + "linear", 0.05f);      // Reduced linear attenuation
            shader.SetFloat(lightPrefix + "quadratic", 0.01f);   // Reduced quadratic attenuation
        }
        
        // Debug: Print light info on first frame (only one light - the sun)
        static bool firstFrame = true;
        if (firstFrame && numLights > 0) {
            auto light = lights[0];
            glm::vec3 pos = light->GetPosition();
            std::cout << "Rendering with 1 light (Sun) at (" << pos.x << ", " << pos.y << ", " << pos.z 
                      << "), intensity=" << light->GetIntensity() << std::endl;
            firstFrame = false;
        }

        // Render scene (including weapon, but weapon position needs special handling)
        // First save weapon node's original position and rotation
        auto weaponNode = objectManager.FindNode("WeaponBody");
        glm::vec3 weaponOriginalPos(0.0f);
        glm::vec3 weaponOriginalRot(0.0f);
        if (weaponNode) {
            weaponOriginalPos = weaponNode->GetPosition();
            weaponOriginalRot = weaponNode->GetRotation();
            
            // Calculate weapon's position and rotation in world coordinates (follow camera)
            glm::vec3 cameraPos = camera.GetPosition();
            glm::vec3 cameraFront = camera.GetFront();
            glm::vec3 cameraRight = camera.GetRight();
            glm::vec3 cameraUp = camera.GetUp();
            
            // Convert weapon local coordinates to world coordinates
            // Note: In camera coordinate system, positive z is forward (camera looks at -z, so positive z means forward)
            glm::vec3 weaponWorldPos = cameraPos + 
                cameraRight * weaponOriginalPos.x + 
                cameraUp * weaponOriginalPos.y + 
                cameraFront * weaponOriginalPos.z;  // positive z means camera forward
            
            // Temporarily set weapon's world position (for subsequent rendering)
            weaponNode->SetPosition(weaponWorldPos);
            weaponNode->SetRotation(weaponOriginalRot);
        }
        
        // First render other scene objects (excluding weapon)
        auto allNodes = objectManager.GetAllNodes();
        glm::mat4 identity = glm::mat4(1.0f);
        for (auto& node : allNodes) {
            if (node && node->GetName() != "WeaponBody" && 
                node->GetName() != "WeaponBarrel" && 
                node->GetName() != "WeaponStock" && 
                node->GetName() != "WeaponScope") {
                auto sceneNode = std::dynamic_pointer_cast<SoulsEngine::SceneNode>(node);
                if (sceneNode) {
                    sceneNode->Render(identity, &shader);
                }
            }
        }
        
        // Render weapon separately, using camera's rotation matrix to make it follow view
        if (weaponNode) {
            auto weaponSceneNode = std::dynamic_pointer_cast<SoulsEngine::SceneNode>(weaponNode);
            if (weaponSceneNode) {
                glm::vec3 cameraPos = camera.GetPosition();
                glm::vec3 cameraFront = camera.GetFront();
                glm::vec3 cameraRight = camera.GetRight();
                glm::vec3 cameraUp = camera.GetUp();
                
                // Calculate weapon world position (using original local position)
                glm::vec3 weaponWorldPos = cameraPos + 
                    cameraRight * weaponOriginalPos.x + 
                    cameraUp * weaponOriginalPos.y + 
                    cameraFront * weaponOriginalPos.z;
                
                // Get weapon's local rotation and scale (needed for scope alignment calculation)
                glm::vec3 weaponRot = weaponNode->GetRotation();
                
                // Build camera's rotation matrix (from camera's direction vectors)
                glm::mat4 cameraRotation = glm::mat4(1.0f);
                cameraRotation[0] = glm::vec4(cameraRight, 0.0f);   // First column: right vector
                cameraRotation[1] = glm::vec4(cameraUp, 0.0f);     // Second column: up vector
                cameraRotation[2] = glm::vec4(-cameraFront, 0.0f); // Third column: front vector (negative)
                cameraRotation[3] = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
                
                // When zoomed, adjust weapon position so scope center aligns with crosshair
                if (fpsGameManager.IsZoomed()) {
                    auto scopeNode = weaponModel.GetScopeNode();
                    if (scopeNode) {
                        // Get scope's local position relative to weapon body
                        glm::vec3 scopeLocalPos = scopeNode->GetPosition();
                        
                        // Calculate where scope center should be to align with crosshair
                        // Crosshair is at screen center, which corresponds to camera forward direction
                        float scopeDistance = 1.0f;  // Distance from camera to scope center when zoomed
                        glm::vec3 targetScopeCenter = cameraPos + cameraFront * scopeDistance;
                        
                        // Build weapon's local rotation matrix (same order as in rendering code)
                        glm::mat4 weaponRotMat = glm::mat4(1.0f);
                        weaponRotMat = glm::rotate(weaponRotMat, glm::radians(weaponRot.z), glm::vec3(0.0f, 0.0f, 1.0f));
                        weaponRotMat = glm::rotate(weaponRotMat, glm::radians(weaponRot.y), glm::vec3(0.0f, 1.0f, 0.0f));
                        weaponRotMat = glm::rotate(weaponRotMat, glm::radians(weaponRot.x), glm::vec3(1.0f, 0.0f, 0.0f));
                        
                        // Transform scope local position through camera rotation and weapon rotation
                        // Order: cameraRotation * weaponRotMat * scopeLocalPos
                        glm::vec4 scopePosRotated = cameraRotation * weaponRotMat * glm::vec4(scopeLocalPos, 1.0f);
                        glm::vec3 scopeOffsetInWorld = glm::vec3(scopePosRotated);
                        
                        // Adjust weapon position so that scope center aligns with target
                        // targetScopeCenter = weaponWorldPos + scopeOffsetInWorld
                        // Therefore: weaponWorldPos = targetScopeCenter - scopeOffsetInWorld
                        weaponWorldPos = targetScopeCenter - scopeOffsetInWorld;
                    }
                }
                
                // Get weapon's local scale
                glm::vec3 weaponScale = weaponNode->GetScale();
                
                // Build weapon's complete transformation matrix: Translation * CameraRotation * WeaponLocalRotation * Scale
                glm::mat4 weaponTransform = glm::translate(glm::mat4(1.0f), weaponWorldPos);
                weaponTransform = weaponTransform * cameraRotation;
                
                // Apply weapon's local rotation (relative to camera coordinate system)
                weaponTransform = glm::rotate(weaponTransform, glm::radians(weaponRot.z), glm::vec3(0.0f, 0.0f, 1.0f));
                weaponTransform = glm::rotate(weaponTransform, glm::radians(weaponRot.y), glm::vec3(0.0f, 1.0f, 0.0f));
                weaponTransform = glm::rotate(weaponTransform, glm::radians(weaponRot.x), glm::vec3(1.0f, 0.0f, 0.0f));
                
                // Apply scale
                weaponTransform = glm::scale(weaponTransform, weaponScale);
                
                // Set model matrix directly to shader, then render weapon and its children
                shader.SetMat4("model", glm::value_ptr(weaponTransform));
                
                // Render weapon main node
                if (weaponSceneNode->GetMesh()) {
                    // Apply material
                    const auto& material = weaponSceneNode->GetMaterial();
                    if (material) {
                        shader.SetVec3("material.ambient", material->GetAmbient().r, material->GetAmbient().g, material->GetAmbient().b);
                        shader.SetVec3("material.diffuse", material->GetDiffuse().r, material->GetDiffuse().g, material->GetDiffuse().b);
                        shader.SetVec3("material.specular", material->GetSpecular().r, material->GetSpecular().g, material->GetSpecular().b);
                        shader.SetFloat("material.shininess", material->GetShininess());
                    } else {
                        static auto defaultMat = SoulsEngine::Material::CreateDefault();
                        shader.SetVec3("material.ambient", defaultMat.GetAmbient().r, defaultMat.GetAmbient().g, defaultMat.GetAmbient().b);
                        shader.SetVec3("material.diffuse", defaultMat.GetDiffuse().r, defaultMat.GetDiffuse().g, defaultMat.GetDiffuse().b);
                        shader.SetVec3("material.specular", defaultMat.GetSpecular().r, defaultMat.GetSpecular().g, defaultMat.GetSpecular().b);
                        shader.SetFloat("material.shininess", defaultMat.GetShininess());
                    }
                    weaponSceneNode->GetMesh()->Draw();
                }
                
                // Render weapon's child nodes (barrel, stock, scope)
                for (auto& child : weaponNode->GetChildren()) {
                    auto childSceneNode = std::dynamic_pointer_cast<SoulsEngine::SceneNode>(child);
                    if (childSceneNode && childSceneNode->GetMesh()) {
                        // Calculate child node's world transformation
                        glm::vec3 childPos = child->GetPosition();
                        glm::vec3 childRot = child->GetRotation();
                        glm::vec3 childScale = child->GetScale();
                        
                        glm::mat4 childTransform = weaponTransform;
                        childTransform = glm::translate(childTransform, childPos);
                        childTransform = glm::rotate(childTransform, glm::radians(childRot.z), glm::vec3(0.0f, 0.0f, 1.0f));
                        childTransform = glm::rotate(childTransform, glm::radians(childRot.y), glm::vec3(0.0f, 1.0f, 0.0f));
                        childTransform = glm::rotate(childTransform, glm::radians(childRot.x), glm::vec3(1.0f, 0.0f, 0.0f));
                        childTransform = glm::scale(childTransform, childScale);
                        
                        shader.SetMat4("model", glm::value_ptr(childTransform));
                        
                        // Apply material
                        const auto& childMaterial = childSceneNode->GetMaterial();
                        if (childMaterial) {
                            shader.SetVec3("material.ambient", childMaterial->GetAmbient().r, childMaterial->GetAmbient().g, childMaterial->GetAmbient().b);
                            shader.SetVec3("material.diffuse", childMaterial->GetDiffuse().r, childMaterial->GetDiffuse().g, childMaterial->GetDiffuse().b);
                            shader.SetVec3("material.specular", childMaterial->GetSpecular().r, childMaterial->GetSpecular().g, childMaterial->GetSpecular().b);
                            shader.SetFloat("material.shininess", childMaterial->GetShininess());
                        } else {
                            static auto defaultMat = SoulsEngine::Material::CreateDefault();
                            shader.SetVec3("material.ambient", defaultMat.GetAmbient().r, defaultMat.GetAmbient().g, defaultMat.GetAmbient().b);
                            shader.SetVec3("material.diffuse", defaultMat.GetDiffuse().r, defaultMat.GetDiffuse().g, defaultMat.GetDiffuse().b);
                            shader.SetVec3("material.specular", defaultMat.GetSpecular().r, defaultMat.GetSpecular().g, defaultMat.GetSpecular().b);
                            shader.SetFloat("material.shininess", defaultMat.GetShininess());
                        }
                        childSceneNode->GetMesh()->Draw();
                    }
                }
            }
        }
        
        // Restore weapon's local position and rotation
        if (weaponNode) {
            weaponNode->SetPosition(weaponOriginalPos);
            weaponNode->SetRotation(weaponOriginalRot);
        }

        // Render game UI (using ImGui)
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Game UI window
        {
            ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Always);
            ImGui::SetNextWindowSize(ImVec2(250, 150), ImGuiCond_Always);
            ImGui::Begin("Game Info", nullptr, 
                         ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | 
                         ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);
            
            // Display score
            ImGui::Text("Score: %d", fpsGameManager.GetScore());
            ImGui::Separator();
            
            // Game status
            if (fpsGameManager.IsGameOver()) {
                ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Game Over!");
            } else {
                ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Playing...");
            }
            
            ImGui::Separator();
            ImGui::Text("Controls:");
            ImGui::BulletText("WASD - Move");
            ImGui::BulletText("Shift - Sprint");
            ImGui::BulletText("Ctrl - Crouch");
            ImGui::BulletText("Space - Jump");
            ImGui::BulletText("Mouse - Rotate view");
            ImGui::BulletText("Right-click - Zoom");
            ImGui::BulletText("Left-click - Shoot");
            ImGui::BulletText("ESC - Exit");
            
            ImGui::End();
        }

        // Draw crosshair (after ImGui::NewFrame(), before ImGui::Render())
        DrawCrosshairImGui(window.GetWidth(), window.GetHeight());
        
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        
        // Swap buffers
        window.SwapBuffers();

        // Check OpenGL errors
        GL_CHECK_ERROR();
        
        } catch (const std::exception& e) {
            std::cerr << "Exception in game loop: " << e.what() << std::endl;
            break;
        } catch (...) {
            std::cerr << "Unknown exception in game loop!" << std::endl;
            break;
        }
    }

    // Cleanup resources
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    objectManager.Clear();

    std::cout << "Game ended, closing..." << std::endl;
    
    } catch (const std::exception& e) {
        std::cerr << "Exception occurred: " << e.what() << std::endl;
        std::cout << "Press Enter to exit..." << std::endl;
        std::cin.get();
        return -1;
    } catch (...) {
        std::cerr << "Unknown exception occurred!" << std::endl;
        std::cout << "Press Enter to exit..." << std::endl;
        std::cin.get();
        return -1;
    }
    
    return 0;
}

