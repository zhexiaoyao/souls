#include "Cone.h"
#include <vector>
#include <cmath>

namespace SoulsEngine {

Cone::Cone(float radius, float height, int sectors, const glm::vec3& color) {
    std::vector<float> vertices;
    float h = height * 0.5f;
    float sectorStep = 2.0f * 3.14159265359f / sectors;

    // 侧面
    for (int i = 0; i < sectors; ++i) {
        float angle1 = i * sectorStep;
        float angle2 = (i + 1) * sectorStep;

        float x1 = radius * cosf(angle1);
        float z1 = radius * sinf(angle1);
        float x2 = radius * cosf(angle2);
        float z2 = radius * sinf(angle2);

        // 三角形（从顶点到底边）
        vertices.insert(vertices.end(), { 0.0f,  h, 0.0f, color.r, color.g, color.b });
        vertices.insert(vertices.end(), { x1, -h, z1, color.r, color.g, color.b });
        vertices.insert(vertices.end(), { x2, -h, z2, color.r, color.g, color.b });
    }

    // 底面
    for (int i = 0; i < sectors; ++i) {
        float angle1 = i * sectorStep;
        float angle2 = (i + 1) * sectorStep;

        float x1 = radius * cosf(angle1);
        float z1 = radius * sinf(angle1);
        float x2 = radius * cosf(angle2);
        float z2 = radius * sinf(angle2);

        vertices.insert(vertices.end(), { 0.0f, -h, 0.0f, color.r, color.g, color.b });
        vertices.insert(vertices.end(), { x2, -h, z2, color.r, color.g, color.b });
        vertices.insert(vertices.end(), { x1, -h, z1, color.r, color.g, color.b });
    }

    SetupMesh(vertices);
}

}

