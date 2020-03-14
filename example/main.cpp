#include <GLFW/glfw3.h>

#include <cassert>
#include <memory>

int main() {
    assert(glfwInit());
    GLFWwindow *const window(glfwCreateWindow(640, 480, "Test", NULL, NULL));
    if (window == nullptr) glfwTerminate();
    glfwMakeContextCurrent(window);
    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
    while (glfwWindowShouldClose(window) == GL_FALSE) {
        glClear(GL_COLOR_BUFFER_BIT);
        glfwSwapBuffers(window);
        glfwWaitEvents();
    }
}
