#include "GridPlane.h"
#include <vector>

namespace SoulsEngine {

GridPlane::GridPlane(float size, int tiles) {
    if (tiles <= 0) tiles = 1;
    float half = size * 0.5f;
    float tileSize = size / static_cast<float>(tiles);

    std::vector<float> vertices;
    vertices.reserve(static_cast<size_t>(tiles) * tiles * 6 * 6); // 6 verts per quad, 6 floats per vert

    for (int rz = 0; rz < tiles; ++rz) {
        for (int rx = 0; rx < tiles; ++rx) {
            float x0 = -half + rx * tileSize;
            float x1 = x0 + tileSize;
            float z0 = -half + rz * tileSize;
            float z1 = z0 + tileSize;

            bool white = ((rx + rz) % 2 == 0);
            glm::vec3 color = white ? glm::vec3(1.0f, 1.0f, 1.0f) : glm::vec3(0.0f, 0.0f, 0.0f);

            // Triangle 1: (x0,0,z0) - (x1,0,z0) - (x1,0,z1)
            vertices.insert(vertices.end(), { x0, 0.0f, z0, color.r, color.g, color.b });
            vertices.insert(vertices.end(), { x1, 0.0f, z0, color.r, color.g, color.b });
            vertices.insert(vertices.end(), { x1, 0.0f, z1, color.r, color.g, color.b });

            // Triangle 2: (x0,0,z0) - (x1,0,z1) - (x0,0,z1)
            vertices.insert(vertices.end(), { x0, 0.0f, z0, color.r, color.g, color.b });
            vertices.insert(vertices.end(), { x1, 0.0f, z1, color.r, color.g, color.b });
            vertices.insert(vertices.end(), { x0, 0.0f, z1, color.r, color.g, color.b });
        }
    }

    SetupMesh(vertices);
}

} // namespace SoulsEngine







