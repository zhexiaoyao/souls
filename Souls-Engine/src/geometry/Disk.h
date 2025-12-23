#pragma once

#include "Mesh.h"
#include <glm/glm.hpp>

namespace SoulsEngine {

class Disk : public Mesh {
public:
    Disk(float radius = 1.0f, int sectors = 36, const glm::vec3& color = glm::vec3(1.0f, 1.0f, 1.0f));
    virtual ~Disk() = default;
};

} // namespace SoulsEngine

