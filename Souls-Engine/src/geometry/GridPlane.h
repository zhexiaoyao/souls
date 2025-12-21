#pragma once

#include "Mesh.h"
#include <glm/glm.hpp>

namespace SoulsEngine {

class GridPlane : public Mesh {
public:
    // size: 平面边长，tiles: 每边分割格子数量
    GridPlane(float size = 20.0f, int tiles = 20);
    virtual ~GridPlane() = default;
};

} // namespace SoulsEngine


