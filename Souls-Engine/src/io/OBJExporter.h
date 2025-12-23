#pragma once

#include <string>
#include <memory>
#include "geometry/Mesh.h"
#include "core/SceneNode.h"
#include <glm/glm.hpp>

namespace SoulsEngine {

// OBJ导出器 - 将模型导出为OBJ文件格式
class OBJExporter {
public:
    // 导出单个Mesh为OBJ文件
    static bool ExportMesh(const Mesh& mesh, const std::string& filepath, const glm::mat4& transform = glm::mat4(1.0f));
    
    // 导出SceneNode及其所有子节点为OBJ文件
    static bool ExportSceneNode(const std::shared_ptr<SceneNode>& node, const std::string& filepath);
    
    // 导出武器模型（包含所有部件）
    static bool ExportWeaponModel(const std::shared_ptr<SceneNode>& weaponNode, const std::string& filepath);
};

} // namespace SoulsEngine


