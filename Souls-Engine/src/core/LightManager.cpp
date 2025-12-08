#include "LightManager.h"
#include <algorithm>

namespace SoulsEngine {

LightManager::LightManager()
    : m_lightCounter(0)
{
}

std::shared_ptr<Light> LightManager::AddLight(const glm::vec3& position, const glm::vec3& color, float intensity, float angle) {
    auto light = std::make_shared<Light>(position, color, intensity, angle);
    light->SetName("Light_" + std::to_string(m_lightCounter++));
    m_lights.push_back(light);
    return light;
}

void LightManager::RemoveLight(std::shared_ptr<Light> light) {
    if (!light) return;
    m_lights.erase(
        std::remove_if(m_lights.begin(), m_lights.end(),
            [light](const std::shared_ptr<Light>& l) {
                return l == light;
            }),
        m_lights.end()
    );
}

void LightManager::Clear() {
    m_lights.clear();
    m_lightCounter = 0;
}

std::shared_ptr<Light> LightManager::FindLightByName(const std::string& name) const {
    for (auto light : m_lights) {
        if (light->GetName() == name) {
            return light;
        }
    }
    return nullptr;
}

} // namespace SoulsEngine

