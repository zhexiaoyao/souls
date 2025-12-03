#pragma once

#include "Mesh.h"
#include <glm/glm.hpp>

namespace SoulsEngine {

class Cone : public Mesh {
public:
    Cone(float radius = 1.0f, float height = 2.0f, int sectors = 36, const glm::vec3& color = glm::vec3(1.0f, 1.0f, 1.0f));
    virtual ~Cone() = default;
};

} // namespace SoulsEngine

