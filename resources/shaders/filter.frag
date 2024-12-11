#version 330 core

in vec2 uvcoords;

uniform sampler2D textureUniform;

uniform bool isPixelFilter;
uniform bool isKernelFilter;
uniform bool isBlurFilter;
uniform bool isGrayscaleFilter;

uniform bool isSobelFilter;

uniform float kernel[9];
uniform float fboHeight;
uniform float fboWidth;

uniform float sobelXKernel[9];
uniform float sobelYKernel[9];

out vec4 fragColor;


vec4 sharpenFilter() {

    vec4 sum = vec4(0.0);

    for (int c = -1; c < 2; c++) {
        float u = uvcoords[0] + c/fboWidth;
        u = clamp(u, 0.f, 1.f);

        for (int r = -1; r < 2; r++) {

            float v = uvcoords[1] + r/fboHeight;
            v = clamp(v, 0.f, 1.f);

            int kernelIndex = (r+1) * 3 + (c+1);
            sum += kernel[kernelIndex]*texture(textureUniform, vec2(u, v));
        }
    }
    return sum;
}

vec4 blurFilter() {

    vec4 sum = vec4(0.0);

    for (int c = -2; c < 3; c++) {
        float u = uvcoords[0] + c/fboWidth;
        u = clamp(u, 0.f, 1.f);

        for (int r = -2; r < 3; r++) {

            float v = uvcoords[1] + r/fboHeight;
            v = clamp(v, 0.f, 1.f);

            sum += texture(textureUniform, vec2(u, v));
        }
    }
    return sum/25.f;
}

vec4 grayscale(vec4 color) {
    float intensity = color.r*0.299+color.g*0.587+color.b*0.114;
    intensity = clamp(intensity, 0.f, 1.f);
    return vec4(intensity, intensity, intensity, 1.f);
}

vec4 sobelConvolve(float[9] sobelKernel) {
    vec4 sum = vec4(0.0);

    for (int c = -1; c < 2; c++) {
        float u = uvcoords[0] + c/fboWidth;
        u = clamp(u, 0.f, 1.f);

        for (int r = -1; r < 2; r++) {

            float v = uvcoords[1] + r/fboHeight;
            v = clamp(v, 0.f, 1.f);

            int kernelIndex = (r+1) * 3 + (c+1);
            sum += sobelKernel[kernelIndex]*texture(textureUniform, vec2(u, v));
        }
    }
    return sum;
}

void main()
{

    // sharpen if kernel filter selected
    if (isKernelFilter) {
        fragColor = sharpenFilter();
    } else {
        // set fragColor using the sampler2D at the UV coordinate
        fragColor = texture(textureUniform, uvcoords);
    }

    // blur if extra credit 1 is selected
    if (isBlurFilter) {
        fragColor = blurFilter();
    }

    // invert color channels if pixel filter selected
    if (isPixelFilter) {
        fragColor = 1 - texture(textureUniform, uvcoords);
    }

    // turn to grayscale if extra credit 2 is selected
    if (isGrayscaleFilter) {
        fragColor = grayscale(fragColor);
    }

    if (isSobelFilter) {
        vec4 sobelEdgeX = sobelConvolve(sobelXKernel);
        vec4 sobelEdgeY = sobelConvolve(sobelYKernel);

        float gRValue = sqrt(sobelEdgeX.r*sobelEdgeX.r + sobelEdgeY.r*sobelEdgeY.r);
        float gGValue = sqrt(sobelEdgeX.g*sobelEdgeX.g + sobelEdgeY.g*sobelEdgeY.g);
        float gBValue = sqrt(sobelEdgeX.b*sobelEdgeX.b + sobelEdgeY.b*sobelEdgeY.b);

        gRValue = clamp(gRValue, 0.f, 1.f);
        gGValue = clamp(gGValue, 0.f, 1.f);
        gBValue = clamp(gBValue, 0.f, 1.f);

        //vec3 sobel = sqrt(vec3(sobelEdgeX)*vec3(sobelEdgeX) + vec3(sobelEdgeY)*vec3(sobelEdgeY));
        vec4 sobel = vec4(gRValue, gGValue, gBValue, 1.f);
        sobel = grayscale(sobel);

        float edgeThreshold = 0.4;

        if (sobel.r > edgeThreshold) {
            // add black outline
                fragColor = vec4(0.0, 0.0, 0.0, 1.0);
            } else {
                fragColor = texture(textureUniform, uvcoords);
        }
    }
}

