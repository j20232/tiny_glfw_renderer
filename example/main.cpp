#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "tiny_renderer.h"

int main() {
    tiny_renderer::Initialize();
    auto window = tiny_renderer::CreateWindow(640, 480, "Test");

    static const GLchar vsrc[] =
        "#version 150 core\n"
        "in vec4 position;\n"
        "void main(){\n"
        "  gl_Position = position;\n"
        "}\n";

    static const GLchar fsrc[] =
        "#version 150 core\n"
        "out vec4 fragment;\n"
        "void main()\n"
        "  fragment = vec4(1.0, 0.0, 0.0, 1.0);\n"
        "}\n";

    const GLuint program(tiny_renderer::CreateProgram(vsrc, fsrc));
    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
    while (glfwWindowShouldClose(window) == GL_FALSE) {
        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(program);
        glfwSwapBuffers(window);
        glfwWaitEvents();
    }
}
