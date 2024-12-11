#version 330 core

// declare a vec3 object-space position variable and normal variable
layout(location = 0) in vec3 objPos;
layout(location = 1) in vec3 objNorm;

// `out` variables for the world-space position and normal, to be passed to the fragment shader
out vec3 worldPos;
out vec3 worldNorm;

uniform mat4 m_model;
uniform mat4 m_view;
uniform mat4 m_proj;
uniform mat3 invTransMat;
uniform mat4 mvp;

void main() {
    // compute the world-space position and normal to be passed to the fragment shader
    worldPos = vec3(m_model*vec4(objPos, 1.0));
    worldNorm = invTransMat*objNorm;

    // set gl_Position to the object space position transformed to clip space
    gl_Position = mvp*vec4(objPos, 1.0);
}
