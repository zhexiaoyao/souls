#pragma once

#include <GLFW/glfw3.h>
#include "Material.h"
#include <memory>
#include <vector>

// 鍓嶅悜澹版槑
struct ImGuiContext;

namespace SoulsEngine {

// 鍓嶅悜澹版槑
class Camera;
class ObjectManager;
class SelectionSystem;
class Material;
class LightManager;

// ImGui绯荤粺绠＄悊绫�
class ImGuiSystem {
public:
    ImGuiSystem();
    ~ImGuiSystem();

    // 绂佹鎷疯礉
    ImGuiSystem(const ImGuiSystem&) = delete;
    ImGuiSystem& operator=(const ImGuiSystem&) = delete;

    // 鍒濆鍖朓mGui
    bool Initialize(GLFWwindow* window);

    // 娓呯悊ImGui
    void Shutdown();

    // 寮€濮嬫柊甯�?
    void BeginFrame();

    // 缁撴潫甯у苟娓叉煋
    void EndFrame();

    // 娓叉煋渚ф爮UI
    void RenderSidebar(ObjectManager* objectManager, SelectionSystem* selectionSystem, 
                       Camera* camera, LightManager* lightManager, float aspectRatio);

private:
    GLFWwindow* m_window;
    ImGuiContext* m_context;

    // UI鐘舵€�
    bool m_showGeometryMenu;
    bool m_showMaterialMenu;
    bool m_showLightMenu;
    bool m_showModelMenu;

    // 鐏厜鍙傛暟
    float m_lightAngle;      // 鍏夌収寮犺锛�0-360搴︼�?
    float m_lightIntensity;  // 鍏夌収寮哄害锛�0-10锛�

    // 杈呭姪鏂规硶
    void RenderGeometryMenu(ObjectManager* objectManager, SelectionSystem* selectionSystem, Camera* camera);
    void RenderMaterialMenu(SelectionSystem* selectionSystem);
    void RenderLightMenu();
    void RenderModelMenu();

    // 生成预设材质列表
    void InitMaterialPresets();

    std::vector<Material> m_materialPresets;
};

} // namespace SoulsEngine

