#include "Frustum.h"
#include <vector>
#include <cmath>

namespace SoulsEngine {

Frustum::Frustum(int sides, float topRadius, float bottomRadius, float height, const glm::vec3& color) {
    std::vector<float> vertices;
    float h = height * 0.5f;
    float angleStep = 2.0f * 3.14159265359f / sides;

    // 侧面
    for (int i = 0; i < sides; ++i) {
        float angle1 = i * angleStep;
        float angle2 = (i + 1) * angleStep;

        float x1t = topRadius * cosf(angle1);
        float z1t = topRadius * sinf(angle1);
        float x2t = topRadius * cosf(angle2);
        float z2t = topRadius * sinf(angle2);

        float x1b = bottomRadius * cosf(angle1);
        float z1b = bottomRadius * sinf(angle1);
        float x2b = bottomRadius * cosf(angle2);
        float z2b = bottomRadius * sinf(angle2);

        // 第一个三角形
        vertices.insert(vertices.end(), { x1b, -h, z1b, color.r, color.g, color.b });
        vertices.insert(vertices.end(), { x2b, -h, z2b, color.r, color.g, color.b });
        vertices.insert(vertices.end(), { x1t,  h, z1t, color.r, color.g, color.b });

        // 第二个三角形
        vertices.insert(vertices.end(), { x2b, -h, z2b, color.r, color.g, color.b });
        vertices.insert(vertices.end(), { x2t,  h, z2t, color.r, color.g, color.b });
        vertices.insert(vertices.end(), { x1t,  h, z1t, color.r, color.g, color.b });
    }

    // 顶面
    for (int i = 0; i < sides; ++i) {
        float angle1 = i * angleStep;
        float angle2 = (i + 1) * angleStep;

        float x1 = topRadius * cosf(angle1);
        float z1 = topRadius * sinf(angle1);
        float x2 = topRadius * cosf(angle2);
        float z2 = topRadius * sinf(angle2);

        vertices.insert(vertices.end(), { 0.0f,  h, 0.0f, color.r, color.g, color.b });
        vertices.insert(vertices.end(), { x1,  h, z1, color.r, color.g, color.b });
        vertices.insert(vertices.end(), { x2,  h, z2, color.r, color.g, color.b });
    }

    // 底面
    for (int i = 0; i < sides; ++i) {
        float angle1 = i * angleStep;
        float angle2 = (i + 1) * angleStep;

        float x1 = bottomRadius * cosf(angle1);
        float z1 = bottomRadius * sinf(angle1);
        float x2 = bottomRadius * cosf(angle2);
        float z2 = bottomRadius * sinf(angle2);

        vertices.insert(vertices.end(), { 0.0f, -h, 0.0f, color.r, color.g, color.b });
        vertices.insert(vertices.end(), { x2, -h, z2, color.r, color.g, color.b });
        vertices.insert(vertices.end(), { x1, -h, z1, color.r, color.g, color.b });
    }

    SetupMesh(vertices);
}

}

