#pragma once

#include "Mesh.h"
#include <glm/glm.hpp>

namespace SoulsEngine {

class Prism : public Mesh {
public:
    Prism(int sides = 6, float radius = 1.0f, float height = 2.0f, const glm::vec3& color = glm::vec3(1.0f, 1.0f, 1.0f));
    virtual ~Prism() = default;
};

} // namespace SoulsEngine

