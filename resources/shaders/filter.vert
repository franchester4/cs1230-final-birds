#version 330 core

// Task 15: add a second layout variable representing a UV coordinate
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 uv_coords;

// Task 16: create an "out" variable representing a UV coordinate
out vec3 uv;

void main() {
    // Task 16: assign the UV layout variable to the UV "out" variable

    gl_Position = vec4(position, 1.0);

    uv = uv_coords;
}
