#include "Cone.h"

void Cone::updateParams(int param1, int param2) {
    m_vertexData = std::vector<float>();
    m_param1 = fmax(1, param1);
    m_param2 = fmax(3, param2);
    setVertexData();
}



// get i/n from A to B
glm::vec3 Cone::getNth(glm::vec3& A, glm::vec3& B, int i, int n) {
    glm::vec3 displacement = B - A;
    float val = i * 1.0 / n;
    return A + val * displacement;
}

void Cone::makeSideTriangle(std::vector<glm::vec3>&A, std::vector<glm::vec3>&B, int i) {
    insertVec3(m_vertexData, A[2*i]); insertVec3(m_vertexData, A[2*i+1]);
    insertVec3(m_vertexData, B[2*i]); insertVec3(m_vertexData, B[2*i+1]);
    insertVec3(m_vertexData, B[2*i+2]); insertVec3(m_vertexData, B[2*i+3]);

    insertVec3(m_vertexData, B[2*i+2]); insertVec3(m_vertexData, B[2*i+3]);
    insertVec3(m_vertexData, A[2*i+2]); insertVec3(m_vertexData, A[2*i+3]);
    insertVec3(m_vertexData, A[2*i]); insertVec3(m_vertexData, A[2*i+1]);

}

void Cone::makeBase() {

}
// void Cone::makeSide(float theta1, float theta2) {

void Cone::makeSide(float theta1, float theta2) {
    // SIDE

    // ccw is theta1, theta2, center
    glm::vec3 center = {0.0, 0.5, 0.0};
    glm::vec3 A = {.5 * cos(theta1), -0.5, 0.5 * sin(theta1)};
    glm::vec3 B = {.5 * cos(theta2), -0.5, 0.5 * sin(theta2)};

    std::vector<glm::vec3>nthA; // even is pos, odd is norm
    std::vector<glm::vec3>nthB;
    for (int i = 0; i <= m_param1; i++) {
        nthA.push_back(getNth(center, A, i, m_param1));
        nthA.push_back(glm::vec3{2 * nthA[2*i][0], 0.25 - 0.5 * nthA[2*i][1], 2 * nthA[2*i][2]});
        nthA[2*i + 1] = glm::normalize(nthA[2 * i + 1]);

        nthB.push_back(getNth(center, B, i, m_param1));
        nthB.push_back(glm::vec3{2 * nthB[2*i][0], 0.25 - 0.5 * nthB[2*i][1], 2 * nthB[2*i][2]});
        nthB[2*i + 1] = glm::normalize(nthB[2 * i + 1]);
    }
    // top
    glm::vec3 A_norm = glm::normalize(glm::vec3{2 * A[0], 0.25 - 0.5 * A[1], 2 * A[2]});
    glm::vec3 B_norm = glm::normalize(glm::vec3{2 * B[0], 0.25 - 0.5 * B[1], 2 * B[2]});
    glm::vec3 center_norm = (A_norm + B_norm) * 0.5f;
    center_norm[1] = 0.f;
    center_norm = normalize(center_norm);
    center_norm[1] = (A_norm[1] + B_norm[1]) * 0.5;
    center_norm = normalize(center_norm);
    insertVec3(m_vertexData, nthA[2]); insertVec3(m_vertexData, nthA[3]);
    insertVec3(m_vertexData, center); insertVec3(m_vertexData, center_norm);
    insertVec3(m_vertexData, nthB[2]); insertVec3(m_vertexData, nthB[3]);

    // rest
    for (int i = 1; i < m_param1; i++) {
        makeSideTriangle(nthA, nthB, i);
    }

    // BASE
    glm::vec3 baseCenter = {0.0, -0.5, 0.0};
    glm::vec3 down = {0.0, -1.0, 0.0};

    for (int i = 0; i <= m_param1; i++) {
        nthA[2 * i] = getNth(baseCenter, A, i, m_param1);
        nthA[2 * i + 1] = down;
        nthB[2 * i] = getNth(baseCenter, B, i, m_param1);
        nthB[2 * i + 1] = down;
    }
    // top
    insertVec3(m_vertexData, nthA[2]); insertVec3(m_vertexData, nthA[3]);
    insertVec3(m_vertexData, nthB[2]); insertVec3(m_vertexData, nthB[3]);
    insertVec3(m_vertexData, baseCenter); insertVec3(m_vertexData, down);

    // rest
    for (int i = 1; i < m_param1; i++) {
        makeSideTriangle(nthB, nthA, i);
    }

}

void Cone::makeCone() {
    float thetaStep = glm::radians(360.f / m_param2);
    glm::vec3 baseCenter = {0, -0.5, 0};
    glm::vec3 down = {0, -1, 0};
    for (int j = 0; j < m_param2; j++) {
        makeSide(j * thetaStep, (j + 1) * thetaStep);
    }

}

void Cone::setVertexData() {
    // TODO for Project 5: Lights, Camera
    makeCone();
}


// Inserts a glm::vec3 into a vector of floats.
// This will come in handy if you want to take advantage of vectors to build your shape!
void Cone::insertVec3(std::vector<float> &data, glm::vec3 v) {
    data.push_back(v.x);
    data.push_back(v.y);
    data.push_back(v.z);
}
