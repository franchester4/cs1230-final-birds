#include "cylinder.h"

void Cylinder::updateParams(int param1, int param2) {
    m_vertexData = std::vector<float>();
    m_param1 = param1;
    m_param2 = param2;
    setVertexData();
}

/**
 * @brief make tile for top cap of cylinder
 * @param currentTheta
 * @param nextTheta
 */
void Cylinder::makeTopCapTile(float currentTheta, float nextTheta) {

    float step = 0.5/m_param1;

    float r = 0.5; // radius

    glm::vec3 normal = {0.f, 1.f, 0.f};

    for (int i = 0; i < m_param1; i++) {

        glm::vec3 tL = {(r-step*(i+1))*cos(currentTheta), 0.5, (r-step*(i+1))*sin(currentTheta)};
        glm::vec3 bL = {(r-step*i)*cos(currentTheta), 0.5, (r-step*i)*sin(currentTheta)};
        glm::vec3 tR = {(r-step*(i+1))*cos(nextTheta), 0.5, (r-step*(i+1))*sin(nextTheta)};
        glm::vec3 bR = {(r-step*i)*cos(nextTheta), 0.5, (r-step*i)*sin(nextTheta)};

        // first triangle
        insertVec3(m_vertexData, tL);
        insertVec3(m_vertexData, normal);
        insertVec3(m_vertexData, tR);
        insertVec3(m_vertexData, normal);
        insertVec3(m_vertexData, bL);
        insertVec3(m_vertexData, normal);

        // second triangle
        insertVec3(m_vertexData, tR);
        insertVec3(m_vertexData, normal);
        insertVec3(m_vertexData, bR);
        insertVec3(m_vertexData, normal);
        insertVec3(m_vertexData, bL);
        insertVec3(m_vertexData, normal);
    }
}

/**
 * @brief make tile for bottom cap of cylinder
 * @param currentTheta
 * @param nextTheta
 */
void Cylinder::makeBottomCapTile(float currentTheta, float nextTheta) {

    float step = 0.5/m_param1;

    float r = 0.5; // radius

    glm::vec3 normal = {0.f, -1.f, 0.f};

    for (int i = 0; i < m_param1; i++) {

        glm::vec3 tL = {(r-step*(i+1))*cos(currentTheta), -0.5, (r-step*(i+1))*sin(currentTheta)};
        glm::vec3 bL = {(r-step*i)*cos(currentTheta), -0.5, (r-step*i)*sin(currentTheta)};
        glm::vec3 tR = {(r-step*(i+1))*cos(nextTheta), -0.5, (r-step*(i+1))*sin(nextTheta)};
        glm::vec3 bR = {(r-step*i)*cos(nextTheta), -0.5, (r-step*i)*sin(nextTheta)};

        // first triangle
        insertVec3(m_vertexData, tL);
        insertVec3(m_vertexData, normal);
        insertVec3(m_vertexData, bL);
        insertVec3(m_vertexData, normal);
        insertVec3(m_vertexData, tR);
        insertVec3(m_vertexData, normal);

        // second triangle
        insertVec3(m_vertexData, tR);
        insertVec3(m_vertexData, normal);
        insertVec3(m_vertexData, bL);
        insertVec3(m_vertexData, normal);
        insertVec3(m_vertexData, bR);
        insertVec3(m_vertexData, normal);
    }
}

/**
 * @brief generates top and bottom cap based on param2
 */
void Cylinder::makeCaps() {

    float thetaIncrement = glm::radians(360.f / m_param2);

    for (int i = 0; i < m_param2; i++) {

        float currentTheta = i*thetaIncrement;
        float nextTheta = currentTheta+thetaIncrement;

        makeTopCapTile(currentTheta, nextTheta);
        makeBottomCapTile(currentTheta, nextTheta);
    }
}

/**
 * @brief makes tile for cylinder body
 * @param topLeft
 * @param topRight
 * @param bottomLeft
 * @param bottomRight
 */
void Cylinder::makeTile(glm::vec3 topLeft,
                        glm::vec3 topRight,
                        glm::vec3 bottomLeft,
                        glm::vec3 bottomRight) {

    // normal calculated as gradient of implicit equation
    glm::vec3 normalTopLeft = glm::vec3(2.f*topLeft.x, 0.f, 2.f*topLeft.z);
    glm::vec3 normalTopRight = glm::vec3(2.f*topRight.x, 0.f, 2.f*topRight.z);
    glm::vec3 normalBottomLeft = glm::vec3(2.f*bottomLeft.x, 0.f, 2.f*bottomLeft.z);
    glm::vec3 normalBottomRight = glm::vec3(2.f*bottomRight.x, 0.f, 2.f*bottomRight.z);

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
 * @brief creates wedge for cylinder body based on param1
 * @param currentTheta
 * @param nextTheta
 */
void Cylinder::makeWedge(float currentTheta, float nextTheta) {

    float r = 0.5; // radius

    float cosCurrTheta = cos(currentTheta);
    float sinCurrTheta = sin(currentTheta);
    float cosNextTheta = cos(nextTheta);
    float sinNextTheta = sin(nextTheta);

    for (int i = 0; i < m_param1; i++) {

        float currY = 0.5 - (1.f/m_param1)*i;
        float nextY = 0.5 - (1.f/m_param1)*(i+1);

        glm::vec3 tL = {r*cosCurrTheta, currY, r*sinCurrTheta};
        glm::vec3 tR = {r*cosNextTheta, currY, r*sinNextTheta};
        glm::vec3 bL = {r*cosCurrTheta, nextY, r*sinCurrTheta};
        glm::vec3 bR = {r*cosNextTheta, nextY, r*sinNextTheta};

        makeTile(tR, tL, bR, bL);
    }
}

/**
 * @brief creates cylinder body by creating wedges based on param2
 */
void Cylinder::makeBody() {

    float thetaIncrement = glm::radians(360.f / m_param2);

    for (int i = 0; i < m_param2; i++) {

        float currentTheta = i*thetaIncrement;
        float nextTheta = currentTheta+thetaIncrement;

        makeWedge(currentTheta, nextTheta);
    }
}

/**
 * @brief generate vertex data for cylinder body and caps, called in updateParams
 */
void Cylinder::setVertexData() {
    makeBody();
    makeCaps();
}

// Inserts a glm::vec3 into a vector of floats.
// This will come in handy if you want to take advantage of vectors to build your shape!
void Cylinder::insertVec3(std::vector<float> &data, glm::vec3 v) {
    data.push_back(v.x);
    data.push_back(v.y);
    data.push_back(v.z);
}
