#include "Light.h"

namespace SoulsEngine {

Light::Light()
    : m_name("Light")
    , m_position(0.0f, 5.0f, 0.0f)
    , m_color(1.0f, 1.0f, 1.0f)
    , m_intensity(1.0f)
    , m_angle(45.0f)
{
}

Light::Light(const glm::vec3& position, const glm::vec3& color, float intensity, float angle)
    : m_name("Light")
    , m_position(position)
    , m_color(color)
    , m_intensity(intensity)
    , m_angle(angle)
{
}

} // namespace SoulsEngine










