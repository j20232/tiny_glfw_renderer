#version 150 core

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 normal_mat;
in vec4 position;
in vec3 normal;
out vec3 Idiff;
out vec4 P;
out vec3 N;

void main() {
    P = view * model * position;
    N = normalize(normal_mat * normal);
    gl_Position = projection * P;
}
