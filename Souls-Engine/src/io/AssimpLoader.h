#pragma once

#include <string>
#include <memory>
#include "geometry/Mesh.h"

namespace SoulsEngine {

// 使用 Assimp 加载任意模型并转换为引擎 Mesh（返回 nullptr 表示失败或未启用 Assimp）
std::unique_ptr<Mesh> LoadModelWithAssimp(const std::string& path);

} // namespace SoulsEngine







