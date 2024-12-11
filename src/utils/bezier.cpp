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

/*
 * builds num_pts sided polygon to bound a circle like shape
 * for now, circle around the axis (0, 1, 0)
 * will probably look more like a flower?
 * [TODO] set min val for num_pts
*/
void Bezier::setCircle() {
    // ideally we have the camera always tangent to thiis curve
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
    // t1 = fmin(M_PI / 3.f, fmax(t1, -M_PI / 3.f));
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
        // std::cout<<i<<" "<<pts[i][0]<<" "<<pts[i][1]<<" "<<pts[i][2]<<"\n";
    }
}




// rotation matrix from norm(A) to norm(B) [gets directions]
glm::mat3 Bezier::rotationMat(glm::vec3 A, glm::vec3 B) {
    glm::vec3 a = glm::normalize(A);
    glm::vec3 b = glm::normalize(B);
    glm::vec3 v = glm::cross(a, b);
    float cos = glm::dot(a, b);

    glm::mat3 res = glm::mat3(1.0);

    glm::mat3 skew = glm::mat3(
        0, v[2], -v[1],
        -v[2], 0, v[0],
        v[1], -v[0], 0);

    return res + skew +  (1.f /(1.f + cos)) * skew * skew;
}

// randomly generates bezier curve with given source and start direction
std::vector<glm::vec3> Bezier::genBounding(glm::vec3 src, glm::vec3 grad) {
    // randomly gen in distance d
    // want vectors within some "cone"

    // idea: gen d for each 3 differently (maybe like d/3, 2d/3, d)
    // want cos(normalized, src) <= sqrt3/2 or smth

    glm::vec3 normal = glm::normalize(grad);

    // do it about (0, 1, 0)
    std::vector<glm::vec3>result;
    result.push_back(src);
    for (int i = 0; i < 3; i++) {
        float len = d * (1.f * i) / 3.f;
        float y_coord = sqrt(3.f) / 2.f;
        float xz_len = 0.5f;
        // get random val in this cone
        float theta = rand() * 2 * M_PI;

        glm::vec3 normalized_delta = glm::vec3(xz_len * cos(theta), y_coord, xz_len * sin(theta));

        // want to send this back

        // rotate normalzied_delta to

        glm::vec3 lol = rotationMat(normalized_delta, normal) * normalized_delta;

        // now lol is the delta pointing along the direction of normal

        result.push_back(src + len * lol);
    }
    return result;
}

