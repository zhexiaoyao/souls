#include "ImGuiSystem.h"
#include "Camera.h"
#include "ObjectManager.h"
#include "SelectionSystem.h"
#include "SceneNode.h"
// 材质系统已删除
// #include "Material.h"
#include "Light.h"
#include "LightManager.h"
#include "../geometry/Cube.h"
#include "../geometry/Sphere.h"
#include "../geometry/Cylinder.h"
#include "../geometry/Cone.h"
#include "../geometry/Prism.h"
#include "../geometry/Frustum.h"
#include <cstdio>  // 用于fopen
#include <algorithm>  // 用于std::remove_if
#include <glm/glm.hpp>  // 用于glm::vec3

// ImGui includes
// 注意：必须在包含GLAD之后包含ImGui
// 定义使用自定义loader（GLAD），这样ImGui不会使用自己的loader
#define IMGUI_IMPL_OPENGL_LOADER_CUSTOM
#include <glad/glad.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace SoulsEngine {

ImGuiSystem::ImGuiSystem()
    : m_window(nullptr)
    , m_context(nullptr)
    , m_showGeometryMenu(false)
    , m_showMaterialMenu(false)
    , m_showLightMenu(false)
    , m_showModelMenu(false)
    , m_lightAngle(45.0f)
    , m_lightIntensity(1.0f)
{
}

ImGuiSystem::~ImGuiSystem() {
    Shutdown();
}

bool ImGuiSystem::Initialize(GLFWwindow* window) {
    m_window = window;
    
    // 设置ImGui上下文
    IMGUI_CHECKVERSION();
    m_context = ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    
    // 设置样式
    ImGui::StyleColorsDark();
    
    // 加载中文字体支持（必须在初始化渲染器之前）
    // 添加中文字符范围（CJK统一汉字）
    static const ImWchar ranges[] = {
        0x0020, 0x00FF, // 基本拉丁字符
        0x0100, 0x017F, // 拉丁扩展-A
        0x4E00, 0x9FFF, // CJK统一汉字
        0x3000, 0x303F, // CJK符号和标点
        0x3040, 0x309F, // 平假名
        0x30A0, 0x30FF, // 片假名
        0xFF00, 0xFFEF, // 全角ASCII、全角标点
        0,
    };
    
    // 尝试加载Windows系统字体（如果可用）
    const char* fontPaths[] = {
        "C:/Windows/Fonts/msyh.ttc",        // 微软雅黑（推荐）
        "C:/Windows/Fonts/simsun.ttc",      // 宋体
        "C:/Windows/Fonts/simhei.ttf",      // 黑体
        "C:/Windows/Fonts/simkai.ttf",      // 楷体
    };
    
    bool fontLoaded = false;
    for (const char* fontPath : fontPaths) {
        FILE* file = fopen(fontPath, "rb");
        if (file) {
            fclose(file);
            // 先加载默认字体
            io.Fonts->AddFontDefault();
            // 然后合并中文字体
            ImFontConfig config;
            config.MergeMode = true;
            config.GlyphMinAdvanceX = 13.0f;
            io.Fonts->AddFontFromFileTTF(fontPath, 16.0f, &config, ranges);
            fontLoaded = true;
            break;
        }
    }
    
    // 如果无法加载中文字体，使用默认字体
    if (!fontLoaded) {
        io.Fonts->AddFontDefault();
    }
    
    // 初始化平台/渲染器绑定
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
    
    // 重建字体纹理（必须在初始化渲染器之后）
    unsigned char* pixels;
    int width, height;
    io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);
    
    return true;
}

void ImGuiSystem::Shutdown() {
    if (m_context) {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext(m_context);
        m_context = nullptr;
    }
}

