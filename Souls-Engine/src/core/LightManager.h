#pragma once

#include "Light.h"
#include <vector>
#include <memory>

namespace SoulsEngine {

// 光源管理器
class LightManager {
public:
    LightManager();
    ~LightManager() = default;

    // 禁止拷贝
    LightManager(const LightManager&) = delete;
    LightManager& operator=(const LightManager&) = delete;

    // 添加光源
    std::shared_ptr<Light> AddLight(const glm::vec3& position, const glm::vec3& color, float intensity, float angle);
    
    // 移除光源
    void RemoveLight(std::shared_ptr<Light> light);
    
    // 获取所有光源
    const std::vector<std::shared_ptr<Light>>& GetLights() const { return m_lights; }
    
    // 清空所有光源
    void Clear();
    
    // 获取第一个光源（用于简化版光照）
    std::shared_ptr<Light> GetFirstLight() const {
        return m_lights.empty() ? nullptr : m_lights[0];
    }
    
    // 根据名称查找光源
    std::shared_ptr<Light> FindLightByName(const std::string& name) const;

private:
    std::vector<std::shared_ptr<Light>> m_lights;
    int m_lightCounter;
};

} // namespace SoulsEngine

