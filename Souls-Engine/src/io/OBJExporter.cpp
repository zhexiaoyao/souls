#include "OBJExporter.h"
#include "geometry/Mesh.h"
#include "geometry/Cube.h"
#include "geometry/Cylinder.h"
#include "geometry/Sphere.h"
#include "core/SceneNode.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <cmath>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

namespace SoulsEngine {

bool OBJExporter::ExportMesh(const Mesh& mesh, const std::string& filepath, const glm::mat4& transform) {
    // 注意：当前Mesh类没有提供直接访问顶点数据的方法
    // 这是一个占位实现，实际需要从Mesh中提取顶点数据
    // 由于Mesh使用OpenGL缓冲区，我们需要在创建时保存顶点数据
    // 这里提供一个基础框架
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Failed to open file for writing: " << filepath << std::endl;
        return false;
    }
    
    file << "# Exported from Souls Engine\n";
    file << "# Mesh export\n\n";
    
    // 注意：实际实现需要从Mesh中提取顶点数据
    // 当前Mesh类使用OpenGL缓冲区，无法直接读取
    // 需要在Mesh类中添加GetVertices()方法，或者保存原始顶点数据
    
    file.close();
    return true;
}

bool OBJExporter::ExportSceneNode(const std::shared_ptr<SceneNode>& node, const std::string& filepath) {
    if (!node) {
        std::cerr << "Invalid node for export" << std::endl;
        return false;
    }
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Failed to open file for writing: " << filepath << std::endl;
        return false;
    }
    
    file << "# Exported from Souls Engine\n";
    file << "# Scene Node: " << node->GetName() << "\n\n";
    
    // 获取节点的世界变换矩阵
    glm::mat4 worldTransform = node->GetWorldTransform();
    
    // 注意：实际实现需要从Mesh中提取顶点数据
    // 当前实现是占位符
    
    file.close();
    return true;
}

bool OBJExporter::ExportWeaponModel(const std::shared_ptr<SceneNode>& weaponNode, const std::string& filepath) {
    if (!weaponNode) {
        std::cerr << "Invalid weapon node for export" << std::endl;
        return false;
    }
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Failed to open file for writing: " << filepath << std::endl;
        return false;
    }
    
    file << "# Exported Weapon Model from Souls Engine\n";
    file << "# Weapon: " << weaponNode->GetName() << "\n\n";
    
    // 遍历武器节点及其所有子节点
    // 注意：实际实现需要从Mesh中提取顶点数据
    
    file.close();
    std::cout << "Weapon model exported to: " << filepath << std::endl;
    return true;
}

} // namespace SoulsEngine

