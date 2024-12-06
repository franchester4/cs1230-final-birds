#include "camera.h"

void Camera::setCamera(SceneCameraData cam, float near_, float far_, int width, int height) {
    pos = cam.pos;
    look = cam.look;
    up = cam.up;

    near = near_;
    far = far_;

    w = width;
    h = height;
    aspect = w * 1.0 / h;

    heightAngle = cam.heightAngle;


    setViewMatrix();
    setProjMatrix();
    setPV();
}

void Camera::updateNearFar(float near_, float far_) {
    near = near_;
    far = far_;
    setProjMatrix();
    setPV();
}

void Camera::setViewMatrix() {
    glm::vec3 p = {pos[0], pos[1], pos[2]};
    glm::vec3 l = {look[0], look[1], look[2]};
    glm::vec3 up3 = {up[0], up[1], up[2]};

    glm::vec3 w = -normalize(l);
    glm::vec3 v = normalize(up3 - glm::dot(up3, w) * w);
    glm::vec3 u = glm::cross(v, w);

    glm::mat4 m_trans = glm::mat4(1.0f);
    m_trans[3][0] = -pos[0];
    m_trans[3][1] = -pos[1];
    m_trans[3][2] = -pos[2];

    glm::mat4 m_rot(1.0f);
    m_rot[0][0] = u[0];
    m_rot[1][0] = u[1];
    m_rot[2][0] = u[2];
    m_rot[0][1] = v[0];
    m_rot[1][1] = v[1];
    m_rot[2][1] = v[2];
    m_rot[0][2] = w[0];
    m_rot[1][2] = w[1];
    m_rot[2][2] = w[2];

    viewMatrix =  m_rot * m_trans;
}

void Camera::setProjMatrix() {
    glm::mat4 clip(1.f);
    clip[2][2] = -2.f;
    clip[3][2] = -1.f;

    float c = -near / far;

    glm::mat4 unhinge(1.f);
    unhinge[2][2] = 1.f / (1.f + c);
    unhinge[3][2] = -1.f * c / (1.f + c);
    unhinge[2][3] = -1.f;
    unhinge[3][3] = 0.f;

    float h2 = tan(heightAngle / 2);
    float w2 = aspect * h2;

    glm::mat4 scaleMat(1.f);
    scaleMat[0][0] = 1.f / (far * w2);
    scaleMat[1][1] = 1.f / (far * h2);
    scaleMat[2][2] = 1.f / far;
    projMatrix = clip * unhinge * scaleMat;

}

void Camera::setPV() {
    pv = projMatrix * viewMatrix;
}



