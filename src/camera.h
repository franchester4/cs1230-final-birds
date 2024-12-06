#pragma once

#include "utils/scenedata.h"
#include <glm/glm.hpp>

// A class representing a virtual camera.

// Feel free to make your own design choices for Camera class, the functions below are all optional / for your convenience.
// You can either implement and use these getters, or make your own design.
// If you decide to make your own design, feel free to delete these as TAs won't rely on them to grade your assignments.

class Camera {
public:
    void setCamera(SceneCameraData cam, float near_, float far_, int width, int height);
    glm::vec4 pos;
    glm::vec4 look;
    glm::vec4 up;

    glm::mat4 viewMatrix; //world to cam
    glm::mat4 projMatrix; // cam to clip
    glm::mat4 pv;

    // can update this
    float near;
    float far;

    float heightAngle; // The height angle of the camera in RADIANS

    float aperture;    // Only applicable for depth of field
    float focalLength; // Only applicable for depth of field

    float aspect; // w / h

    int w;
    int h;

    void setViewMatrix();
    void setProjMatrix();
    void setPV();
    void updateNearFar(float near_, float far_);

};
