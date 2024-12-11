#include "cube.h"

void Cube::updateParams(int param1) {
    m_vertexData = std::vector<float>();
    m_param1 = param1;
    setVertexData();
}

/**
 * @brief makes tile of two triangles
 * @param topLeft
 * @param topRight
 * @param bottomLeft
 * @param bottomRight
 */
void Cube::makeTile(glm::vec3 topLeft,
                    glm::vec3 topRight,
                    glm::vec3 bottomLeft,
                    glm::vec3 bottomRight) {

    glm::vec3 leftSide = topLeft - bottomLeft;
    glm::vec3 bottomSide = bottomRight - bottomLeft;
    glm::vec3 normal = normalize(cross(bottomSide, leftSide));

    // first triangle
    insertVec3(m_vertexData, topLeft);
    insertVec3(m_vertexData, normal);
    insertVec3(m_vertexData, bottomLeft);
    insertVec3(m_vertexData, normal);
    insertVec3(m_vertexData, bottomRight);
    insertVec3(m_vertexData, normal);

    // second triangle
    insertVec3(m_vertexData, bottomRight);
    insertVec3(m_vertexData, normal);
    insertVec3(m_vertexData, topRight);
    insertVec3(m_vertexData, normal);
    insertVec3(m_vertexData, topLeft);
    insertVec3(m_vertexData, normal);
}

/**
 * @brief makes one face of the cube based on param1
 * @param topLeft
 * @param topRight
 * @param bottomLeft
 * @param bottomRight
 */
void Cube::makeFace(glm::vec3 topLeft,
                    glm::vec3 topRight,
                    glm::vec3 bottomLeft,
                    glm::vec3 bottomRight) {

    glm::vec3 incrementRight = (topRight - topLeft)/float(m_param1);
    glm::vec3 incrementDown = (bottomLeft - topLeft)/float(m_param1);

    for (int i = 0; i < m_param1; i++) {
        for (int j = 0; j < m_param1; j++) {

            glm::vec3 tL = topLeft + float(i)*incrementRight + float(j)*incrementDown;
            glm::vec3 tR = tL+incrementRight;
            glm::vec3 bL = tL+incrementDown;
            glm::vec3 bR = tR+incrementDown;

            makeTile(tL, tR, bL, bR);
        }
    }
}

/**
 * @brief generates six sides of cube using makeFace
 */
void Cube::setVertexData() {

    // on z = 0.5 plane
    makeFace(glm::vec3(-0.5f,  0.5f, 0.5f),
             glm::vec3( 0.5f,  0.5f, 0.5f),
             glm::vec3(-0.5f, -0.5f, 0.5f),
             glm::vec3( 0.5f, -0.5f, 0.5f));

    // on z = -0.5 plane
    makeFace(glm::vec3( 0.5f,  0.5f, -0.5f),
             glm::vec3(-0.5f,  0.5f, -0.5f),
             glm::vec3( 0.5f, -0.5f, -0.5f),
             glm::vec3(-0.5f, -0.5f, -0.5f));

    // on x = 0.5 plane
    makeFace(glm::vec3( 0.5f,  0.5f,  0.5f),
             glm::vec3( 0.5f,  0.5f, -0.5f),
             glm::vec3( 0.5f, -0.5f,  0.5f),
             glm::vec3( 0.5f, -0.5f, -0.5f));

    // on x = -0.5 plane
    makeFace(glm::vec3(-0.5f,  0.5f, -0.5f),
             glm::vec3(-0.5f,  0.5f,  0.5f),
             glm::vec3(-0.5f, -0.5f, -0.5f),
             glm::vec3(-0.5f, -0.5f,  0.5f));

    // on y = 0.5 plane
    makeFace(glm::vec3(-0.5f,  0.5f, -0.5f),
             glm::vec3( 0.5f,  0.5f, -0.5f),
             glm::vec3(-0.5f,  0.5f,  0.5f),
             glm::vec3( 0.5f,  0.5f,  0.5f));

    // on y = -0.5 plane
    makeFace(glm::vec3(-0.5f, -0.5f,  0.5f),
             glm::vec3( 0.5f, -0.5f,  0.5f),
             glm::vec3(-0.5f, -0.5f, -0.5f),
             glm::vec3( 0.5f, -0.5f, -0.5f));
}

// Inserts a glm::vec3 into a vector of floats.
// This will come in handy if you want to take advantage of vectors to build your shape!
void Cube::insertVec3(std::vector<float> &data, glm::vec3 v) {
    data.push_back(v.x);
    data.push_back(v.y);
    data.push_back(v.z);
}
