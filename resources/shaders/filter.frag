#version 330 core

in vec3 uv;
uniform sampler2D texture_sampler;
uniform bool pixel_flag;
uniform bool kernel_flag;

uniform int screen_w;
uniform int screen_h;

uniform bool outline_flag;

uniform float xKernel[9];
uniform float yKernel[9];
uniform float simpleKernel[9];

out vec4 fragColor;

vec4 grayscale(vec4 color) {
    float intensity = color.r*0.299+color.g*0.587+color.b*0.114;
    intensity = clamp(intensity, 0.f, 1.f);
    return vec4(intensity, intensity, intensity, 1.f);
}

vec4 sobelConvolve(float[9] kernel) {
    vec4 sum = vec4(0.0);

    for (int c = -1; c < 2; c++) {
        float u = uv[0] + c/(1.f * screen_w);
        u = clamp(u, 0.f, 1.f);

        for (int r = -1; r < 2; r++) {

            float v = uv[1] + r/(1.f * screen_h);
            v = clamp(v, 0.f, 1.f);

            int kernelIndex = (r+1) * 3 + (c+1);
            sum += vec4(kernel[kernelIndex]) *texture(texture_sampler, vec2(u, v));
        }
    }
    return sum;
}

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

    if (outline_flag) {

        vec4 sobelEdgeX = sobelConvolve(xKernel);
        vec4 sobelEdgeY = sobelConvolve(yKernel);

        float gRValue = sqrt(sobelEdgeX.r*sobelEdgeX.r + sobelEdgeY.r*sobelEdgeY.r);
        float gGValue = sqrt(sobelEdgeX.g*sobelEdgeX.g + sobelEdgeY.g*sobelEdgeY.g);
        float gBValue = sqrt(sobelEdgeX.b*sobelEdgeX.b + sobelEdgeY.b*sobelEdgeY.b);

        gRValue = clamp(gRValue, 0.f, 1.f);
        gGValue = clamp(gGValue, 0.f, 1.f);
        gBValue = clamp(gBValue, 0.f, 1.f);

        vec4 sobel = vec4(gRValue, gGValue, gBValue, 1.f);
        fragColor = grayscale(sobel);

         float intensityThreshold = 0.4;

         if (sobel.r > intensityThreshold) {
             // add black outline if above intensity level
             fragColor = vec4(0.0, 0.0, 0.0, 1.0);
             } else {
             fragColor = texture(texture_sampler, P);
         }
    }
    if (pixel_flag) {
        fragColor = vec4(1.0) - fragColor;
    }
}
