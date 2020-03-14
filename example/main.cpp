#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "tiny_renderer.h"

const std::string SHADER_DIR = "../example/shaders/";
const std::string VERT = SHADER_DIR + "point.vert";
const std::string FRAG = SHADER_DIR + "point.frag";

int main() {
    tiny_renderer::Initialize();
    auto window = tiny_renderer::CreateWindow(640, 480, "Test");
    const GLuint program(tiny_renderer::LoadProgram(VERT, FRAG));
    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
    while (glfwWindowShouldClose(window) == GL_FALSE) {
        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(program);
        glfwSwapBuffers(window);
        glfwWaitEvents();
    }
}
