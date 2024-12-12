#include <stdexcept>
#include "camera.h"

#include <glm/glm.hpp>

#include "glm/ext/matrix_clip_space.hpp"
#include "settings.h"
#include <iostream>

glm::mat4 viewMatrix;
glm::mat4 inverseViewMatrix;
glm::vec4 camWorldPos;

Camera::Camera() {

}

Camera::Camera(const SceneCameraData &data) {
    cameraData = data;
    this->setViewMatrix();
    this->setInverseViewMatrix();
    this->setWorldPos();
    cam_start_position = getWorldPos();
    cam_x_rotation = 0;
    cam_y_rotation = 0;
    start_look = data.look;
}

/**
 * @brief creates view matrix given camera data, called when camera is initialized
 */
void Camera::setViewMatrix() {

    glm::vec3 pos = cameraData.pos;
    glm::vec3 look = cameraData.look;
    glm::vec3 up = cameraData.up;

    glm::vec3 w = -normalize(look);
    glm::vec3 v = normalize(up - glm::dot(up, w)*w);
    glm::vec3 u = glm::cross(v, w);

    glm::mat4 rotationMat = {u[0], v[0], w[0], 0.f, u[1], v[1], w[1], 0.f, u[2], v[2], w[2], 0.f, 0.f, 0.f, 0.f, 1.f};
    glm::mat4 translationMat = {1.f, 0.f, 0.f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.f, 0.f, 1.f, 0.f, -pos[0], -pos[1], -pos[2], 1.f};

    viewMatrix = rotationMat*translationMat;
}

/**
 * @brief returns view matrix
 * @return
 */
glm::mat4 Camera::getViewMatrix() const {
    return viewMatrix;
}

/**
 * @brief calculates inverse view matrix, set when camera is initiated
 */
void Camera::setInverseViewMatrix() {
    inverseViewMatrix = glm::inverse(viewMatrix);
}

/**
 * @brief returns inverse view matrix
 * @return
 */
glm::mat4 Camera::getInverseViewMatrix() const {
    return inverseViewMatrix;
}

/**
 * @brief creates projection matrix
 * @param aspectRatio
 * @param far
 * @param near
 * @return
 */
glm::mat4 Camera::setProjectionMatrix(float aspectRatio, float far, float near) {

    // scaling matrix
    float widthAngle = atan(aspectRatio*tan(cameraData.heightAngle/2.f))*2.f;
    glm::mat4 scaleMat = {(1.f/(far*tan(widthAngle/2.f))), 0.f, 0.f, 0.f,
                          0.f, (1.f/(far*tan(cameraData.heightAngle/2.f))), 0.f, 0.f,
                          0.f, 0.f, 1/far, 0.f,
                          0.f, 0.f, 0.f, 1.f};

   // matrix for unhinging frustum
    float c = -near/far;
    glm::mat4 unhingeMat = {1.f, 0.f, 0.f, 0.f,
                            0.f, 1.f, 0.f, 0.f,
                            0.f, 0.f, (1.f/(1.f+c)), -1.f,
                            0.f, 0.f, (-c/(1.f+c)), 0.f};

    // matrix for remapping z
    glm::mat4 remapZMat = {1.f, 0.f, 0.f, 0.f,
                           0.f, 1.f, 0.f, 0.f,
                           0.f, 0.f, -2.f, 0.f,
                           0.f, 0.f, -1.f, 1.f};

    // projection matrix
    return remapZMat*unhingeMat*scaleMat;
}

/**
 * @brief returns height angle of camera
 * @return
 */
float Camera::getHeightAngle() const {
    return cameraData.heightAngle;
}

/**
 * @brief set position of camera in world space
 * @return
 */
void Camera::setWorldPos() {
    glm::vec4 origin = {0.f, 0.f, 0.f, 1.f};
    camWorldPos = inverse(viewMatrix)*origin;
}

/**
 * @brief returns position of camera in world space
 * @return
 */
