#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "tiny_renderer.h"

using namespace tiny_renderer;

const std::string SHADER_DIR = "../example/shaders/";
const std::string SCALE_VERT = SHADER_DIR + "keep_scale.vert";
const std::string FRAG = SHADER_DIR + "point.frag";
const Vec2 rectangle_vtx[] = {
    {-0.5f, -0.5f}, {0.5f, -0.5f}, {0.5f, 0.5f}, {-0.5f, 0.5f}};

int main() {
    Initialize();
    Window window(640, 480, "Test");

    const GLuint program(LoadProgram(SCALE_VERT, FRAG));
    const GLint aspect_width(glGetUniformLocation(program, "width"));
    const GLint aspect_height(glGetUniformLocation(program, "height"));
    const GLint aspect_scale(glGetUniformLocation(program, "scale"));

    std::unique_ptr<const Geometry2D> shape(
        new Geometry2D(2, 4, rectangle_vtx));
    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
    while (window.ShouldClose() == GL_FALSE) {
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(program);
        glUniform1f(aspect_width, window.GetWidth());
        glUniform1f(aspect_height, window.GetHeight());
        glUniform1f(aspect_scale, window.GetScale());

        shape->draw();
        window.SwapBuffers();
    }
}
