#pragma once

#include <GLFW/glfw3.h>
#include <memory>

// 前向声明
struct ImGuiContext;

namespace SoulsEngine {

// 前向声明
class Camera;
class ObjectManager;
class SelectionSystem;
class Material;
class LightManager;

// ImGui系统管理类
class ImGuiSystem {
public:
    ImGuiSystem();
    ~ImGuiSystem();

    // 禁止拷贝
    ImGuiSystem(const ImGuiSystem&) = delete;
    ImGuiSystem& operator=(const ImGuiSystem&) = delete;

    // 初始化ImGui
    bool Initialize(GLFWwindow* window);

    // 清理ImGui
    void Shutdown();

    // 开始新帧
    void BeginFrame();

    // 结束帧并渲染
    void EndFrame();

    // 渲染侧栏UI
    void RenderSidebar(ObjectManager* objectManager, SelectionSystem* selectionSystem, 
                       Camera* camera, LightManager* lightManager, float aspectRatio);

private:
    GLFWwindow* m_window;
    ImGuiContext* m_context;

    // UI状态
    bool m_showGeometryMenu;
    bool m_showMaterialMenu;
    bool m_showLightMenu;
    bool m_showModelMenu;

    // 灯光参数
    float m_lightAngle;      // 光照张角（0-360度）
    float m_lightIntensity;  // 光照强度（0-10）

    // 辅助方法
    void RenderGeometryMenu(ObjectManager* objectManager, SelectionSystem* selectionSystem, Camera* camera);
    void RenderMaterialMenu(SelectionSystem* selectionSystem);
    void RenderLightMenu();
    void RenderModelMenu();
};

} // namespace SoulsEngine

