#include "ImGuiSystem.h"

#include "Camera.h"
#include "Light.h"
#include "LightManager.h"
#include "ObjectManager.h"
#include "SceneNode.h"
#include "SelectionSystem.h"
#include "Material.h"
#include "../geometry/Cone.h"
#include "../geometry/Cube.h"
#include "../geometry/Cylinder.h"
#include "../geometry/Frustum.h"
#include "../geometry/Prism.h"
#include "../geometry/Sphere.h"

#include <algorithm>
#include <cstdio>
#include <glm/glm.hpp>

// ImGui includes (loader must come from GLAD)
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
    , m_lightIntensity(1.0f) {
    InitMaterialPresets();
}

ImGuiSystem::~ImGuiSystem() {
    Shutdown();
}

bool ImGuiSystem::Initialize(GLFWwindow* window) {
    m_window = window;


    IMGUI_CHECKVERSION();
    m_context = ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    ImGui::StyleColorsDark();

    // 中文字符范围
    static const ImWchar ranges[] = {
        0x0020, 0x00FF, // 基本拉丁字符
        0x0100, 0x017F, // 拉丁扩展字符
        0x3000, 0x303F, // CJK 符号和标点
        0x3040, 0x30FF, // 日文平假名
        0x4E00, 0x9FFF, // 中文汉字
        0xFF00, 0xFFEF, // 全角字符
        0,
    };


    const char* fontPaths[] = {
        "C:/Windows/Fonts/msyh.ttc",
        "C:/Windows/Fonts/simsun.ttc",
        "C:/Windows/Fonts/simhei.ttf",
        "C:/Windows/Fonts/simkai.ttf",
    };

    bool fontLoaded = false;
    for (const char* fontPath : fontPaths) {
        FILE* file = fopen(fontPath, "rb");
        if (!file) continue;
        fclose(file);

        io.Fonts->AddFontDefault();
        ImFontConfig config;
        config.MergeMode = true;
        config.GlyphMinAdvanceX = 13.0f;
        io.Fonts->AddFontFromFileTTF(fontPath, 16.0f, &config, ranges);
        fontLoaded = true;
        break;
    }

    if (!fontLoaded) {
        io.Fonts->AddFontDefault();
    }


    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");


    unsigned char* pixels;
    int width;
    int height;
    io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

    return true;
}

