#include "Sphere.h"

void Sphere::updateParams(int param1, int param2) {
    m_vertexData = std::vector<float>();
    m_param1 = fmax(2, param1);
    m_param2 = fmax(3, param2);
    setVertexData();
}

void Sphere::makeTile(glm::vec3 topLeft,
                      glm::vec3 topRight,
                      glm::vec3 bottomLeft,
                      glm::vec3 bottomRight) {
    // Task 5: Implement the makeTile() function for a Sphere
    // Note: this function is very similar to the makeTile() function for Cube,
    //       but the normals are calculated in a different way!

    // TL, BL, BR
    // BR, TR, TL
    glm::vec3 TL = glm::normalize(2.0f * topLeft);
    glm::vec3 BL = glm::normalize(2.0f * bottomLeft);
    glm::vec3 BR = glm::normalize(2.0f * bottomRight);

    glm::vec3 BR1 = glm::normalize(2.0f * bottomRight);
    glm::vec3 TR1 = glm::normalize(2.0f * topRight);
    glm::vec3 TL1 = glm::normalize(2.0f * topLeft);

    insertVec3(m_vertexData, topLeft); insertVec3(m_vertexData, TL);
    insertVec3(m_vertexData, bottomLeft); insertVec3(m_vertexData, BL);
    insertVec3(m_vertexData, bottomRight); insertVec3(m_vertexData, BR);
    insertVec3(m_vertexData, bottomRight); insertVec3(m_vertexData, BR1);
    insertVec3(m_vertexData, topRight); insertVec3(m_vertexData, TR1);
    insertVec3(m_vertexData, topLeft); insertVec3(m_vertexData, TL1);
}

glm::vec3 getCoord(float theta, float phi) {
    return glm::vec3{.5 * sin(phi) * cos(theta), .5 * cos(phi), .5 * sin(phi) * sin(theta)};
}

void Sphere::makeWedge(float currentTheta, float nextTheta) {
    // Task 6: create a single wedge of the sphere using the
    //         makeTile() function you implemented in Task 5
    // Note: think about how param 1 comes into play here!

    float phiStep = glm::radians(180.f / m_param1);
    // float start = glm::radians(90.f)
    for (int i = 0; i < m_param1; i++) {
        glm::vec3 tl = getCoord(currentTheta, i * phiStep);
        glm::vec3 tr = getCoord(nextTheta, i * phiStep);
        glm::vec3 bl = getCoord(currentTheta, (i + 1) * phiStep);
        glm::vec3 br = getCoord(nextTheta, (i + 1) * phiStep);
        makeTile(tr, tl, br, bl);
    }
}

void Sphere::makeSphere() {
    // Task 7: create a full sphere using the makeWedge() function you
    //         implemented in Task 6
    // Note: think about how param 2 comes into play here!
    float thetaStep = glm::radians(360.f / m_param2);
    for (int j = 0; j < m_param2; j++) {
        makeWedge(j * thetaStep, (j + 1) * thetaStep);
    }
}

void Sphere::setVertexData() {
    // Uncomment these lines to make a wedge for Task 6, then comment them out for Task 7:

    // float thetaStep = glm::radians(360.f / m_param2);
    // float currentTheta = 0 * thetaStep;
    // float nextTheta = 1 * thetaStep;
    // makeWedge(currentTheta, nextTheta);

    // Uncomment these lines to make sphere for Task 7:

    makeSphere();
}

// Inserts a glm::vec3 into a vector of floats.
// This will come in handy if you want to take advantage of vectors to build your shape!
void Sphere::insertVec3(std::vector<float> &data, glm::vec3 v) {
    data.push_back(v.x);
    data.push_back(v.y);
    data.push_back(v.z);
}