glm::vec3 Camera::getWorldPos() {
    return camWorldPos;
}

/**
 * @brief moves camera position forward along look vector
 * @param deltaTime
 */
void Camera::moveForward(float deltaTime) {
    cameraData.pos += normalize(cameraData.look)*5.f*deltaTime;
}

/**
 * @brief moves camera position backward along look vector
 * @param deltaTime
 */
void Camera::moveBackward(float deltaTime) {
    cameraData.pos -= normalize(cameraData.look)*5.f*deltaTime;
}

/**
 * @brief moves camera position to the left
 * @param deltaTime
 */
void Camera::moveLeft(float deltaTime) {
    glm::vec3 perpendicularVec = cross(glm::vec3(cameraData.up), glm::vec3(cameraData.look));
    perpendicularVec = normalize(perpendicularVec);
    cameraData.pos += glm::vec4(perpendicularVec*5.f*deltaTime, 0.f);
}

/**
 * @brief moves camera position to the right
 * @param deltaTime
 */
void Camera::moveRight(float deltaTime) {
    glm::vec3 perpendicularVec = cross(glm::vec3(cameraData.look), glm::vec3(cameraData.up));
    perpendicularVec = normalize(perpendicularVec);
    cameraData.pos += glm::vec4(perpendicularVec*5.f*deltaTime, 0.f);
}

/**
 * @brief moves camera up along y axis
 * @param deltaTime
 */
void Camera::moveUp(float deltaTime) {
    cameraData.pos.y += 5.f*deltaTime;
}

/**
 * @brief moves camera down along y axis
 * @param deltaTime
 */
void Camera::moveDown(float deltaTime) {
    cameraData.pos.y -= 5.f*deltaTime;
}

/**
 * @brief rotates camera about y axis by updating look vector
 * @param deltaX
 */
void Camera::rotateX(float deltaX) {
    cam_x_rotation += deltaX;
    float cosX = cos(deltaX);
    float sinX = sin(deltaX);

    glm::vec3 col1 = {cosX, 0.f, -sinX};
    glm::vec3 col2 = {0.f, 1.f, 0.f};
    glm::vec3 col3 = {sinX, 0.f, cosX};

    glm::mat3 rotationMat = {col1, col2, col3};

    cameraData.look = glm::vec4(rotationMat*cameraData.look, 1.f);
}

void Camera::incrementAngles(float x_angle, float y_angle) {
    cam_x_rotation += x_angle;
    cam_y_rotation += y_angle;
}

/**
 * @brief rotates camera about axis perpendicular to look and up vectors by updating look vector
 * @param deltaY
 */
void Camera::rotateY(float deltaY) {
    cam_y_rotation += deltaY;

    glm::vec3 axis = cross(glm::vec3(cameraData.look), glm::vec3(cameraData.up));

    float cosY = cos(deltaY);
    float sinY = sin(deltaY);

    glm::vec3 col1 = {cosY+axis.x*axis.x*(1-cosY), axis.x*axis.y*(1-cosY)+axis.z*sinY, axis.x*axis.z*(1-cosY)-axis.y*sinY};
    glm::vec3 col2 = {axis.x*axis.y*(1-cosY)-axis.z*sinY, cosY+axis.y*axis.y*(1-cosY), axis.y*axis.z*(1-cosY)+axis.x*sinY};
    glm::vec3 col3 = {axis.x*axis.z*(1-cosY)+axis.y*sinY, axis.y*axis.z*(1-cosY)-axis.x*sinY, cosY+axis.z*axis.z*(1-cosY)};

    glm::mat3 rotationMat = {col1, col2, col3};

    cameraData.look = glm::vec4(rotationMat*cameraData.look, 1.f);
}

float Camera::getAspectRatio() const {
    // Optional TODO: implement the getter or make your own design
    throw std::runtime_error("not implemented");
}

