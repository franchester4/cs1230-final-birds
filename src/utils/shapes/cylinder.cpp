#include "Cylinder.h"

void Cylinder::updateParams(int param1, int param2) {
    m_vertexData = std::vector<float>();
    m_param1 = fmax(1, param1);
    m_param2 = fmax(3, param2);
    setVertexData();
}

glm::vec3 sideNormal(glm::vec3 v) {
    return glm::normalize(glm::vec3{v.x * 2, 0, v.z * 2});
}

glm::vec3 Cylinder::getNth(glm::vec3& A, glm::vec3& B, int i, int n) {
    glm::vec3 displacement = B - A;
    float val = i * 1.0 / n;
    return A + val * displacement;
}

void Cylinder::makeTile(glm::vec3 topLeft,
                        glm::vec3 topRight,
                        glm::vec3 bottomLeft,
                        glm::vec3 bottomRight) {

    insertVec3(m_vertexData, topLeft); insertVec3(m_vertexData, sideNormal(topLeft));
    insertVec3(m_vertexData, bottomLeft); insertVec3(m_vertexData, sideNormal(bottomLeft));
    insertVec3(m_vertexData, bottomRight); insertVec3(m_vertexData, sideNormal(bottomRight));

    insertVec3(m_vertexData, bottomRight); insertVec3(m_vertexData, sideNormal(bottomRight));
    insertVec3(m_vertexData, topRight); insertVec3(m_vertexData, sideNormal(topRight));
    insertVec3(m_vertexData, topLeft); insertVec3(m_vertexData, sideNormal(topLeft));
}

void Cylinder::makeSide(float theta1, float theta2) {
    // SIDE
    glm::vec3 topRight = {0.5 * cos(theta1), 0.5, 0.5 * sin(theta1)};
    glm::vec3 topLeft = {0.5 * cos(theta2), 0.5, 0.5 * sin(theta2)};
    glm::vec3 bottomRight = {0.5 * cos(theta1), -0.5, 0.5 * sin(theta1)};
    glm::vec3 bottomLeft = {0.5 * cos(theta2), -0.5, 0.5 * sin(theta2)};
    // ok so this defines the corners
    for (int i = 0; i < m_param1; i++) {
        makeTile(getNth(topLeft, bottomLeft, i, m_param1),
                 getNth(topRight, bottomRight, i, m_param1),
                 getNth(topLeft, bottomLeft, i + 1, m_param1),
                 getNth(topRight, bottomRight, i + 1, m_param1));
    }
}

void Cylinder::makeTop(float theta1, float theta2) {
    glm::vec3 topRight = {0.5 * cos(theta1), 0.5, 0.5 * sin(theta1)};
    glm::vec3 topLeft = {0.5 * cos(theta2), 0.5, 0.5 * sin(theta2)};
    glm::vec3 topCenter = {0.0, 0.5, 0.0};
    glm::vec3 up = {0.0, 1.0, 0.0};
    // make center
    insertVec3(m_vertexData, getNth(topCenter, topLeft, 1, m_param1));
    insertVec3(m_vertexData, up);
    insertVec3(m_vertexData, getNth(topCenter, topRight, 1, m_param1));
    insertVec3(m_vertexData, up);
    insertVec3(m_vertexData, topCenter);
    insertVec3(m_vertexData, up);
    for (int i = 1; i < m_param1; i++) {
        insertVec3(m_vertexData, getNth(topCenter, topLeft, i, m_param1));
        insertVec3(m_vertexData, up);

        insertVec3(m_vertexData, getNth(topCenter, topRight, i + 1, m_param1));
        insertVec3(m_vertexData, up);

        insertVec3(m_vertexData, getNth(topCenter, topRight, i, m_param1));
        insertVec3(m_vertexData, up);

        insertVec3(m_vertexData, getNth(topCenter, topLeft, i + 1, m_param1));
        insertVec3(m_vertexData, up);

        insertVec3(m_vertexData, getNth(topCenter, topRight, i + 1, m_param1));
        insertVec3(m_vertexData, up);

        insertVec3(m_vertexData, getNth(topCenter, topLeft, i, m_param1));
        insertVec3(m_vertexData, up);
    }
}


void Cylinder::makeBottom(float theta1, float theta2) {
    glm::vec3 bottomRight = {0.5 * cos(theta1), -0.5, 0.5 * sin(theta1)};
    glm::vec3 bottomLeft = {0.5 * cos(theta2), -0.5, 0.5 * sin(theta2)};
    glm::vec3 bottomCenter = {0.0, -0.5, 0.0};
    glm::vec3 down = {0.0, -1.0, 0.0};
    // make center
    insertVec3(m_vertexData, getNth(bottomCenter, bottomRight, 1, m_param1));
    insertVec3(m_vertexData, down);
    insertVec3(m_vertexData, getNth(bottomCenter, bottomLeft, 1, m_param1));
    insertVec3(m_vertexData, down);
    insertVec3(m_vertexData, bottomCenter);
    insertVec3(m_vertexData, down);

    for (int i = 1; i < m_param1; i++) {
        insertVec3(m_vertexData, getNth(bottomCenter, bottomRight, i, m_param1));
        insertVec3(m_vertexData, down);

        insertVec3(m_vertexData, getNth(bottomCenter, bottomLeft, i + 1, m_param1));
        insertVec3(m_vertexData, down);

        insertVec3(m_vertexData, getNth(bottomCenter, bottomLeft, i, m_param1));
        insertVec3(m_vertexData, down);

        insertVec3(m_vertexData, getNth(bottomCenter, bottomRight, i + 1, m_param1));
        insertVec3(m_vertexData, down);

        insertVec3(m_vertexData, getNth(bottomCenter, bottomLeft, i + 1, m_param1));
        insertVec3(m_vertexData, down);

        insertVec3(m_vertexData, getNth(bottomCenter, bottomRight, i, m_param1));
        insertVec3(m_vertexData, down);
    }
}


void Cylinder::makeCylinder() {
    float thetaStep = glm::radians(360.f / m_param2);
    for (int i = 0; i < m_param2; i++) {
        makeSide(i * thetaStep, (i + 1) * thetaStep);
        makeTop(i * thetaStep, (i + 1) * thetaStep);
        makeBottom(i * thetaStep, (i + 1) * thetaStep);
    }
}


void Cylinder::setVertexData() {
    // TODO for Project 5: Lights, Camera
    makeCylinder();
}

// Inserts a glm::vec3 into a vector of floats.
// This will come in handy if you want to take advantage of vectors to build your shape!
void Cylinder::insertVec3(std::vector<float> &data, glm::vec3 v) {
    data.push_back(v.x);
    data.push_back(v.y);
    data.push_back(v.z);
}
