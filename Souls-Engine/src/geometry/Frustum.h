#pragma once

#include "Mesh.h"
#include <glm/glm.hpp>

namespace SoulsEngine {

class Frustum : public Mesh {
public:
    Frustum(int sides = 6, float topRadius = 0.5f, float bottomRadius = 1.0f, float height = 2.0f, const glm::vec3& color = glm::vec3(1.0f, 1.0f, 1.0f));
    virtual ~Frustum() = default;
};

} // namespace SoulsEngine

