#include "OBJLoader.h"
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <memory>
#include <cctype>
#include <glm/glm.hpp>

namespace SoulsEngine {

static inline std::string Trim(const std::string& s) {
    size_t a = 0;
    while (a < s.size() && std::isspace(static_cast<unsigned char>(s[a]))) ++a;
    size_t b = s.size();
    while (b > a && std::isspace(static_cast<unsigned char>(s[b-1]))) --b;
    return s.substr(a, b - a);
}

std::unique_ptr<Mesh> LoadOBJFromFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) return nullptr;

    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;
    std::vector<float> outVertices; // flattened position(3) + color(3)

    std::string line;
    while (std::getline(file, line)) {
        line = Trim(line);
        if (line.empty() || line[0] == '#') continue;

        std::istringstream iss(line);
        std::string prefix;
        iss >> prefix;
        if (prefix == "v") {
            float x, y, z;
            iss >> x >> y >> z;
            positions.emplace_back(x, y, z);
        } else if (prefix == "vn") {
            float nx, ny, nz;
            iss >> nx >> ny >> nz;
            normals.emplace_back(nx, ny, nz);
        } else if (prefix == "f") {
            // face may have more than 3 vertices (polygon) -> triangulate as fan
            std::vector<std::string> tokens;
            std::string tok;
            while (iss >> tok) tokens.push_back(tok);
            if (tokens.size() < 3) continue;

            auto parseIndex = [](const std::string& t, int& pIdx, int& nIdx) {
                pIdx = nIdx = -1;
                // formats: v, v/vt, v//vn, v/vt/vn
                size_t p1 = t.find('/');
                if (p1 == std::string::npos) {
                    pIdx = std::stoi(t);
                    return;
                }
                std::string s1 = t.substr(0, p1);
                pIdx = s1.empty() ? 0 : std::stoi(s1);
                size_t p2 = t.find('/', p1 + 1);
                if (p2 == std::string::npos) {
                    // v/vt
                    return;
                }
                std::string s3 = t.substr(p2 + 1);
                if (!s3.empty()) nIdx = std::stoi(s3);
            };

            // fan triangulation
            for (size_t i = 1; i + 1 < tokens.size(); ++i) {
                std::string tri[3] = { tokens[0], tokens[i], tokens[i+1] };
                for (int vi = 0; vi < 3; ++vi) {
                    int pIdx = -1, nIdx = -1;
                    parseIndex(tri[vi], pIdx, nIdx);
                    // OBJ indices are 1-based
                    if (pIdx <= 0 || static_cast<size_t>(pIdx) > positions.size()) {
                        // invalid index, skip triangle
                        pIdx = -1;
                    }
                    glm::vec3 pos(0.0f);
                    if (pIdx > 0) pos = positions[static_cast<size_t>(pIdx) - 1];

                    glm::vec3 col(0.8f, 0.8f, 0.8f); // default color
                    if (nIdx > 0 && static_cast<size_t>(nIdx) <= normals.size()) {
                        glm::vec3 n = normals[static_cast<size_t>(nIdx) - 1];
                        // map normal [-1,1] to color [0,1]
                        col = glm::vec3(n.x * 0.5f + 0.5f, n.y * 0.5f + 0.5f, n.z * 0.5f + 0.5f);
                    }

                    outVertices.push_back(pos.x);
                    outVertices.push_back(pos.y);
                    outVertices.push_back(pos.z);
                    outVertices.push_back(col.r);
                    outVertices.push_back(col.g);
                    outVertices.push_back(col.b);
                }
            }
        }
    }

    if (outVertices.empty()) return nullptr;

    auto mesh = std::make_unique<Mesh>();
    mesh->InitFromFloatVertices(outVertices);
    return mesh;
}

} // namespace SoulsEngine

