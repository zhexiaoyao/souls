#include "Disk.h"
#include <vector>
#include <cmath>

namespace SoulsEngine {

Disk::Disk(float radius, int sectors, const glm::vec3& color) {
    std::vector<float> vertices;
    float sectorStep = 2.0f * 3.14159265359f / sectors;

    // 创建圆盘（在XY平面上，法线指向Z轴正方向）
    for (int i = 0; i < sectors; ++i) {
        float angle1 = i * sectorStep;
        float angle2 = (i + 1) * sectorStep;

        float x1 = radius * cosf(angle1);
        float y1 = radius * sinf(angle1);
        float x2 = radius * cosf(angle2);
        float y2 = radius * sinf(angle2);

        // 每个三角形：中心点 + 两个边缘点
        vertices.insert(vertices.end(), { 0.0f, 0.0f, 0.0f, color.r, color.g, color.b });
        vertices.insert(vertices.end(), { x1, y1, 0.0f, color.r, color.g, color.b });
        vertices.insert(vertices.end(), { x2, y2, 0.0f, color.r, color.g, color.b });
    }

    SetupMesh(vertices);
}

} // namespace SoulsEngine

