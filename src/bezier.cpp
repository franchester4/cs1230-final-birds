#include "bezier.h"
#include <iostream>

void Bezier::init(glm::vec4 cam_pos, glm::vec3 look) {
    glm::vec3 cam_pos3 = glm::vec3(cam_pos);
    glm::vec3 unit = glm::normalize(look);

    dir = glm::normalize(look);
    pts = {
        cam_pos3,
        (cam_pos3 + unit * d),
        (cam_pos3 + unit * d * d),
        (cam_pos3 + unit * d * d * d)
    };
}

// returns matrix of coeffs, 4 rows 3 columns
glm::mat3x4 Bezier::bezierCoeffs() {
    glm::mat4x3 mat;
    mat[0] = pts[0];
    mat[1] = pts[1];
    mat[2] = pts[2];
    mat[3] = pts[3];

    return bezier_mat * glm::transpose(mat);
}

glm::vec3 getSpherical(float t1, float t2) {
    t2 = fmin(M_PI, fmax(0.f, t2));
    return glm::vec3(cos(t1) * sin(t2), cos(t2), sin(t1) * sin(t2));
}

glm::vec3 Bezier::moveBy(float t1, float t2) {
    // recover the current values for dir
    dir = glm::normalize(dir);
    float cur_t2 = acos(dir[1]);
    if (abs(sin(cur_t2)) < 1e-1) {
        return getSpherical(t1, cur_t2 + t2);
    }
    float cur_t1 = acos(fmin(1.f, fmax(-1.f, dir[0] / sin(cur_t2))));
    if (abs(sin(cur_t1) * sin(cur_t2) - dir[2]) > 1e-1) {
        // adjust cur_t1 \in [0, pi]
        cur_t1 = 2 * M_PI - cur_t1;
    }
    // add t1, t2

    return getSpherical(cur_t1 + t1, cur_t2 + t2);

}

void Bezier::updatePoints(float t1, float t2) {
    // set points with given theta1, theta2
    // spherical
    for (int i = 1; i <= 3; i++) {
        pts[i] = ((float) pow(d, i)) * moveBy(t1 * i, t2 * i) + pts[0];
    }
}
