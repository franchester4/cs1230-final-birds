#pragma once

#include <vector>
#include <glm/glm.hpp>

class Cone
{
public:
    void updateParams(int param1, int param2);
    std::vector<float> generateShape() { return m_vertexData; }

    void makeCone();
    void makeBase();
    glm::vec3 getNth(glm::vec3& A, glm::vec3& B, int i, int n);
    void makeSide(float theta1, float theta2);
    void makeSideTriangle(std::vector<glm::vec3>&A, std::vector<glm::vec3>&B, int i);

private:
    void insertVec3(std::vector<float> &data, glm::vec3 v);
    void setVertexData();

    std::vector<float> m_vertexData;
    int m_param1;
    int m_param2;
    float m_radius = 0.5;
};
