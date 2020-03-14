#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <cassert>
#include <cstdlib>
#include <iostream>
#include <vector>
namespace tiny_renderer {

inline void Initialize() {
    assert(glfwInit());
    atexit(glfwTerminate);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

inline GLFWwindow* CreateWindow(int width, int height, const char* title,
                                GLFWmonitor* monitor = NULL,
                                GLFWwindow* share = NULL) {
    GLFWwindow* window(glfwCreateWindow(width, height, title, monitor, share));
    glfwMakeContextCurrent(window);
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) return nullptr;
    glfwSwapInterval(1);
    return window;
}

inline GLuint CreateProgram(const char* vsrc, const char* fsrc) {
    const GLuint program(glCreateProgram());
    if (vsrc == nullptr || fsrc == nullptr) {
        std::cout << "Incompatible shader source." << std::endl;
        exit(1);
    }

    // Compile vertex shader
    const GLuint vobj(glCreateShader(GL_VERTEX_SHADER));
    glShaderSource(vobj, 1, &vsrc, NULL);
    glCompileShader(vobj);
    glAttachShader(program, vobj);
    glDeleteShader(vobj);

    // Compile fragment shader
    const GLuint fobj(glCreateShader(GL_FRAGMENT_SHADER));
    glShaderSource(fobj, 1, &fsrc, NULL);
    glCompileShader(fobj);
    glAttachShader(program, fobj);
    glDeleteShader(fobj);

    // Link shaders
    glBindAttribLocation(program, 0, "position");
    glBindFragDataLocation(program, 0, "fragment");
    glLinkProgram(program);
    return program;
}

inline GLboolean PrintShaderInfoLog(GLuint shader, const char* str) {
    // Get compile outputs
    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE) std::cerr << "Error in " << str << std::endl;

    // Get length of compile logs
    GLsizei buf_size;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &buf_size);

    // Show logs
    if (buf_size > 1) {
        std::vector<GLchar> info_log(buf_size);
        GLsizei length;
        glGetShaderInfoLog(shader, buf_size, &length, &info_log[0]);
        std::cerr << &info_log[0] << std::endl;
    }
    return static_cast<GLboolean>(status);
}

inline GLboolean PrintProgramInfoLog(GLuint program) {
    GLint status;
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if (status == GL_FALSE) std::cerr << "Link Error" << std::endl;

    // Get length of link logs
    GLsizei buf_size;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &buf_size);

    // Show logs
    if (buf_size > 1) {
        std::vector<GLchar> info_log(buf_size);
        GLsizei length;
        glGetProgramInfoLog(program, buf_size, &length, &info_log[0]);
        std::cerr << &info_log[0] << std::endl;
    }
    return static_cast<GLboolean>(status);
}

}  // namespace tiny_renderer
