#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <cassert>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
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
    if (PrintShaderInfoLog(vobj, "vertex shader"))
        glAttachShader(program, vobj);
    glDeleteShader(vobj);

    // Compile fragment shader
    const GLuint fobj(glCreateShader(GL_FRAGMENT_SHADER));
    glShaderSource(fobj, 1, &fsrc, NULL);
    glCompileShader(fobj);
    if (PrintShaderInfoLog(fobj, "fragment shader"))
        glAttachShader(program, fobj);
    glDeleteShader(fobj);

    // Link shaders
    glBindAttribLocation(program, 0, "position");
    glBindFragDataLocation(program, 0, "fragment");
    glLinkProgram(program);
    if (PrintProgramInfoLog(program)) return program;
    glDeleteProgram(program);
    return 0;
}

inline GLboolean ReadShaderSource(const std::string name,
                                  std::vector<GLchar>& buffer) {
    std::ifstream file(name, std::ios::binary);
    if (file.fail()) {
        std::cerr << "Error: Can't open " << name << std::endl;
        return false;
    }

    // Allocate memory
    file.seekg(0L, std::ios::end);
    GLsizei length = static_cast<GLsizei>(file.tellg());
    buffer.resize(length + 1);

    // Open file
    file.seekg(0L, std::ios::beg);
    file.read(buffer.data(), length);
    buffer[length] = '\0';

    if (file.fail()) {
        std::cerr << "Error: Could not read " << name << std::endl;
        file.close();
        return false;
    }

    file.close();
    return true;
}

inline GLuint LoadProgram(const std::string vert_shader_file,
                          const std::string frag_shader_file) {
    std::vector<GLchar> vsrc, fsrc;
    const bool vstat(ReadShaderSource(vert_shader_file, vsrc));
    const bool fstat(ReadShaderSource(frag_shader_file, fsrc));
    return vstat && fstat ? CreateProgram(vsrc.data(), fsrc.data()) : 0;
}

}  // namespace tiny_renderer
