#include "Cube.h"
#include <vector>
#include <cmath>

namespace SoulsEngine {

Cube::Cube(float size, const glm::vec3& color) {
    float s = size * 0.5f; // 半边长
    std::vector<float> vertices;

    // 定义8个顶点（前面4个，后面4个）
    vertices.insert(vertices.end(), { -s, -s,  s, color.r, color.g, color.b });
    vertices.insert(vertices.end(), {  s, -s,  s, color.r, color.g, color.b });
    vertices.insert(vertices.end(), {  s,  s,  s, color.r, color.g, color.b });
    vertices.insert(vertices.end(), { -s,  s,  s, color.r, color.g, color.b });
    vertices.insert(vertices.end(), { -s, -s, -s, color.r, color.g, color.b });
    vertices.insert(vertices.end(), {  s, -s, -s, color.r, color.g, color.b });
    vertices.insert(vertices.end(), {  s,  s, -s, color.r, color.g, color.b });
    vertices.insert(vertices.end(), { -s,  s, -s, color.r, color.g, color.b });

    // 前面
    vertices.insert(vertices.end(), { -s, -s,  s, color.r, color.g, color.b });
    vertices.insert(vertices.end(), {  s, -s,  s, color.r, color.g, color.b });
    vertices.insert(vertices.end(), {  s,  s,  s, color.r, color.g, color.b });
    vertices.insert(vertices.end(), { -s, -s,  s, color.r, color.g, color.b });
    vertices.insert(vertices.end(), {  s,  s,  s, color.r, color.g, color.b });
    vertices.insert(vertices.end(), { -s,  s,  s, color.r, color.g, color.b });

    // 后面
    vertices.insert(vertices.end(), {  s, -s, -s, color.r, color.g, color.b });
    vertices.insert(vertices.end(), { -s, -s, -s, color.r, color.g, color.b });
    vertices.insert(vertices.end(), { -s,  s, -s, color.r, color.g, color.b });
    vertices.insert(vertices.end(), {  s, -s, -s, color.r, color.g, color.b });
    vertices.insert(vertices.end(), { -s,  s, -s, color.r, color.g, color.b });
    vertices.insert(vertices.end(), {  s,  s, -s, color.r, color.g, color.b });

    // 右面
    vertices.insert(vertices.end(), {  s, -s,  s, color.r, color.g, color.b });
    vertices.insert(vertices.end(), {  s, -s, -s, color.r, color.g, color.b });
    vertices.insert(vertices.end(), {  s,  s, -s, color.r, color.g, color.b });
    vertices.insert(vertices.end(), {  s, -s,  s, color.r, color.g, color.b });
    vertices.insert(vertices.end(), {  s,  s, -s, color.r, color.g, color.b });
    vertices.insert(vertices.end(), {  s,  s,  s, color.r, color.g, color.b });

    // 左面
    vertices.insert(vertices.end(), { -s, -s, -s, color.r, color.g, color.b });
    vertices.insert(vertices.end(), { -s, -s,  s, color.r, color.g, color.b });
    vertices.insert(vertices.end(), { -s,  s,  s, color.r, color.g, color.b });
    vertices.insert(vertices.end(), { -s, -s, -s, color.r, color.g, color.b });
    vertices.insert(vertices.end(), { -s,  s,  s, color.r, color.g, color.b });
    vertices.insert(vertices.end(), { -s,  s, -s, color.r, color.g, color.b });

    // 上面
    vertices.insert(vertices.end(), { -s,  s,  s, color.r, color.g, color.b });
    vertices.insert(vertices.end(), {  s,  s,  s, color.r, color.g, color.b });
    vertices.insert(vertices.end(), {  s,  s, -s, color.r, color.g, color.b });
    vertices.insert(vertices.end(), { -s,  s,  s, color.r, color.g, color.b });
    vertices.insert(vertices.end(), {  s,  s, -s, color.r, color.g, color.b });
    vertices.insert(vertices.end(), { -s,  s, -s, color.r, color.g, color.b });

    // 下面
    vertices.insert(vertices.end(), { -s, -s, -s, color.r, color.g, color.b });
    vertices.insert(vertices.end(), {  s, -s, -s, color.r, color.g, color.b });
    vertices.insert(vertices.end(), {  s, -s,  s, color.r, color.g, color.b });
    vertices.insert(vertices.end(), { -s, -s, -s, color.r, color.g, color.b });
    vertices.insert(vertices.end(), {  s, -s,  s, color.r, color.g, color.b });
    vertices.insert(vertices.end(), { -s, -s,  s, color.r, color.g, color.b });

    SetupMesh(vertices);
}

}

