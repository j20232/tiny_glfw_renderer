#version 150 core

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 normal_mat;

const vec4 Lpos = vec4(0.0, 0.0, 5.0, 1.0);

// diffuse
const vec3 Ldiff = vec3(1.0);
const vec3 Kdiff = vec3(0.6, 0.6, 0.2);

// specular
const vec3 Lspec = vec3(1.0);
const vec3 Kspec = vec3(0.3, 0.3, 0.3);
const float Kshi = 30.0;

// ambient
const vec3 Lamb = vec3(0.2);
const vec3 Kamb = vec3(0.6, 0.6, 0.2);

in vec4 position;
in vec3 normal;
out vec3 Idiff;
out vec3 Ispec;

void main() {
    vec4 P = view * model * position;
    vec3 N = normalize(normal_mat * normal);

    // ambient
    vec3 Iamb = Kamb * Lamb;

    // diffuse
    vec3 L = normalize((Lpos * P.w - P * Lpos.w).xyz);
    Idiff = max(dot(N, L), 0.0) * Kdiff * Ldiff + Iamb;

    // specular
    vec3 V = -normalize(P.xyz);
    vec3 H = normalize(L + V);
    Ispec = pow(max(dot(N, H), 0.0), Kshi) * Kspec * Lspec;

    gl_Position = projection * P;
}
