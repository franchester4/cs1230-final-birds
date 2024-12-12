#pragma once

#include <glm/glm.hpp>

#include <utils/sceneparser.h>

#include "utils/scenedata.h"

// A class representing a virtual camera.

// Feel free to make your own design choices for Camera class, the functions below are all optional / for your convenience.
// You can either implement and use these getters, or make your own design.
// If you decide to make your own design, feel free to delete these as TAs won't rely on them to grade your assignments.

class Camera {

private:
    SceneCameraData cameraData;

public:

    Camera();

    void setViewMatrix();
    void setWorldPos();
    void setInverseViewMatrix();

    Camera(const SceneCameraData &data);

    // Returns the view matrix for the current camera settings.
    // You might also want to define another function that return the inverse of the view matrix.
    glm::mat4 getViewMatrix() const;

    glm::mat4 getInverseViewMatrix() const;

    // Returns the aspect ratio of the camera.
    float getAspectRatio() const;

    // Returns the height angle of the camera in RADIANS.
    float getHeightAngle() const;

    // Returns the focal length of this camera.
    // This is for the depth of field extra-credit feature only;
    // You can ignore if you are not attempting to implement depth of field.
    float getFocalLength() const;

    // Returns the focal length of this camera.
    // This is for the depth of field extra-credit feature only;
    // You can ignore if you are not attempting to implement depth of field.
    float getAperture() const;

    glm::mat4 setProjectionMatrix(float aspectRatio, float far, float near);

    glm::vec3 getWorldPos();

    void moveForward(float deltaTime);
    void moveBackward(float deltaTime);
    void moveLeft(float deltaTime);
    void moveRight(float deltaTime);
    void moveUp(float deltaTime);
    void moveDown(float deltaTime);
    void rotateX(float deltaX);
    void rotateY(float deltaY);

    glm::vec3 getLook();
    void setPosAndLook(glm::vec3 p, glm::vec3 l);

    glm::vec3 cam_start_position;
    float cam_x_rotation;
    float cam_y_rotation;
    void updateCTMs(RenderShapeData& rsd);
    glm::mat4 getRotationAboutAxis(float angle, glm::vec4 axis);

};