void ImGuiSystem::BeginFrame() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void ImGuiSystem::EndFrame() {
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void ImGuiSystem::RenderSidebar(ObjectManager* objectManager, SelectionSystem* selectionSystem, 
                                 Camera* camera, LightManager* lightManager, float aspectRatio) {
    if (!objectManager || !selectionSystem || !camera || !lightManager) return;
    
    // 设置侧栏位置和大小（固定在左侧）
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(ImVec2(viewport->Pos.x, viewport->Pos.y), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(200, viewport->Size.y), ImGuiCond_Always);
    
    // 开始侧栏窗口
    ImGui::Begin("游戏引擎工具栏", nullptr, 
                 ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);
    
    // 标题
    ImGui::Text("工具栏");
    ImGui::Separator();
    ImGui::Spacing();
    
    // 几何体计数器
    static int geometryCounter = 0;
    
    // 按键1：生成几何体（折叠菜单）
    if (ImGui::CollapsingHeader("1. 生成几何体", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::Indent();
        
        // 立方体
        if (ImGui::Button("立方体", ImVec2(-1, 0))) {
            auto mesh = std::make_shared<Cube>(1.0f, glm::vec3(1.0f, 0.0f, 0.0f));
            std::string name = "Cube_" + std::to_string(geometryCounter++);
            auto newNode = objectManager->CreateNode(name, mesh);
            // 材质系统已删除，设置材质代码已注释
            // auto material = std::make_shared<Material>();
            // material->SetColor(1.0f, 0.0f, 0.0f);
            // material->SetName("RedMaterial");
            // newNode->SetMaterial(material);
            glm::vec3 cameraPos = camera->GetPosition();
            glm::vec3 cameraFront = camera->GetFront();
            newNode->SetPosition(cameraPos + cameraFront * 5.0f);
            selectionSystem->SelectNode(newNode);
        }
        
        // 球体
        if (ImGui::Button("球体", ImVec2(-1, 0))) {
            auto mesh = std::make_shared<Sphere>(0.8f, 36, 18, glm::vec3(0.0f, 1.0f, 0.0f));
            std::string name = "Sphere_" + std::to_string(geometryCounter++);
            auto newNode = objectManager->CreateNode(name, mesh);
            // 材质系统已删除，设置材质代码已注释
            // auto material = std::make_shared<Material>(Material::CreateEmerald());
            // newNode->SetMaterial(material);
            glm::vec3 cameraPos = camera->GetPosition();
            glm::vec3 cameraFront = camera->GetFront();
            newNode->SetPosition(cameraPos + cameraFront * 5.0f);
            selectionSystem->SelectNode(newNode);
        }
        
        // 圆柱体
        if (ImGui::Button("圆柱体", ImVec2(-1, 0))) {
            auto mesh = std::make_shared<Cylinder>(0.7f, 1.5f, 36, glm::vec3(0.0f, 0.0f, 1.0f));
            std::string name = "Cylinder_" + std::to_string(geometryCounter++);
            auto newNode = objectManager->CreateNode(name, mesh);
            // 材质系统已删除，设置材质代码已注释
            // auto material = std::make_shared<Material>();
            // material->SetColor(0.0f, 0.0f, 1.0f);
            // material->SetName("BlueMaterial");
            // newNode->SetMaterial(material);
            glm::vec3 cameraPos = camera->GetPosition();
            glm::vec3 cameraFront = camera->GetFront();
            newNode->SetPosition(cameraPos + cameraFront * 5.0f);
            selectionSystem->SelectNode(newNode);
        }
        
        // 圆锥体
        if (ImGui::Button("圆锥体", ImVec2(-1, 0))) {
            auto mesh = std::make_shared<Cone>(0.7f, 1.5f, 36, glm::vec3(1.0f, 1.0f, 0.0f));
            std::string name = "Cone_" + std::to_string(geometryCounter++);
            auto newNode = objectManager->CreateNode(name, mesh);
            // 材质系统已删除，设置材质代码已注释
            // auto material = std::make_shared<Material>(Material::CreateGold());
            // newNode->SetMaterial(material);
            glm::vec3 cameraPos = camera->GetPosition();
            glm::vec3 cameraFront = camera->GetFront();
            newNode->SetPosition(cameraPos + cameraFront * 5.0f);
            selectionSystem->SelectNode(newNode);
        }
        
        // 棱柱
        if (ImGui::Button("棱柱", ImVec2(-1, 0))) {
            auto mesh = std::make_shared<Prism>(6, 0.7f, 1.5f, glm::vec3(1.0f, 0.0f, 1.0f));
            std::string name = "Prism_" + std::to_string(geometryCounter++);
            auto newNode = objectManager->CreateNode(name, mesh);
            // 材质系统已删除，设置材质代码已注释
            // auto material = std::make_shared<Material>();
            // material->SetColor(1.0f, 0.0f, 1.0f);
            // material->SetName("MagentaMaterial");
            // newNode->SetMaterial(material);
            glm::vec3 cameraPos = camera->GetPosition();
            glm::vec3 cameraFront = camera->GetFront();
            newNode->SetPosition(cameraPos + cameraFront * 5.0f);
            selectionSystem->SelectNode(newNode);
        }
        
        // 棱台
        if (ImGui::Button("棱台", ImVec2(-1, 0))) {
            auto mesh = std::make_shared<Frustum>(6, 0.4f, 0.7f, 1.5f, glm::vec3(0.0f, 1.0f, 1.0f));
            std::string name = "Frustum_" + std::to_string(geometryCounter++);
            auto newNode = objectManager->CreateNode(name, mesh);
            // 材质系统已删除，设置材质代码已注释
            // auto material = std::make_shared<Material>(Material::CreateJade());
            // newNode->SetMaterial(material);
            glm::vec3 cameraPos = camera->GetPosition();
            glm::vec3 cameraFront = camera->GetFront();
            newNode->SetPosition(cameraPos + cameraFront * 5.0f);
            selectionSystem->SelectNode(newNode);
        }
        
        ImGui::Unindent();
    }
    
    ImGui::Spacing();
    
    // 按键2：生成特定模型（暂时只做前端）
    if (ImGui::CollapsingHeader("2. 生成特定模型", ImGuiTreeNodeFlags_None)) {
        ImGui::Indent();
        ImGui::Text("功能开发中...");
        ImGui::Text("敬请期待");
        ImGui::Unindent();
    }
    
    ImGui::Spacing();
    
    // 按键3：给选中的物体添加材质
    if (ImGui::CollapsingHeader("3. 添加材质", ImGuiTreeNodeFlags_None)) {
        auto selectedNode = selectionSystem->GetSelectedNode();
        if (!selectedNode) {
            ImGui::Indent();
            ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "请先选择对象");
            ImGui::Unindent();
        } else {
            ImGui::Indent();
            
            // 默认材质
            if (ImGui::Button("默认材质", ImVec2(-1, 0))) {
                // 材质系统已删除，按钮效果已注释
                // auto material = std::make_shared<Material>(Material::CreateDefault());
                // selectedNode->SetMaterial(material);
            }
            
            // 翡翠材质
            if (ImGui::Button("翡翠", ImVec2(-1, 0))) {
                // 材质系统已删除，按钮效果已注释
                // auto material = std::make_shared<Material>(Material::CreateEmerald());
                // selectedNode->SetMaterial(material);
            }
            
            // 玉石材质
            if (ImGui::Button("玉石", ImVec2(-1, 0))) {
                // 材质系统已删除，按钮效果已注释
                // auto material = std::make_shared<Material>(Material::CreateJade());
                // selectedNode->SetMaterial(material);
            }
            
            // 红宝石材质
            if (ImGui::Button("红宝石", ImVec2(-1, 0))) {
                // 材质系统已删除，按钮效果已注释
                // auto material = std::make_shared<Material>(Material::CreateRuby());
                // selectedNode->SetMaterial(material);
            }
            
            // 金色材质
            if (ImGui::Button("金色", ImVec2(-1, 0))) {
                // 材质系统已删除，按钮效果已注释
                // auto material = std::make_shared<Material>(Material::CreateGold());
                // selectedNode->SetMaterial(material);
            }
            
            // 银色材质
            if (ImGui::Button("银色", ImVec2(-1, 0))) {
                // 材质系统已删除，按钮效果已注释
                // auto material = std::make_shared<Material>(Material::CreateSilver());
                // selectedNode->SetMaterial(material);
            }
            
            ImGui::Unindent();
        }
    }
    
    ImGui::Spacing();
    
    // 按键4：添加灯光
    if (ImGui::CollapsingHeader("4. 添加灯光", ImGuiTreeNodeFlags_None)) {
        ImGui::Indent();
        
        // 光照张角滑块（0-360度）
        ImGui::Text("光照张角: %.1f 度", m_lightAngle);
        ImGui::SliderFloat("##LightAngle", &m_lightAngle, 0.0f, 360.0f, "%.1f");
        
        // 光照强度滑块（0-10）
        ImGui::Text("光照强度: %.2f", m_lightIntensity);
        ImGui::SliderFloat("##LightIntensity", &m_lightIntensity, 0.0f, 10.0f, "%.2f");
        
        ImGui::Spacing();
        
        // 添加光源按钮
        if (ImGui::Button("添加光源", ImVec2(-1, 0))) {
            glm::vec3 cameraPos = camera->GetPosition();
            glm::vec3 cameraFront = camera->GetFront();
            glm::vec3 lightPos = cameraPos + cameraFront * 5.0f;
            glm::vec3 lightColor(1.0f, 1.0f, 1.0f);  // 白色光源
            
            auto light = lightManager->AddLight(lightPos, lightColor, m_lightIntensity, m_lightAngle);
            
            // 创建一个小球体线框来显示光源位置
            auto mesh = std::make_shared<Sphere>(0.2f, 16, 8, glm::vec3(1.0f, 1.0f, 0.0f));  // 黄色小球
            std::string name = "LightIndicator_" + light->GetName();
            auto lightNode = objectManager->CreateNode(name, mesh);
            lightNode->SetPosition(lightPos);
            // 材质系统已删除，设置材质代码已注释
            // auto material = std::make_shared<Material>();
            // material->SetColor(1.0f, 1.0f, 0.0f);  // 黄色
            // material->SetSpecular(1.0f, 1.0f, 1.0f);
            // material->SetShininess(128.0f);
            // material->SetName("LightMaterial");
            // lightNode->SetMaterial(material);
        }
        
        // 显示当前光源列表
        auto lights = lightManager->GetLights();
        if (!lights.empty()) {
            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Text("光源列表:");
            for (size_t i = 0; i < lights.size(); ++i) {
                auto light = lights[i];
                ImGui::Text("%s", light->GetName().c_str());
                glm::vec3 pos = light->GetPosition();
                ImGui::Text("  位置: (%.2f, %.2f, %.2f)", pos.x, pos.y, pos.z);
                ImGui::Text("  强度: %.2f", light->GetIntensity());
                ImGui::Text("  张角: %.1f 度", light->GetAngle());
                
                // 实时更新光源参数（当滑块改变时）
                light->SetIntensity(m_lightIntensity);
                light->SetAngle(m_lightAngle);
                
                // 注意：不要在这里更新光源指示器位置，因为：
                // 1. 如果指示器正在被拖拽，位置应该由拖拽系统控制
                // 2. 指示器位置应该在main.cpp的渲染循环中统一管理
                // 3. 避免位置被意外重置
                
                // 删除按钮
                ImGui::PushID(static_cast<int>(i));
                if (ImGui::SmallButton("删除")) {
                    // 找到并删除对应的光源指示器节点
                    std::string indicatorName = "LightIndicator_" + light->GetName();
                    auto indicatorNode = objectManager->FindNode(indicatorName);
                    if (indicatorNode) {
                        objectManager->RemoveNode(indicatorNode);
                    }
                    lightManager->RemoveLight(light);
                }
                ImGui::PopID();
                
                if (i < lights.size() - 1) {
                    ImGui::Spacing();
                }
            }
        }
        
        ImGui::Unindent();
    }
    
    ImGui::Spacing();
    ImGui::Separator();
    
    // 显示选中对象信息
    auto selectedNode = selectionSystem->GetSelectedNode();
    if (selectedNode) {
        ImGui::Spacing();
        ImGui::Text("选中对象:");
        ImGui::Text("%s", selectedNode->GetName().c_str());
        
        // 显示位置信息
        glm::vec3 pos = selectedNode->GetPosition();
        ImGui::Text("位置: (%.2f, %.2f, %.2f)", pos.x, pos.y, pos.z);
        
        // 显示旋转信息
        glm::vec3 rot = selectedNode->GetRotation();
        ImGui::Text("旋转: (%.2f, %.2f, %.2f)", rot.x, rot.y, rot.z);
        
        // 显示缩放信息
        glm::vec3 scale = selectedNode->GetScale();
        ImGui::Text("缩放: (%.2f, %.2f, %.2f)", scale.x, scale.y, scale.z);
        
        // 材质系统已删除，显示材质信息代码已注释
        // auto material = selectedNode->GetMaterial();
        // if (material) {
        //     ImGui::Text("材质: %s", material->GetName().c_str());
        // } else {
        //     ImGui::Text("材质: 默认");
        // }
    } else {
        ImGui::Spacing();
        ImGui::Text("未选中对象");
    }
    
    ImGui::End();
}

} // namespace SoulsEngine

