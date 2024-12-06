#version 330 core
layout(location=0) in vec3 obj_pos;
layout(location=1) in vec3 obj_normal;

out vec3 world_pos;
out vec3 world_normal;

uniform mat4 ctm;
uniform mat3 ctm_tinv;
uniform mat4 pv;



void main() {
    vec4 world_pos_4 = ctm * vec4(obj_pos, 1.0);
    world_pos = world_pos_4.xyz;
    world_normal = normalize(ctm_tinv * obj_normal);

    gl_Position = pv * ctm * vec4(obj_pos, 1.0);
}
