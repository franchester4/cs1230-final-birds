#version 330 core

in vec3 world_pos;
in vec3 world_normal;

out vec4 fragColor;

uniform int lightType[8];
uniform vec4 lightColor[8];
uniform vec4 lightPos[8];
uniform vec4 lightDir[8];
uniform float lightPenumbra[8];
uniform float lightAngle[8];

uniform vec3 attn[8];

uniform int numLights;

uniform float ka;
uniform float kd;
uniform float ks;

uniform vec4 ca;
uniform vec4 cd;
uniform vec4 cs;
uniform int cn;

const int diffuse_color_levels = 4;
const float diffuse_scale_factor = 1.f/diffuse_color_levels;

const int specular_color_levels = 2;
const float specular_scale_factor = 1.f / specular_color_levels;

uniform vec4 cam;
void main() {
    vec3 normalized_wnorm = normalize(world_normal);
    vec3 camDir = normalize(cam.xyz - world_pos);
    // vec3 camDir = camDirection4.xyz;

    // ambient
    fragColor = vec4(0.0);
    fragColor[3] = 1.0;
    for (int i = 0; i < 3; i++) {
        // ambient
        fragColor[i] += ka * ca[i];
    }

    for (int i = 0; i < numLights; i++) {
        // l_i
        vec3 l_i = vec3(0.0);
        if (lightType[i] == 1) { // directional
            l_i = normalize(-1.f * lightDir[i].xyz);
        }
        else {
            l_i = normalize(lightPos[i].xyz - world_pos);
        }

        // attn
        float attn_val = 1.0f;
        if (lightType[i] != 1) { // spot or point
            float distance = 0;
            for (int j = 0; j < 3; j++) {
                distance += pow(lightPos[i][j] - world_pos[j], 2);
            }
            distance = sqrt(distance);
            float denom = attn[i][0] + distance * attn[i][1] + pow(distance, 2) * attn[i][2];
            attn_val = min(1.0f, 1.0f / denom);
        }

        // spot
        float spot = 1.0f;
        if (lightType[i] == 2) { // spot
            float x = acos(dot(normalize(lightDir[i].xyz), normalize(-l_i)));
            float inner = lightAngle[i] - lightPenumbra[i];
            if (x <= inner) {
                spot = 1.0f;
            }
            else if (x > lightAngle[i]) {
                spot = 0.0f;
            }
            else {
                float delta = x - inner;
                float falloff = -2.0 * pow(delta / lightPenumbra[i], 3) + 3 * pow(delta / lightPenumbra[i], 2);
                spot = (1.0f - falloff);
            }
        }
        float dot_li_normal = dot(normalized_wnorm, l_i);
        dot_li_normal = clamp(dot_li_normal, 0.f, 1.f);
        if (dot_li_normal > 0.0) {
            vec3 r_i = reflect(-1.f * l_i, normalized_wnorm);
            float dot_ri_cam = dot(r_i, camDir);
            dot_ri_cam = clamp(dot_ri_cam, 0.f, 1.f);
            for (int j = 0; j < 3; j++) {
                // diffuse + specular

                // for discretizing diffuse color
                float diffuseFactor = ceil(dot_li_normal*diffuse_color_levels) * diffuse_scale_factor;
                float diffuse = cd[j] * kd * dot_li_normal * diffuseFactor;

                float powpow = (cn <= 0) ? 1.f : pow(dot_ri_cam, cn);
                // for discretizing specular color
                float specularFactor = ceil(powpow*specular_color_levels) * specular_scale_factor;
                float specular = ks * cs[j] * specularFactor;
                fragColor[j] += attn_val * spot * lightColor[i][j] * (diffuse + specular);
            }
        }
    }
}
