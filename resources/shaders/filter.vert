#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 uvcoord;

// "out" variable representing a UV coordinate
out vec2 uvcoords;

void main() {

    gl_Position = vec4(position, 1.0);

    uvcoords = vec2(uvcoord);
}
