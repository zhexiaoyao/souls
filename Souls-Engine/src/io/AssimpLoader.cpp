#include "AssimpLoader.h"
#include <memory>
#include <vector>
#include <string>
#include <glm/glm.hpp>

#ifdef USE_ASSIMP
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#endif

namespace SoulsEngine {

std::unique_ptr<Mesh> LoadModelWithAssimp(const std::string& path) {
#ifdef USE_ASSIMP
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_JoinIdenticalVertices);
    if (!scene || !scene->HasMeshes()) return nullptr;

    const aiMesh* am = scene->mMeshes[0];
    std::vector<float> temp;
    temp.reserve(am->mNumVertices * 6);

    for (unsigned i = 0; i < am->mNumVertices; ++i) {
        glm::vec3 pos(0.0f);
        if (am->HasPositions()) {
            pos = glm::vec3(am->mVertices[i].x, am->mVertices[i].y, am->mVertices[i].z);
        }
        glm::vec3 col(0.8f);
        if (am->HasNormals()) {
            glm::vec3 n(am->mNormals[i].x, am->mNormals[i].y, am->mNormals[i].z);
            col = glm::vec3(n.x * 0.5f + 0.5f, n.y * 0.5f + 0.5f, n.z * 0.5f + 0.5f);
        }
        temp.push_back(pos.x);
        temp.push_back(pos.y);
        temp.push_back(pos.z);
        temp.push_back(col.r);
        temp.push_back(col.g);
        temp.push_back(col.b);
    }

    std::vector<float> finalVertices;
    for (unsigned f = 0; f < am->mNumFaces; ++f) {
        const aiFace& face = am->mFaces[f];
        if (face.mNumIndices != 3) continue;
        for (unsigned k = 0; k < 3; ++k) {
            unsigned idx = face.mIndices[k];
            size_t base = static_cast<size_t>(idx) * 6;
            finalVertices.insert(finalVertices.end(), temp.begin() + base, temp.begin() + base + 6);
        }
    }

    auto mesh = std::make_unique<Mesh>();
    mesh->InitFromFloatVertices(finalVertices);
    return mesh;
#else
    (void)path;
    return nullptr;
#endif
}

} // namespace SoulsEngine







