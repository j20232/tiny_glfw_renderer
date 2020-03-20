#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

namespace tiny_glfw_renderer {

// ============================== GUI ===================================
class Window {
public:
    Window(int width, int height, const char* title,
           GLFWmonitor* monitor = NULL, GLFWwindow* share = NULL);
    virtual ~Window();
    int ShouldClose() const;
    void SwapBuffers();

    GLfloat GetWidth() const;
    GLfloat GetHeight() const;
    GLfloat GetAspect() const;
    GLfloat GetScale() const;
    const GLfloat* GetLocation() const;

private:
    GLFWwindow* const m_window;
    GLfloat m_width;
    GLfloat m_height;
    GLfloat m_scale;
    GLfloat m_location[2];
    int m_key_status;
    static void Resize(GLFWwindow* const window, int width, int height);
    static void Wheel(GLFWwindow* const window, double x, double y);
    static void KeyBoard(GLFWwindow* const window, int key, int scancode,
                         int action, int mods);
};

// ============================= Matrix =================================
class Matrix {
public:
    Matrix() {}
    Matrix(const GLfloat* a);
    const GLfloat* Data() const;
    Matrix operator*(const Matrix& m) const;

    static Matrix Identity();
    static Matrix Translate(GLfloat x, GLfloat y, GLfloat z);
    static Matrix Scale(GLfloat x, GLfloat y, GLfloat z);
    static Matrix Rotate(GLfloat theta, GLfloat x, GLfloat y, GLfloat z);
    static Matrix LookAt(GLfloat ex, GLfloat ey, GLfloat ez,  // eye position
                         GLfloat gx, GLfloat gy, GLfloat gz,  // target position
                         GLfloat ux, GLfloat uy, GLfloat uz   // upper vector
    );
    static Matrix Orthogonal(GLfloat left, GLfloat right, GLfloat bottom,
                             GLfloat top, GLfloat z_near, GLfloat z_far);
    static Matrix Frustum(GLfloat left, GLfloat right, GLfloat bottom,
                          GLfloat top, GLfloat z_near, GLfloat z_far);
    static Matrix Perspective(GLfloat fovy, GLfloat aspect, GLfloat z_near,
                              GLfloat z_far);

private:
    GLfloat m_matrix[16];
    /*
     | 0  4  8 12 |
     | 1  5  9 13 |
     | 2  6 10 14 |
     | 3  7 11 15 |
    */
};

// ============================ Geometry ================================

template <int N>
struct Vec {
    GLfloat position[N];
};

using Vec2 = Vec<2>;
using Vec3 = Vec<3>;

template <int N>
class Object {
public:
    Object(GLint size, GLsizei vtx_cnt, const Vec<N>* vtx) {
        glGenVertexArrays(1, &m_vao);
        glBindVertexArray(m_vao);

        glGenBuffers(1, &m_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glBufferData(GL_ARRAY_BUFFER, vtx_cnt * sizeof(Vec<N>), vtx,
                     GL_STATIC_DRAW);

        glVertexAttribPointer(0, size, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(0);
    }
    virtual ~Object() {
        glDeleteBuffers(1, &m_vao);
        glDeleteBuffers(1, &m_vbo);
    }
    void bind() const { glBindVertexArray(m_vao); }

private:
    Object(const Object& o);
    Object& operator=(const Object& o);

    GLuint m_vao;
    GLuint m_vbo;
};

using Object2D = Object<2>;
using Object3D = Object<3>;

template <int N>
class Geometry {
public:
    Geometry(GLint size, GLsizei vtx_cnt, const Vec<N>* vtx)
        : m_obj(new Object<N>(size, vtx_cnt, vtx)), m_vtx_cnt(vtx_cnt){};

    void draw() const {
        m_obj->bind();
        execute();
    }

    virtual void execute() const { glDrawArrays(GL_LINE_LOOP, 0, m_vtx_cnt); }

private:
    std::shared_ptr<const Object<N>> m_obj;
    const GLsizei m_vtx_cnt;
};

using Geometry2D = Geometry<2>;
using Geometry3D = Geometry<3>;

// ============================= Primitive =================================

std::unique_ptr<const Geometry<2>> Rectangle(GLfloat x, GLfloat y, GLfloat w,
                                             GLfloat h) {
    const Vec2 rectangle_vtx[] = {
        {{x, y}}, {{x + w, y}}, {{x + w, y + h}}, {{x, y + h}}};
    std::unique_ptr<const Geometry<2>> shape(
        new Geometry<2>(2, 4, rectangle_vtx));
    return shape;
}

std::unique_ptr<const Geometry<3>> Octahedron(GLfloat s = 1.0f) {
    const Vec3 octahedron_vtx[] = {
        {{0.0f, s, 0.0f}},  {{-s, 0.0f, 0.0f}}, {{0.0f, -s, 0.0f}},
        {{s, 0.0f, 0.0f}},  {{0.0f, s, 0.0f}},  {{0.0f, 0.0f, s}},
        {{0.0f, -s, 0.0f}}, {{0.0f, 0.0f, -s}}, {{-s, 0.0f, 0.0f}},
        {{0.0f, 0.0f, s}},  {{s, 0.0f, 0.0f}},  {{0.0f, 0.0f, -s}}};
    std::unique_ptr<const Geometry<3>> shape(
        new Geometry<3>(3, 12, octahedron_vtx));
    return shape;
}

// ============================ Initializer ================================

inline void Initialize() {
    assert(glfwInit());
    atexit(glfwTerminate);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
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

// ============================== GUI ===================================
Window::Window(int width, int height, const char* title, GLFWmonitor* monitor,
               GLFWwindow* share)
    : m_window(glfwCreateWindow(width, height, title, monitor, share)),
      m_width(width),
      m_height(height),
      m_scale(100.0f),
      m_location{0.0f, 0.0f},
      m_key_status(GLFW_RELEASE) {
    if (m_window == NULL) {
        std::cerr << "Can't create GLFW window." << std::endl;
        exit(1);
    }
    glfwMakeContextCurrent(m_window);
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Can't initialize GLEW." << std::endl;
        exit(1);
    }
    glfwSwapInterval(1);
    glfwSetWindowUserPointer(m_window, this);
    glfwSetWindowSizeCallback(m_window, Resize);
    glfwSetScrollCallback(m_window, Wheel);
    glfwSetKeyCallback(m_window, KeyBoard);
    Resize(m_window, m_width, m_height);
    m_location[0] = m_location[1] = 0.0f;
}

Window::~Window() { glfwDestroyWindow(m_window); }

int Window::ShouldClose() const {
    return glfwWindowShouldClose(m_window) ||
           glfwGetKey(m_window, GLFW_KEY_ESCAPE);
}

void Window::SwapBuffers() {
    glfwSwapBuffers(m_window);
    if (m_key_status == GLFW_RELEASE) {
        glfwWaitEvents();
    } else {
        glfwPollEvents();
    }

    // Left or Right
    if (glfwGetKey(m_window, GLFW_KEY_LEFT) != GLFW_RELEASE) {
        m_location[0] -= 2.0f / m_width;
    } else if (glfwGetKey(m_window, GLFW_KEY_RIGHT) != GLFW_RELEASE) {
        m_location[0] += 2.0f / m_width;
    }

    // Down or Up
    if (glfwGetKey(m_window, GLFW_KEY_DOWN) != GLFW_RELEASE) {
        m_location[1] -= 2.0f / m_height;
    } else if (glfwGetKey(m_window, GLFW_KEY_UP) != GLFW_RELEASE) {
        m_location[1] += 2.0f / m_height;
    }

    // Mouse
    if (glfwGetMouseButton(m_window, GLFW_MOUSE_BUTTON_1) != GLFW_RELEASE) {
        double x, y;
        glfwGetCursorPos(m_window, &x, &y);
        m_location[0] = static_cast<GLfloat>(x) * 2.0f / m_width - 1.0f;
        m_location[1] = 1.0f - static_cast<GLfloat>(y) * 2.0f / m_height;
    }
}

void Window::Resize(GLFWwindow* const window, int width, int height) {
    glViewport(0, 0, width, height);
    Window* const instance(
        static_cast<Window*>(glfwGetWindowUserPointer(window)));
    if (instance != nullptr) {
        instance->m_width = width;
        instance->m_height = height;
    }
}

void Window::Wheel(GLFWwindow* const window, double x, double y) {
    Window* const instance(
        static_cast<Window*>(glfwGetWindowUserPointer(window)));
    if (instance != NULL) {
        instance->m_scale += static_cast<GLfloat>(y);
    }
}

void Window::KeyBoard(GLFWwindow* const window, int key, int scancode,
                      int action, int mods) {
    Window* const instance(
        static_cast<Window*>(glfwGetWindowUserPointer(window)));
    if (instance != NULL) {
        instance->m_key_status = action;
    }
}

GLfloat Window::GetWidth() const { return m_width; }
GLfloat Window::GetHeight() const { return m_height; }
GLfloat Window::GetAspect() const { return m_width / m_height; }
GLfloat Window::GetScale() const { return m_scale; }
const GLfloat* Window::GetLocation() const { return m_location; }

// ============================= Matrix =================================
Matrix::Matrix(const GLfloat* a) { std::copy(a, a + 16, m_matrix); }

const GLfloat* Matrix::Data() const { return m_matrix; }

Matrix Matrix::operator*(const Matrix& m) const {
    Matrix t;
    for (int i = 0; i < 16; i++) {
        const int j(i & 3), k(i & ~3);
        t.m_matrix[i] = m_matrix[0 + j] * m.m_matrix[k + 0] +
                        m_matrix[4 + j] * m.m_matrix[k + 1] +
                        m_matrix[8 + j] * m.m_matrix[k + 2] +
                        m_matrix[12 + j] * m.m_matrix[k + 3];
    }
    return t;
}

Matrix Matrix::Identity() {
    Matrix t;
    std::fill(t.m_matrix, t.m_matrix + 16, 0.0f);
    t.m_matrix[0] = t.m_matrix[5] = t.m_matrix[10] = t.m_matrix[15] = 1.0f;
    return t;
}

Matrix Matrix::Translate(GLfloat x, GLfloat y, GLfloat z) {
    Matrix t(Identity());
    t.m_matrix[12] = x;
    t.m_matrix[13] = y;
    t.m_matrix[14] = z;
    return t;
}

Matrix Matrix::Scale(GLfloat x, GLfloat y, GLfloat z) {
    Matrix t(Identity());
    t.m_matrix[0] = x;
    t.m_matrix[5] = y;
    t.m_matrix[10] = z;
    return t;
}

Matrix Matrix::Rotate(GLfloat theta, GLfloat x, GLfloat y, GLfloat z) {
    Matrix t(Identity());
    const GLfloat d(std::sqrt(x * x + y * y + z * z));
    if (d <= 0.0f) return t;

    // Rodrigues' rotation formula
    const GLfloat l(x / d), m(y / d), n(z / d);

    t.m_matrix[0] = l * l * (1 - std::cos(theta)) + std::cos(theta);
    t.m_matrix[1] = l * m * (1 - std::cos(theta)) + n * std::sin(theta);
    t.m_matrix[2] = l * n * (1 - std::cos(theta)) - m * std::sin(theta);

    t.m_matrix[4] = l * m * (1 - std::cos(theta)) - n * std::sin(theta);
    t.m_matrix[5] = m * m * (1 - std::cos(theta)) + std::cos(theta);
    t.m_matrix[6] = m * n * (1 - std::cos(theta)) + l * std::sin(theta);

    t.m_matrix[8] = l * n * (1 - std::cos(theta)) + m * std::sin(theta);
    t.m_matrix[9] = m * n * (1 - std::cos(theta)) - l * std::sin(theta);
    t.m_matrix[10] = n * n * (1 - std::cos(theta)) + std::cos(theta);

    return t;
}
Matrix Matrix::LookAt(GLfloat ex, GLfloat ey, GLfloat ez,  // eye position
                      GLfloat gx, GLfloat gy, GLfloat gz,  // target position
                      GLfloat ux, GLfloat uy, GLfloat uz   // upper vector
) {
    // translation
    const Matrix tv(Translate(-ex, -ey, -ez));

    // t = e - g
    const GLfloat tx(ex - gx);
    const GLfloat ty(ey - gy);
    const GLfloat tz(ez - gz);

    // r = u x t
    const GLfloat rx(uy * tz - uz * ty);
    const GLfloat ry(uz * tx - ux * tz);
    const GLfloat rz(ux * ty - uy * tx);

    // s = t x r
    const GLfloat sx(ty * rz - tz * ry);
    const GLfloat sy(tz * rx - tx * rz);
    const GLfloat sz(tx * ry - ty * rx);

    const GLfloat s2(sx * sx + sy * sy + sz * sz);
    if (s2 == 0.0f) return tv;

    Matrix rv(Identity());  // rotation

    const GLfloat r(std::sqrt(rx * rx + ry * ry + rz * rz));
    rv.m_matrix[0] = rx / r;
    rv.m_matrix[4] = ry / r;
    rv.m_matrix[8] = rz / r;

    const GLfloat s(std::sqrt(s2));
    rv.m_matrix[1] = sx / s;
    rv.m_matrix[5] = sy / s;
    rv.m_matrix[9] = sz / s;

    const GLfloat t(std::sqrt(tx * tx + ty * ty + tz * tz));
    rv.m_matrix[2] = tx / t;
    rv.m_matrix[6] = ty / t;
    rv.m_matrix[10] = tz / t;

    return rv * tv;
}

Matrix Matrix::Orthogonal(GLfloat left, GLfloat right, GLfloat bottom,
                          GLfloat top, GLfloat z_near, GLfloat z_far) {
    Matrix t(Identity());
    const GLfloat dx(right - left);
    const GLfloat dy(top - bottom);
    const GLfloat dz(z_far - z_near);

    if (dx != 0.0f && dy != 0.0f && dz != 0.0f) {
        t.m_matrix[0] = 2.0f / dx;
        t.m_matrix[5] = 2.0f / dy;
        t.m_matrix[10] = -2.0f / dz;
        t.m_matrix[12] = -(right + left) / dx;
        t.m_matrix[13] = -(top + bottom) / dy;
        t.m_matrix[14] = -(z_far + z_near) / dz;
    }

    return t;
}

Matrix Matrix::Frustum(GLfloat left, GLfloat right, GLfloat bottom, GLfloat top,
                       GLfloat z_near, GLfloat z_far) {
    Matrix t(Identity());
    const GLfloat dx(right - left);
    const GLfloat dy(top - bottom);
    const GLfloat dz(z_far - z_near);

    if (dx != 0.0f && dy != 0.0f && dz != 0.0f) {
        t.m_matrix[0] = 2.0f + z_near / dx;
        t.m_matrix[5] = 2.0f + z_near / dy;
        t.m_matrix[8] = (right + left) / dx;
        t.m_matrix[9] = (top + bottom) / dy;
        t.m_matrix[10] = -(z_far + z_near) / dz;
        t.m_matrix[11] = -1.0f;
        t.m_matrix[14] = -2.0f * z_far * z_near / dz;
        t.m_matrix[15] = 0.0f;
    }

    return t;
}

Matrix Matrix::Perspective(GLfloat fovy, GLfloat aspect, GLfloat z_near,
                           GLfloat z_far) {
    Matrix t(Identity());
    const GLfloat dz(z_far - z_near);

    if (dz != 0.0f) {
        float f = 1.0f / std::tan(fovy * 0.5f);
        t.m_matrix[0] = f / aspect;
        t.m_matrix[5] = f;
        t.m_matrix[10] = -(z_far + z_near) / dz;
        t.m_matrix[11] = -1.0f;
        t.m_matrix[14] = -2.0f * z_far * z_near / dz;
        t.m_matrix[15] = 0.0f;
    }

    return t;
}

}  // namespace tiny_glfw_renderer
