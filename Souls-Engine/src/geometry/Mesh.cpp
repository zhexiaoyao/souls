#include "Mesh.h"
#include <glad/glad.h>

namespace SoulsEngine {

Mesh::Mesh() : m_VAO(0), m_VBO(0), m_vertexCount(0) {
}

Mesh::~Mesh() {
    if (m_VBO != 0) {
        glDeleteBuffers(1, &m_VBO);
        m_VBO = 0;
    }
    if (m_VAO != 0) {
        glDeleteVertexArrays(1, &m_VAO);
        m_VAO = 0;
    }
}

void Mesh::SetupMesh(const std::vector<float>& vertices) {
    m_vertexCount = vertices.size() / 6; // 濮ｅ繋閲滄い鍓佸仯6娑撶專loat

    // 閸掓稑缂揤AO閸滃BO
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);

    // 缂佹垵鐣綱AO
    glBindVertexArray(m_VAO);

    // 缂佹垵鐣綱BO楠炴湹绗傛导鐘虫殶閹�?
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, 
                 static_cast<GLsizeiptr>(vertices.size() * sizeof(float)), 
                 vertices.data(), 
                 GL_STATIC_DRAW);

    // 鐠佸墽鐤嗘い鍓佸仯鐏炵偞鈧�?
    // 娴ｅ秶鐤嗙仦鐐粹偓? (location = 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    // 妫版粏澹婄仦鐐粹偓? (location = 1)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // 鐟欙絿绮�
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Mesh::Draw() const {
    if (m_VAO != 0 && m_vertexCount > 0) {
        glBindVertexArray(m_VAO);
        glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(m_vertexCount));
        glBindVertexArray(0);
    }
}

void Mesh::DrawWireframe() const {
    // 缁炬寧顢嬮弫鍫熺亯闁俺绻� SceneNode::RenderWireframe 鐎圭偟骞�
    if (m_VAO != 0 && m_vertexCount > 0) {
        // 濞夈劍鍓伴敍姝PolygonMode(GL_LINE) 閸�? OpenGL Core Profile 娑擃厺绗夐崣顖滄暏
        // 閹存垳婊戞担璺ㄦ暏娑撯偓娑擃亞鐣濋崠鏍畱閺傝纭堕敍姘辨纯閹恒儲瑕嗛弻鎾筹綖閸忓懏膩瀵骏绱濇潏瑙勵攱閺佸牊鐏夐柅姘崇箖
        // SceneNode::RenderWireframe 娑擃厾娈戠紓鈺傛杹閸欐ɑ宕查崪瀹籬ader閻ㄥ嫯顩惄鏍杹閼瑰弶娼电€圭偟骞�
        
        // 濞撳弶鐓嬫潏瑙勵攱閿涘牅濞囬悽銊ワ綖閸忓懏膩瀵骏绱濇担鍡涒偓姘崇箖shader閻ㄥ嫯顩惄鏍杹閼瑰弶娼电€圭偟骞囨鎴ｅ鏉堣顢嬮弫鍫熺亯閿�?
        // 鏉堣顢嬮弫鍫熺亯闁俺绻冪粙宥呬簳閺€鎯с亣鐎电钖勯敍鍫濇躬SceneNode娑擃厼鐤勯悳甯礆閸滃矂绮﹂懝鑼额洬閻╂牠顤侀懝鎻掔杽閻�?
        glBindVertexArray(m_VAO);
        glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(m_vertexCount));
        glBindVertexArray(0);
    }
}

} // namespace SoulsEngine