void ImGuiSystem::Shutdown() {
    if (!m_context) return;
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext(m_context);
    m_context = nullptr;
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

void ImGuiSystem::InitMaterialPresets() {
    m_materialPresets.clear();
    m_materialPresets.push_back(Material::CreateDefault());
    m_materialPresets.push_back(Material::CreateEmerald());
    m_materialPresets.push_back(Material::CreateJade());
    m_materialPresets.push_back(Material::CreateRuby());
    m_materialPresets.push_back(Material::CreateGlass());
    m_materialPresets.push_back(Material::CreateMetal());
}

void ImGuiSystem::RenderSidebar(ObjectManager* objectManager,
                                SelectionSystem* selectionSystem,
                                Camera* camera,
                                LightManager* lightManager,
                                float /*aspectRatio*/) {
    if (!objectManager || !selectionSystem || !camera || !lightManager) return;

    // 固定左侧工具栏
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(ImVec2(viewport->Pos.x, viewport->Pos.y), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(200, viewport->Size.y), ImGuiCond_Always);

    ImGui::Begin("Souls 工具栏", nullptr,
                 ImGuiWindowFlags_NoMove |
                     ImGuiWindowFlags_NoResize |
                     ImGuiWindowFlags_NoCollapse |
                     ImGuiWindowFlags_NoTitleBar);

    ImGui::Text("工具栏");
    ImGui::Separator();
    ImGui::Spacing();

    static int geometryCounter = 0;


    if (ImGui::CollapsingHeader("1. 预设几何体", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::Indent();

        if (ImGui::Button("立方体", ImVec2(-1, 0))) {
            auto mesh = std::make_shared<Cube>(1.0f, glm::vec3(1.0f, 0.0f, 0.0f));
            auto name = "Cube_" + std::to_string(geometryCounter++);
            auto node = objectManager->CreateNode(name, mesh);
            glm::vec3 camPos = camera->GetPosition();
            glm::vec3 camFront = camera->GetFront();
            node->SetPosition(camPos + camFront * 5.0f);
            selectionSystem->SelectNode(node);
        }

        if (ImGui::Button("球体", ImVec2(-1, 0))) {
            auto mesh = std::make_shared<Sphere>(0.8f, 36, 18, glm::vec3(0.0f, 1.0f, 0.0f));
            auto name = "Sphere_" + std::to_string(geometryCounter++);
            auto node = objectManager->CreateNode(name, mesh);
            glm::vec3 camPos = camera->GetPosition();
            glm::vec3 camFront = camera->GetFront();
            node->SetPosition(camPos + camFront * 5.0f);
            selectionSystem->SelectNode(node);
        }

        if (ImGui::Button("圆柱体", ImVec2(-1, 0))) {
            auto mesh = std::make_shared<Cylinder>(0.7f, 1.5f, 36, glm::vec3(0.0f, 0.0f, 1.0f));
            auto name = "Cylinder_" + std::to_string(geometryCounter++);
            auto node = objectManager->CreateNode(name, mesh);
            glm::vec3 camPos = camera->GetPosition();
            glm::vec3 camFront = camera->GetFront();
            node->SetPosition(camPos + camFront * 5.0f);
            selectionSystem->SelectNode(node);
        }

        if (ImGui::Button("圆锥体", ImVec2(-1, 0))) {
            auto mesh = std::make_shared<Cone>(0.7f, 1.5f, 36, glm::vec3(1.0f, 1.0f, 0.0f));
            auto name = "Cone_" + std::to_string(geometryCounter++);
            auto node = objectManager->CreateNode(name, mesh);
            glm::vec3 camPos = camera->GetPosition();
            glm::vec3 camFront = camera->GetFront();
            node->SetPosition(camPos + camFront * 5.0f);
            selectionSystem->SelectNode(node);
        }

        if (ImGui::Button("棱柱体", ImVec2(-1, 0))) {
            auto mesh = std::make_shared<Prism>(6, 0.7f, 1.5f, glm::vec3(1.0f, 0.0f, 1.0f));
            auto name = "Prism_" + std::to_string(geometryCounter++);
            auto node = objectManager->CreateNode(name, mesh);
            glm::vec3 camPos = camera->GetPosition();
            glm::vec3 camFront = camera->GetFront();
            node->SetPosition(camPos + camFront * 5.0f);
            selectionSystem->SelectNode(node);
        }

        if (ImGui::Button("棱台体", ImVec2(-1, 0))) {
            auto mesh = std::make_shared<Frustum>(6, 0.4f, 0.7f, 1.5f, glm::vec3(0.0f, 1.0f, 1.0f));
            auto name = "Frustum_" + std::to_string(geometryCounter++);
            auto node = objectManager->CreateNode(name, mesh);
            glm::vec3 camPos = camera->GetPosition();
            glm::vec3 camFront = camera->GetFront();
            node->SetPosition(camPos + camFront * 5.0f);
            selectionSystem->SelectNode(node);
        }

        ImGui::Unindent();
    }

    ImGui::Spacing();

    // 2. 生成特定模型（占位）
    if (ImGui::CollapsingHeader("2. 生成特定模型", ImGuiTreeNodeFlags_None)) {
        ImGui::Indent();
        ImGui::Text("功能开发中...");
        ImGui::Text("敬请期待");
        ImGui::Unindent();
    }

    ImGui::Spacing();

    // 3. 添加材质
    if (ImGui::CollapsingHeader("3. 添加材质", ImGuiTreeNodeFlags_DefaultOpen)) {
        auto selectedNode = selectionSystem->GetSelectedNode();
        if (!selectedNode) {
            ImGui::Indent();
            ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "请先选择物体");
            ImGui::Unindent();
        } else {
            ImGui::Indent();
            auto currentMat = selectedNode->GetMaterial();
            ImGui::Text("当前材质: %s", currentMat ? currentMat->GetName().c_str() : "无材质");
            if (currentMat) {
                if (ImGui::SmallButton("移除材质")) {
                    selectedNode->SetMaterial(nullptr);
                }
            }

            ImGui::Spacing();
            ImGui::Text("棰勮鏉愯川:");

            for (size_t i = 0; i < m_materialPresets.size(); ++i) {
                const auto& preset = m_materialPresets[i];
                ImGui::PushID(static_cast<int>(i));

                glm::vec3 diffuse = preset.GetDiffuse();
                ImVec4 color(diffuse.r, diffuse.g, diffuse.b, 1.0f);
                ImGui::ColorButton("##Preview", color, ImGuiColorEditFlags_NoTooltip, ImVec2(18, 18));
                ImGui::SameLine();
                ImGui::Text("%s", preset.GetName().c_str());
                ImGui::SameLine();
                if (ImGui::SmallButton("应用")) {
                    selectedNode->SetMaterial(std::make_shared<Material>(preset));
                }

                ImGui::PopID();
            }

            ImGui::Spacing();
            ImGui::Separator();
            // 表面类型选择（哑光 / 光滑）
            ImGui::Text("表面类型:");
            // 0=默认, 1=哑光(粗糙), 2=光滑(光泽)
            static int surfaceType = 0;
            int prevSurfaceType = surfaceType;
            ImGui::RadioButton("默认", &surfaceType, 0); ImGui::SameLine();
            ImGui::RadioButton("哑光 (粗糙)", &surfaceType, 1); ImGui::SameLine();
            ImGui::RadioButton("光滑 (光泽)", &surfaceType, 2);
            // 当选择发生变化时，立即应用到当前选中物体的材质（通过修改 shininess）
            if (surfaceType != prevSurfaceType) {
                auto mat = selectedNode->GetMaterial();
                if (mat) {
                    if (surfaceType == 1) {
                        // 哑光：较低的 shininess 值
                        mat->SetShininess(8.0f);
                    } else if (surfaceType == 2) {
                        // 光滑：较高的 shininess 值
                        mat->SetShininess(128.0f);
                    } else {
                        // 默认值（项目中默认为32）
                        mat->SetShininess(32.0f);
                    }
                }
            }

            ImGui::Unindent();
        }
    }

    ImGui::Spacing();

    // 4. 添加光源
    if (ImGui::CollapsingHeader("4. 添加光源", ImGuiTreeNodeFlags_None)) {
        ImGui::Indent();

        ImGui::Text("光照角度: %.1f", m_lightAngle);
        ImGui::SliderFloat("##LightAngle", &m_lightAngle, 0.0f, 360.0f, "%.1f");

        ImGui::Text("光照强度: %.2f", m_lightIntensity);
        ImGui::SliderFloat("##LightIntensity", &m_lightIntensity, 0.0f, 10.0f, "%.2f");

        ImGui::Spacing();

        if (ImGui::Button("添加光源", ImVec2(-1, 0))) {
            glm::vec3 camPos = camera->GetPosition();
            glm::vec3 camFront = camera->GetFront();
            glm::vec3 lightPos = camPos + camFront * 5.0f;
            glm::vec3 lightColor(1.0f, 1.0f, 1.0f);

            auto light = lightManager->AddLight(lightPos, lightColor, m_lightIntensity, m_lightAngle);

            auto mesh = std::make_shared<Sphere>(0.2f, 16, 8, glm::vec3(1.0f, 1.0f, 0.0f));
            auto name = "LightIndicator_" + light->GetName();
            auto lightNode = objectManager->CreateNode(name, mesh);
            lightNode->SetPosition(lightPos);
        }

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
                ImGui::Text("  角度: %.1f", light->GetAngle());


                light->SetIntensity(m_lightIntensity);
                light->SetAngle(m_lightAngle);

                ImGui::PushID(static_cast<int>(i));
                if (ImGui::SmallButton("删除")) {
                    std::string indicatorName = "LightIndicator_" + light->GetName();
                    auto indicatorNode = objectManager->FindNode(indicatorName);
                    if (indicatorNode) {
                        objectManager->RemoveNode(indicatorNode);
                    }
                    lightManager->RemoveLight(light);
                }
                ImGui::PopID();

                if (i + 1 < lights.size()) {
                    ImGui::Spacing();
                }
            }
        }

        ImGui::Unindent();
    }

    ImGui::Spacing();
    ImGui::Separator();

    // 选中物体信息
    auto selectedNode = selectionSystem->GetSelectedNode();
    if (selectedNode) {
        ImGui::Spacing();
        ImGui::Text("选中物体:");
        ImGui::Text("%s", selectedNode->GetName().c_str());

        glm::vec3 pos = selectedNode->GetPosition();
        ImGui::Text("位置: (%.2f, %.2f, %.2f)", pos.x, pos.y, pos.z);

        glm::vec3 rot = selectedNode->GetRotation();
        ImGui::Text("旋转: (%.2f, %.2f, %.2f)", rot.x, rot.y, rot.z);

        glm::vec3 scale = selectedNode->GetScale();
        ImGui::Text("缩放: (%.2f, %.2f, %.2f)", scale.x, scale.y, scale.z);
    } else {
        ImGui::Spacing();
        ImGui::Text("未选中物体");
    }

    ImGui::End();
}

} // namespace SoulsEngine

