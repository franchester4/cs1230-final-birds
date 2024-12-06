#version 330 core

in vec3 uv;
uniform sampler2D texture_sampler;
uniform bool pixel_flag;
uniform bool kernel_flag;

uniform int screen_w;
uniform int screen_h;

out vec4 fragColor;

void main()
{
    fragColor = vec4(1);
    vec2 P = vec2(uv.x, uv.y);
    fragColor = vec4(texture(texture_sampler, P));

    if (kernel_flag) {
        // do things
        vec4 newFragColor = vec4(0.0f);
        for (int i = -1; i <= 1; i++) {
            for (int j = -1; j <= 1; j++) {
                // center is uv
                vec2 delta = vec2(i * 1.0f / (screen_w * 1.f), j * 1.0f / (screen_h * 1.f));
                newFragColor += -1.f / 9.f * vec4(texture(texture_sampler, delta + P));
                if (i == 0 && j == 0) {
                    newFragColor += 2 * vec4(texture(texture_sampler, delta + P));
                }
            }
        }
        fragColor = newFragColor;
    }
    if (pixel_flag) {
        fragColor = vec4(1.0) - fragColor;
    }
}
