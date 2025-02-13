#version 150 core

uniform float scale;
uniform float width;
uniform float height;
uniform vec2 location;
in vec4 position;

void main() {
    gl_Position =
        vec4(2.0 * scale / width, 2.0 * scale / height, 1.0, 1.0) * position +
        vec4(location, 0.0, 0.0);
}
