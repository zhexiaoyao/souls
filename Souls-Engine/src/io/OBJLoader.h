#pragma once

#include <string>
#include <memory>
#include <vector>
#include "geometry/Mesh.h"

namespace SoulsEngine {

// 从 OBJ 文件创建一个 Mesh（使用当前引擎顶点格式：position(3) + color(3)）
// 返回 nullptr 表示导入失败
std::unique_ptr<Mesh> LoadOBJFromFile(const std::string& path);

} // namespace SoulsEngine


