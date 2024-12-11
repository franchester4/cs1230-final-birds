#include "cone.h"

#include <algorithm>
#include <iostream>

void Cone::updateParams(int param1, int param2) {
    m_vertexData = std::vector<float>();
    m_param1 = param1;
    m_param2 = param2;
    setVertexData();
}

/**
 * @brief makes tile for bottom cap of cone
 * @param currentTheta
 * @param nextTheta
 */
void Cone::makeBottomCapTile(float currentTheta, float nextTheta) {

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
 * @brief creates bottom cap of cone based on param2
 */
void Cone::makeCap() {

    float thetaIncrement = glm::radians(360.f / m_param2);

    for (int i = 0; i < m_param2; i++) {

        float currentTheta = i*thetaIncrement;
        float nextTheta = currentTheta+thetaIncrement;

        makeBottomCapTile(currentTheta, nextTheta);
    }
}

/**
 * @brief calculates normal of cone tip
 * @param leftNorm
 * @param rightNorm
 * @return
 */
glm::vec3 calculateTipNormal(glm::vec3 leftNorm,
                             glm::vec3 rightNorm) {

    glm::vec3 tipNorm = glm::vec3((leftNorm+rightNorm)/2.f);
    tipNorm.y = 0.f;
    tipNorm = normalize(tipNorm);
    tipNorm.y = 0.5;
    tipNorm = normalize(tipNorm);
    return tipNorm;
}

/**
 * @brief makes tile for cone body
 * @param topLeft
 * @param topRight
 * @param bottomLeft
 * @param bottomRight
 * @param isTip
 */
void Cone::makeTile(glm::vec3 topLeft,
                    glm::vec3 topRight,
                    glm::vec3 bottomLeft,
                    glm::vec3 bottomRight, bool isTip) {

    glm::vec3 normalLeft = glm::vec3(2.f*bottomLeft.x, 0.25 - 0.5*bottomLeft.y, 2.f*bottomLeft.z);
    glm::vec3 normalRight = glm::vec3(2.f*bottomRight.x, 0.25 - 0.5*bottomRight.y, 2.f*bottomRight.z);

    normalLeft = normalize(normalLeft);
    normalRight = normalize(normalRight);

    // if generating cone tip, use different calculation for normal
    if (isTip) {
        glm::vec3 tipNorm = calculateTipNormal(normalLeft, normalRight);

        insertVec3(m_vertexData, glm::vec3(0.f, 0.5, 0.f));
        insertVec3(m_vertexData, tipNorm);
        insertVec3(m_vertexData, bottomLeft);
        insertVec3(m_vertexData, normalLeft);
        insertVec3(m_vertexData, bottomRight);
        insertVec3(m_vertexData, normalRight);
    } else {

        // first triangle
        insertVec3(m_vertexData, topLeft);
        insertVec3(m_vertexData, normalLeft);
        insertVec3(m_vertexData, bottomLeft);
        insertVec3(m_vertexData, normalLeft);
        insertVec3(m_vertexData, bottomRight);
        insertVec3(m_vertexData, normalRight);

        // second triangle
        insertVec3(m_vertexData, bottomRight);
        insertVec3(m_vertexData, normalRight);
        insertVec3(m_vertexData, topRight);
        insertVec3(m_vertexData, normalRight);
        insertVec3(m_vertexData, topLeft);
        insertVec3(m_vertexData, normalLeft);
    }
}

// maps value in a certain range within new range
// took from site -> https://docs.arduino.cc/language-reference/en/functions/math/map/
float map(double value, double fromLow, double fromHigh, double toLow, double toHigh) {
    return (value - fromLow) * (toHigh - toLow) / (fromHigh - fromLow) + toLow;
}

/**
 * @brief makes wedge of cone based on param1
 * @param currentTheta
 * @param nextTheta
 */
void Cone::makeWedge(float currentTheta, float nextTheta) {

    float cosCurrTheta = cos(currentTheta);
    float sinCurrTheta = sin(currentTheta);
    float cosNextTheta = cos(nextTheta);
    float sinNextTheta = sin(nextTheta);

    for (int i = 0; i < m_param1; i++) {

        float currY = 0.5 - (1.f/m_param1)*i;
        float nextY = 0.5 - (1.f/m_param1)*(i+1);

        float topR = map(currY, 0.5, -0.5, 0.f, 0.5); // radius for top two points
        float bottomR = map(nextY, 0.5, -0.5, 0.f, 0.5); // radius for bottom two points

        glm::vec3 tL = {topR*cosCurrTheta, currY, topR*sinCurrTheta};
        glm::vec3 tR = {topR*cosNextTheta, currY, topR*sinNextTheta};
        glm::vec3 bL = {bottomR*cosCurrTheta, nextY, bottomR*sinCurrTheta};
        glm::vec3 bR = {bottomR*cosNextTheta, nextY, bottomR*sinNextTheta};

        if (i == 0) { // if i = 0, then at cone tip
            makeTile(tR, tL, bR, bL, true);
        } else {
            makeTile(tR, tL, bR, bL, false);
        }

    }
}

/**
 * @brief creates body of cone by making wedges based on param2
 */
void Cone::makeBody() {
    float thetaIncrement = glm::radians(360.f / m_param2);

    for (int i = 0; i < m_param2; i++) {

        float currentTheta = i*thetaIncrement;
        float nextTheta = currentTheta+thetaIncrement;

        makeWedge(currentTheta, nextTheta);
    }
}

/**
 * @brief generates vertex data for cone body and cap
 */
void Cone::setVertexData() {
    makeBody();
    makeCap();
}


// Inserts a glm::vec3 into a vector of floats.
// This will come in handy if you want to take advantage of vectors to build your shape!
void Cone::insertVec3(std::vector<float> &data, glm::vec3 v) {
    data.push_back(v.x);
    data.push_back(v.y);
    data.push_back(v.z);
}