float Camera::getFocalLength() const {
    // Optional TODO: implement the getter or make your own design
    throw std::runtime_error("not implemented");
}

float Camera::getAperture() const {
    // Optional TODO: implement the getter or make your own design
    throw std::runtime_error("not implemented");
}

void Camera::updateCTMs(RenderShapeData &rsd) {
    glm::vec3 cam_translation = getWorldPos() - cam_start_position;
    glm::mat4 trans_mat = glm::mat4(1,0,0,0,
                                    0,1,0,0,
                                    0,0,1,0,
                                    cam_translation.x,cam_translation.y,cam_translation.z,1);

    // translate object with camera
    rsd.ctm = trans_mat * rsd.original_ctm;

    // glm::vec4 x_rot_axis = getViewMatrix() * glm::vec4(0,1,0,0);
    // glm::mat4 x_rotation_mat = getRotationAboutAxis(cam_x_rotation, x_rot_axis);
    // // glm::mat4 counter_rotation_mat = getRotationAboutAxis(-cam_x_rotation, glm::vec4(0,1,0,0));
    // glm::vec4 y_rot_axis = getViewMatrix() * glm::vec4(cross(glm::vec3(cameraData.look), glm::vec3(cameraData.up)),0);
    // glm::mat4 y_rotation_mat = getRotationAboutAxis(cam_y_rotation, y_rot_axis);

    // //put CTM into camera space, rotate, then put back into world space.
    // rsd.ctm = getInverseViewMatrix() * y_rotation_mat * x_rotation_mat * getViewMatrix() * rsd.ctm;
    glm::mat4 rotationMat = computeRotationMatrix(start_look, getLook());
    rsd.ctm = getInverseViewMatrix() * rotationMat * getViewMatrix() * rsd.ctm;

}

glm::mat4 Camera::computeRotationMatrix(glm::vec3 A, glm::vec3 B) {
    // rotation matrix from norm(A) to norm(B) [gets directions]
    glm::vec3 a = glm::normalize(A);
    glm::vec3 b = glm::normalize(B);
    glm::vec3 v = glm::cross(a, b);
    float cos = glm::dot(a, b);

    glm::mat3 res = glm::mat3(1.0);

    glm::mat3 skew = glm::mat3(
        0, v[2], -v[1],
        -v[2], 0, v[0],
        v[1], -v[0], 0);

    glm::mat3 mod_skew = res + skew +  (1.f /(1.f + cos)) * skew * skew;

    return glm::mat4(glm::vec4(mod_skew[0], 0), glm::vec4(mod_skew[1], 0), glm::vec4(mod_skew[2], 0), glm::vec4(0,0,0,1));
}

glm::mat4 Camera::getRotationAboutAxis(float angle, glm::vec4 axis) {
    float cosY = cos(angle);
    float sinY = sin(angle);

    glm::vec4 col1 = {cosY+axis.x*axis.x*(1-cosY), axis.x*axis.y*(1-cosY)+axis.z*sinY, axis.x*axis.z*(1-cosY)-axis.y*sinY, 0.f};
    glm::vec4 col2 = {axis.x*axis.y*(1-cosY)-axis.z*sinY, cosY+axis.y*axis.y*(1-cosY), axis.y*axis.z*(1-cosY)+axis.x*sinY, 0.f};
    glm::vec4 col3 = {axis.x*axis.z*(1-cosY)+axis.y*sinY, axis.y*axis.z*(1-cosY)-axis.x*sinY, cosY+axis.z*axis.z*(1-cosY), 0.f};

    glm::mat4 rotationMat = {col1, col2, col3, glm::vec4(0,0,0,1)};
    return rotationMat;
}

glm::vec3 Camera::getLook() {
    return glm::vec3(cameraData.look);
}

void Camera::setPosAndLook(glm::vec3 p, glm::vec3 l) {
    cameraData.pos = glm::vec4(p, 1.f);
    setWorldPos();
    cameraData.look = glm::vec4(l, 0.f);
}
