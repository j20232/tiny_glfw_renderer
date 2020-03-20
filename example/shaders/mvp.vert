#version 150 core

uniform mat4 model;
uniform mat4 view;
in vec4 position;

void main() { gl_Position = view * model * position; }
