#include "Sphere.h"
#include <vector>
#include <cmath>

namespace SoulsEngine {

Sphere::Sphere(float radius, int sectors, int stacks, const glm::vec3& color) {
    std::vector<float> vertices;
    float sectorStep = 2.0f * 3.14159265359f / sectors;
    float stackStep = 3.14159265359f / stacks;

    // 直接生成三角形
    for (int i = 0; i < stacks; ++i) {
        float stackAngle1 = 3.14159265359f / 2.0f - i * stackStep;
        float stackAngle2 = 3.14159265359f / 2.0f - (i + 1) * stackStep;

        float xy1 = radius * cosf(stackAngle1);
        float z1 = radius * sinf(stackAngle1);
        float xy2 = radius * cosf(stackAngle2);
        float z2 = radius * sinf(stackAngle2);

        for (int j = 0; j < sectors; ++j) {
            float sectorAngle1 = j * sectorStep;
            float sectorAngle2 = (j + 1) * sectorStep;

            float x1 = xy1 * cosf(sectorAngle1);
            float y1 = xy1 * sinf(sectorAngle1);
            float x2 = xy1 * cosf(sectorAngle2);
            float y2 = xy1 * sinf(sectorAngle2);
            float x3 = xy2 * cosf(sectorAngle1);
            float y3 = xy2 * sinf(sectorAngle1);
            float x4 = xy2 * cosf(sectorAngle2);
            float y4 = xy2 * sinf(sectorAngle2);

            if (i != 0) {
                // 第一个三角形
                vertices.insert(vertices.end(), { x1, y1, z1, color.r, color.g, color.b });
                vertices.insert(vertices.end(), { x3, y3, z2, color.r, color.g, color.b });
                vertices.insert(vertices.end(), { x2, y2, z1, color.r, color.g, color.b });
            }

            if (i != (stacks - 1)) {
                // 第二个三角形
                vertices.insert(vertices.end(), { x2, y2, z1, color.r, color.g, color.b });
                vertices.insert(vertices.end(), { x3, y3, z2, color.r, color.g, color.b });
                vertices.insert(vertices.end(), { x4, y4, z2, color.r, color.g, color.b });
            }
        }
    }

    SetupMesh(vertices);
}

}

