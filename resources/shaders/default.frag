#version 330 core

// "in" variables for the world-space position and normal, received post-interpolation from the vertex shader
in vec3 worldPos;
in vec3 worldNorm;

out vec4 fragColor;

uniform float k_a;

uniform float k_d;

uniform float k_s;
uniform float shininess;

uniform vec3 material_a;
uniform vec3 material_d;
uniform vec3 material_s;

uniform vec4 cameraPos;

uniform vec3 lightDirections[8];
uniform vec3 lightPositions[8];
uniform vec3 lightColors[8];
uniform int lightTypes[8];
uniform vec3 lightFunctions[8];
uniform float lightAngles[8];
uniform float lightPenumbras[8];

uniform float numLights;

const int diffuse_color_levels = 4;
const float diffuse_scale_factor = 1.f/diffuse_color_levels;

const int specular_color_levels = 2;
const float specular_scale_factor = 1.f / specular_color_levels;

void main() {

    vec3 norm = normalize(worldNorm);

    vec3 dirToLight;

    float attenuation;

    fragColor = vec4(0.0);

    // add ambient
    fragColor += vec4(k_a*material_a, 1.f);

    for (int i = 0; i < numLights; i++) {

        vec4 lighting = vec4(0.0);

        // for directional lights
        if (lightTypes[i] == 1) {
            dirToLight = -normalize(lightDirections[i]);
            attenuation = 1.f;
        } else {
            // for point and spot lights
            dirToLight = normalize(lightPositions[i]-worldPos);
            float distance = length((lightPositions[i]-worldPos));
            attenuation = min(1.f, 1.f/(lightFunctions[i].x+lightFunctions[i].y*distance+lightFunctions[i].z*distance*distance));
        }

        // add diffuse
        // for discretizing diffuse color
        float diffuseFactor = ceil(clamp(dot(norm, dirToLight), 0.f, 1.f)*diffuse_color_levels) * diffuse_scale_factor;

        lighting += vec4(attenuation*lightColors[i]*k_d*material_d*diffuseFactor, 1.f);

        if (shininess != 0) {
            vec3 reflectedVec = normalize(reflect(-dirToLight, norm));
            vec3 dirToCam = normalize(vec3(cameraPos) - worldPos);

            // add specular
            float rawSpecular = pow(clamp(dot(reflectedVec, dirToCam), 0.f, 1.f), shininess);
            // for discretizing specular color
            float specularFactor = ceil(rawSpecular*specular_color_levels) * specular_scale_factor;

            lighting += vec4(attenuation*lightColors[i]*k_s*material_s*specularFactor, 1.f);
        }

        // if spot light, calculate and multiply by fall off
        if (lightTypes[i] == 2) {

            float falloff;

            vec3 normLightDir = normalize(lightDirections[i]);
            float x = acos(dot(normLightDir, -dirToLight));

            float outer = lightAngles[i];
            float inner = outer - lightPenumbras[i];

            if (x <= inner) {
                falloff = 1.f;
            } else if (x > inner && x <= outer) {
                float penumbraRatio = (x-inner)/(outer-inner);
                falloff = 1.f - (-2.f*penumbraRatio*penumbraRatio*penumbraRatio + 3.f*penumbraRatio*penumbraRatio);
            } else {
                falloff = 0.f;
            }

            fragColor += lighting*falloff;

        } else {
            fragColor += lighting;
        }
    }
}
