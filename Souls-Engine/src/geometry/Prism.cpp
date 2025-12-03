#include "Prism.h"
#include <vector>
#include <cmath>

namespace SoulsEngine {

Prism::Prism(int sides, float radius, float height, const glm::vec3& color) {
    std::vector<float> vertices;
    float h = height * 0.5f;
    float angleStep = 2.0f * 3.14159265359f / sides;

    // 侧面
    for (int i = 0; i < sides; ++i) {
        float angle1 = i * angleStep;
        float angle2 = (i + 1) * angleStep;

        float x1 = radius * cosf(angle1);
        float z1 = radius * sinf(angle1);
        float x2 = radius * cosf(angle2);
        float z2 = radius * sinf(angle2);

        // 第一个三角形
        vertices.insert(vertices.end(), { x1, -h, z1, color.r, color.g, color.b });
        vertices.insert(vertices.end(), { x2, -h, z2, color.r, color.g, color.b });
        vertices.insert(vertices.end(), { x1,  h, z1, color.r, color.g, color.b });

        // 第二个三角形
        vertices.insert(vertices.end(), { x2, -h, z2, color.r, color.g, color.b });
        vertices.insert(vertices.end(), { x2,  h, z2, color.r, color.g, color.b });
        vertices.insert(vertices.end(), { x1,  h, z1, color.r, color.g, color.b });
    }

    // 顶面
    for (int i = 0; i < sides; ++i) {
        float angle1 = i * angleStep;
        float angle2 = (i + 1) * angleStep;

        float x1 = radius * cosf(angle1);
        float z1 = radius * sinf(angle1);
        float x2 = radius * cosf(angle2);
        float z2 = radius * sinf(angle2);

        vertices.insert(vertices.end(), { 0.0f,  h, 0.0f, color.r, color.g, color.b });
        vertices.insert(vertices.end(), { x1,  h, z1, color.r, color.g, color.b });
        vertices.insert(vertices.end(), { x2,  h, z2, color.r, color.g, color.b });
    }

    // 底面
    for (int i = 0; i < sides; ++i) {
        float angle1 = i * angleStep;
        float angle2 = (i + 1) * angleStep;

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

