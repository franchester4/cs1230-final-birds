#include "sphere.h"

void Sphere::updateParams(int param1, int param2) {
    m_vertexData = std::vector<float>();
    m_param1 = param1;
    m_param2 = param2;
    setVertexData();
}

/**
 * @brief makes single tile of two triangles
 * @param topLeft
 * @param topRight
 * @param bottomLeft
 * @param bottomRight
 */
void Sphere::makeTile(glm::vec3 topLeft,
                      glm::vec3 topRight,
                      glm::vec3 bottomLeft,
                      glm::vec3 bottomRight) {

    glm::vec3 normalTopLeft = normalize(topLeft);
    glm::vec3 normalTopRight = normalize(topRight);
    glm::vec3 normalBottomLeft = normalize(bottomLeft);
    glm::vec3 normalBottomRight = normalize(bottomRight);

    // first triangle
    insertVec3(m_vertexData, topLeft);
    insertVec3(m_vertexData, normalTopLeft);
    insertVec3(m_vertexData, bottomLeft);
    insertVec3(m_vertexData, normalBottomLeft);
    insertVec3(m_vertexData, bottomRight);
    insertVec3(m_vertexData, normalBottomRight);

    // second triangle
    insertVec3(m_vertexData, bottomRight);
    insertVec3(m_vertexData, normalBottomRight);
    insertVec3(m_vertexData, topRight);
    insertVec3(m_vertexData, normalTopRight);
    insertVec3(m_vertexData, topLeft);
    insertVec3(m_vertexData, normalTopLeft);
}

/**
 * @brief creates one wedge of the sphere based on param1
 * @param currentTheta
 * @param nextTheta
 */
void Sphere::makeWedge(float currentTheta, float nextTheta) {

    float phiIncrement = glm::radians(180.f/m_param1);

    float r = 0.5;

    float cosCurrTheta = cos(currentTheta);
    float sinCurrTheta = sin(currentTheta);
    float cosNextTheta = cos(nextTheta);
    float sinNextTheta = sin(nextTheta);

    for (int i = 0; i < m_param1; i++) {

        float currPhi = i*phiIncrement;
        float sinPhi = sin(currPhi);
        float cosPhi = cos(currPhi);

        float nextPhi = currPhi + phiIncrement;
        float sinNextPhi = sin(nextPhi);
        float cosNextPhi = cos(nextPhi);

        glm::vec3 tL = {r*sinPhi*cosCurrTheta, r*cosPhi, r*sinPhi*sinCurrTheta};
        glm::vec3 tR = {r*sinPhi*cosNextTheta, r*cosPhi, r*sinPhi*sinNextTheta};
        glm::vec3 bL = {r*sinNextPhi*cosCurrTheta, r*cosNextPhi, r*sinNextPhi*sinCurrTheta};
        glm::vec3 bR = {r*sinNextPhi*cosNextTheta, r*cosNextPhi, r*sinNextPhi*sinNextTheta};

        makeTile(tR, tL, bR, bL);
    }
}

/**
 * @brief creates sphere by making wedges based on param2
 */
void Sphere::makeSphere() {

    float thetaIncrement = glm::radians(360.f / m_param2);

    for (int i = 0; i < m_param2; i++) {

        float currentTheta = i*thetaIncrement;
        float nextTheta = currentTheta+thetaIncrement;

        makeWedge(currentTheta, nextTheta);
    }
}

/**
 * @brief sets vertex data for sphere, called in updateParams
 */
void Sphere::setVertexData() {
    makeSphere();
}

// Inserts a glm::vec3 into a vector of floats.
// This will come in handy if you want to take advantage of vectors to build your shape!
void Sphere::insertVec3(std::vector<float> &data, glm::vec3 v) {
    data.push_back(v.x);
    data.push_back(v.y);
    data.push_back(v.z);
}
