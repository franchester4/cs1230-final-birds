#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>
// Contains all shape data (aka scene primitives)

class Bezier
{
public:
    float d = 1.1; // distance to move for each piece
    void init(glm::vec4 cam_pos, glm::vec3 look);
    glm::mat3x4 bezierCoeffs();

    std::vector<glm::vec3> genBounding(glm::vec3 src, glm::vec3 grad);

    glm::vec3 dir;

    float theta1 = 0;
    float theta2 = 0;

    std::vector<glm::vec3> pts;

    glm::mat3 rotationMat(glm::vec3 A, glm::vec3 B);

    void updatePoints(float t1, float t2);
    glm::vec3 moveBy(float t1, float t2);
private:
    void setCircle();
    std::vector<glm::vec3> space_points;
    glm::mat4x4 bezier_mat =
        glm::inverse(
            glm::mat4(
                0, 1, 0, 3,
                0, 1, 0, 2,
                0, 1, 1, 1,
                1, 1, 0, 0)
            ) *
            glm::mat4(
                1, 0, -3, 0,
                0, 0, 3, 0,
                0, 0, 0, -3,
                0, 1, 0, 3
            );

};
