#pragma once

#include "Mesh.h"
#include <glm/glm.hpp>

namespace SoulsEngine {

class Sphere : public Mesh {
public:
    Sphere(float radius = 1.0f, int sectors = 36, int stacks = 18, const glm::vec3& color = glm::vec3(1.0f, 1.0f, 1.0f));
    virtual ~Sphere() = default;
};

} // namespace SoulsEngine

