#include "cube.h"

void Cube::updateParams(int param1) {
    m_vertexData = std::vector<float>();
    m_param1 = param1;
    setVertexData();
}

void Cube::makeTile(glm::vec3 topLeft,
                    glm::vec3 topRight,
                    glm::vec3 bottomLeft,
                    glm::vec3 bottomRight) {

    glm::vec3 TL = glm::normalize(glm::cross(bottomLeft-topLeft, bottomRight - topLeft));
    glm::vec3 BL = glm::normalize(glm::cross(bottomRight - bottomLeft, topLeft - bottomLeft));
    glm::vec3 BR = glm::normalize(glm::cross(topLeft - bottomRight, bottomLeft - bottomRight));

    glm::vec3 BR1 = glm::normalize(glm::cross(topRight - bottomRight, topLeft - bottomRight));
    glm::vec3 TR1 = glm::normalize(glm::cross(topLeft - topRight, bottomRight - topRight));
    glm::vec3 TL1 = glm::normalize(glm::cross(bottomRight - topLeft, topRight - topLeft));

    insertVec3(m_vertexData, topLeft); insertVec3(m_vertexData, TL);
    insertVec3(m_vertexData, bottomLeft); insertVec3(m_vertexData, BL);
    insertVec3(m_vertexData, bottomRight); insertVec3(m_vertexData, BR);
    insertVec3(m_vertexData, bottomRight); insertVec3(m_vertexData, BR1);
    insertVec3(m_vertexData, topRight); insertVec3(m_vertexData, TR1);
    insertVec3(m_vertexData, topLeft); insertVec3(m_vertexData, TL1);
}

void Cube::makeFace(glm::vec3 topLeft,
                    glm::vec3 topRight,
                    glm::vec3 bottomLeft,
                    glm::vec3 bottomRight) {

    glm::vec3 dx = topRight - topLeft;
    glm::vec3 dy = -topRight + bottomRight;
    glm::vec3 newdx = dx / (1.0f * m_param1);
    glm::vec3 newdy = dy / (1.0f * m_param1);
    for (int i = 0; i < m_param1; i++) {
        for (int j = 0; j < m_param1; j++) {
            // the
            glm::vec3 newTL = topLeft + dx * (1.0f * i / m_param1) + dy * (1.0f * j / m_param1);
            makeTile(newTL, newTL + newdx, newTL + newdy, newTL + newdx + newdy);
        }
    }
}

void Cube::setVertexData() {

    makeFace(glm::vec3(-0.5f,  0.5f, 0.5f),
             glm::vec3( 0.5f,  0.5f, 0.5f),
             glm::vec3(-0.5f, -0.5f, 0.5f),
             glm::vec3( 0.5f, -0.5f, 0.5f));

    makeFace(glm::vec3( 0.5f,  0.5f, 0.5f),
             glm::vec3( 0.5f,  0.5f,-0.5f),
             glm::vec3( 0.5f, -0.5f, 0.5f),
             glm::vec3( 0.5f, -0.5f,-0.5f));

    makeFace(glm::vec3(-0.5f, -0.5f, 0.5f),
             glm::vec3( 0.5f, -0.5f, 0.5f),
             glm::vec3(-0.5f, -0.5f,-0.5f),
             glm::vec3( 0.5f, -0.5f,-0.5f));

    makeFace(glm::vec3(-0.5f, -0.5f,-0.5f),
             glm::vec3( 0.5f, -0.5f,-0.5f),
             glm::vec3(-0.5f,  0.5f,-0.5f),
             glm::vec3( 0.5f,  0.5f,-0.5f));

    makeFace(glm::vec3(-0.5f,  0.5f,-0.5f),
             glm::vec3(-0.5f,  0.5f, 0.5f),
             glm::vec3(-0.5f, -0.5f,-0.5f),
             glm::vec3(-0.5f, -0.5f, 0.5f));

    makeFace(glm::vec3( 0.5f, 0.5f, 0.5f),
             glm::vec3(-0.5f, 0.5f, 0.5f),
             glm::vec3( 0.5f, 0.5f,-0.5f),
             glm::vec3(-0.5f, 0.5f,-0.5f));


}

void Cube::insertVec3(std::vector<float> &data, glm::vec3 v) {
    data.push_back(v.x);
    data.push_back(v.y);
    data.push_back(v.z);
}
