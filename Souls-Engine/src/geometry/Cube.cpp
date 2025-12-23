#include "Cube.h"
#include <vector>
#include <cmath>

namespace SoulsEngine {

Cube::Cube(float size, const glm::vec3& color) {
    float s = size * 0.5f; // 半边长
    std::vector<float> vertices;

    // 前面 (z = +s, 法线指向 +z)
    // 第一个三角形：左下 -> 右下 -> 右上
    vertices.insert(vertices.end(), { -s, -s,  s, color.r, color.g, color.b });
    vertices.insert(vertices.end(), {  s, -s,  s, color.r, color.g, color.b });
    vertices.insert(vertices.end(), {  s,  s,  s, color.r, color.g, color.b });
    // 第二个三角形：左下 -> 右上 -> 左上
    vertices.insert(vertices.end(), { -s, -s,  s, color.r, color.g, color.b });
    vertices.insert(vertices.end(), {  s,  s,  s, color.r, color.g, color.b });
    vertices.insert(vertices.end(), { -s,  s,  s, color.r, color.g, color.b });

    // 后面 (z = -s, 法线指向 -z)
    // 第一个三角形：右下 -> 左下 -> 左上
    vertices.insert(vertices.end(), {  s, -s, -s, color.r, color.g, color.b });
    vertices.insert(vertices.end(), { -s, -s, -s, color.r, color.g, color.b });
    vertices.insert(vertices.end(), { -s,  s, -s, color.r, color.g, color.b });
    // 第二个三角形：右下 -> 左上 -> 右上
    vertices.insert(vertices.end(), {  s, -s, -s, color.r, color.g, color.b });
    vertices.insert(vertices.end(), { -s,  s, -s, color.r, color.g, color.b });
    vertices.insert(vertices.end(), {  s,  s, -s, color.r, color.g, color.b });

    // 右面 (x = +s, 法线指向 +x)
    // 第一个三角形：前下 -> 后下 -> 后上
    vertices.insert(vertices.end(), {  s, -s,  s, color.r, color.g, color.b });
    vertices.insert(vertices.end(), {  s, -s, -s, color.r, color.g, color.b });
    vertices.insert(vertices.end(), {  s,  s, -s, color.r, color.g, color.b });
    // 第二个三角形：前下 -> 后上 -> 前上
    vertices.insert(vertices.end(), {  s, -s,  s, color.r, color.g, color.b });
    vertices.insert(vertices.end(), {  s,  s, -s, color.r, color.g, color.b });
    vertices.insert(vertices.end(), {  s,  s,  s, color.r, color.g, color.b });

    // 左面 (x = -s, 法线指向 -x)
    // 第一个三角形：后下 -> 前下 -> 前上
    vertices.insert(vertices.end(), { -s, -s, -s, color.r, color.g, color.b });
    vertices.insert(vertices.end(), { -s, -s,  s, color.r, color.g, color.b });
    vertices.insert(vertices.end(), { -s,  s,  s, color.r, color.g, color.b });
    // 第二个三角形：后下 -> 前上 -> 后上
    vertices.insert(vertices.end(), { -s, -s, -s, color.r, color.g, color.b });
    vertices.insert(vertices.end(), { -s,  s,  s, color.r, color.g, color.b });
    vertices.insert(vertices.end(), { -s,  s, -s, color.r, color.g, color.b });

    // 上面 (y = +s, 法线指向 +y)
    // 第一个三角形：前左 -> 前右 -> 后右
    vertices.insert(vertices.end(), { -s,  s,  s, color.r, color.g, color.b });
    vertices.insert(vertices.end(), {  s,  s,  s, color.r, color.g, color.b });
    vertices.insert(vertices.end(), {  s,  s, -s, color.r, color.g, color.b });
    // 第二个三角形：前左 -> 后右 -> 后左
    vertices.insert(vertices.end(), { -s,  s,  s, color.r, color.g, color.b });
    vertices.insert(vertices.end(), {  s,  s, -s, color.r, color.g, color.b });
    vertices.insert(vertices.end(), { -s,  s, -s, color.r, color.g, color.b });

    // 下面 (y = -s, 法线指向 -y)
    // 第一个三角形：后左 -> 后右 -> 前右
    vertices.insert(vertices.end(), { -s, -s, -s, color.r, color.g, color.b });
    vertices.insert(vertices.end(), {  s, -s, -s, color.r, color.g, color.b });
    vertices.insert(vertices.end(), {  s, -s,  s, color.r, color.g, color.b });
    // 第二个三角形：后左 -> 前右 -> 前左
    vertices.insert(vertices.end(), { -s, -s, -s, color.r, color.g, color.b });
    vertices.insert(vertices.end(), {  s, -s,  s, color.r, color.g, color.b });
    vertices.insert(vertices.end(), { -s, -s,  s, color.r, color.g, color.b });

    SetupMesh(vertices);
}

}

