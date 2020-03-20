#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "tiny_glfw_renderer.h"

using namespace tiny_glfw_renderer;

const std::string SHADER_DIR = "../example/shaders/";
const std::string ASPECT_VERT = SHADER_DIR + "keep_aspect.vert";
const std::string FRAG = SHADER_DIR + "point.frag";
const Vec2 rectangle_vtx[] = {
    {-0.5f, -0.5f}, {0.5f, -0.5f}, {0.5f, 0.5f}, {-0.5f, 0.5f}};

int main() {
    Initialize();
    Window window(640, 480, "Test");

    const GLuint program(LoadProgram(ASPECT_VERT, FRAG));
    const GLint aspect_location(glGetUniformLocation(program, "aspect"));

    std::unique_ptr<const Geometry2D> shape(
        new Geometry2D(2, 4, rectangle_vtx));
    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
    while (window.ShouldClose() == GL_FALSE) {
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(program);
        glUniform1f(aspect_location, window.GetAspect());

        shape->draw();
        window.SwapBuffers();
    }
}
