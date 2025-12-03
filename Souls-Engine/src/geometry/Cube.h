#pragma once

#include "Mesh.h"
#include <glm/glm.hpp>

namespace SoulsEngine {

class Cube : public Mesh {
public:
    Cube(float size = 1.0f, const glm::vec3& color = glm::vec3(1.0f, 1.0f, 1.0f));
    virtual ~Cube() = default;
};

} // namespace SoulsEngine

