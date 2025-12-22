#pragma once

#include <GLFW/glfw3.h>
#include "Material.h"
#include <memory>
#include <vector>

// ??????
struct ImGuiContext;

namespace SoulsEngine {

// ??????
class Camera;
class ObjectManager;
class SelectionSystem;
class Material;
class LightManager;

// ImGui????????
class ImGuiSystem {
public:
    ImGuiSystem();
    ~ImGuiSystem();

    // ??????
    ImGuiSystem(const ImGuiSystem&) = delete;
    ImGuiSystem& operator=(const ImGuiSystem&) = delete;

    // ?????mGui
    bool Initialize(GLFWwindow* window);

    // ???ImGui
    void Shutdown();

    // ????????
    void BeginFrame();

    // ?????????
    void EndFrame();

    // ??????UI
    void RenderSidebar(ObjectManager* objectManager, SelectionSystem* selectionSystem, 
                       Camera* camera, LightManager* lightManager, float aspectRatio);

private:
    GLFWwindow* m_window;
    ImGuiContext* m_context;

    // UI????
    bool m_showGeometryMenu;
    bool m_showMaterialMenu;
    bool m_showLightMenu;
    bool m_showModelMenu;
    // 已导入模型名称列表（用于左侧模块显示）
    std::vector<std::string> m_importedModelNames;
    // 左侧工具栏宽度（可调整）
    float m_toolbarWidth;

    // ??????
    float m_lightAngle;      // ????????0-360????
    float m_lightIntensity;  // ????????0-10??

    // ??????
    void RenderGeometryMenu(ObjectManager* objectManager, SelectionSystem* selectionSystem, Camera* camera);
    void RenderMaterialMenu(SelectionSystem* selectionSystem);
    void RenderLightMenu();
    void RenderModelMenu();

    // ????????
    void InitMaterialPresets();

    std::vector<Material> m_materialPresets;
};

} // namespace SoulsEngine

