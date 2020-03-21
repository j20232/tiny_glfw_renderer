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
    static void Resize(GLFWwindow* const window, int width, int height);
    static void Wheel(GLFWwindow* const window, double x, double y);
};

// ============================= Matrix =================================
class Matrix {
public:
    Matrix() {}
    Matrix(const GLfloat* a);
    const GLfloat* Data() const;
    Matrix operator*(const Matrix& m) const;
    void GetNormalMatrix(GLfloat* m) const;

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
    GLfloat normal[N];
};

using Vec2 = Vec<2>;
using Vec3 = Vec<3>;

template <int N>
class Object {
public:
    Object(GLint size, GLsizei vtx_cnt, const Vec<N>* vtx, GLsizei idx_cnt = 0,
           const GLuint* idx = nullptr) {
        // vertex array object
        glGenVertexArrays(1, &m_vao);
        glBindVertexArray(m_vao);

        // vertex buffer object
        glGenBuffers(1, &m_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glBufferData(GL_ARRAY_BUFFER, vtx_cnt * sizeof(Vec<N>), vtx,
                     GL_STATIC_DRAW);

        glVertexAttribPointer(0, size, GL_FLOAT, GL_FALSE, sizeof(Vec<N>), 0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vec<N>),
                              static_cast<char*>(0) + sizeof vtx->position);
        glEnableVertexAttribArray(1);

        // index buffer object
        glGenBuffers(1, &m_ibo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, idx_cnt * sizeof(GLuint), idx,
                     GL_STATIC_DRAW);
    }

    virtual ~Object() {
        glDeleteBuffers(1, &m_vao);
        glDeleteBuffers(1, &m_vbo);
        glDeleteBuffers(1, &m_ibo);
    }

    void bind() const { glBindVertexArray(m_vao); }

private:
    Object(const Object& o);
    Object& operator=(const Object& o);

    GLuint m_vao;
    GLuint m_vbo;
    GLuint m_ibo;
};

using Object2D = Object<2>;
using Object3D = Object<3>;

template <int N>
class Geometry {
public:
    Geometry(GLint size, GLsizei vtx_cnt, const Vec<N>* vtx,
             GLsizei idx_cnt = 0, const GLuint* idx = nullptr)
        : m_obj(new Object<N>(size, vtx_cnt, vtx, idx_cnt, idx)),
          m_vtx_cnt(vtx_cnt) {}

    void draw(GLenum mode = GL_LINE_LOOP) const {
        m_obj->bind();
        execute(mode);
    }

    virtual void execute(GLenum mode = GL_LINE_LOOP) const {
        glDrawArrays(mode, 0, m_vtx_cnt);
    }

private:
    std::shared_ptr<const Object<N>> m_obj;
    const GLsizei m_vtx_cnt;
};

using Geometry2D = Geometry<2>;
using Geometry3D = Geometry<3>;

template <int N>
class GeometryIndex : public Geometry<N> {
public:
    GeometryIndex(GLint size, GLsizei vtx_cnt, const Vec<N>* vtx,
                  GLsizei idx_cnt = 0, const GLuint* idx = nullptr)
        : Geometry<N>(size, vtx_cnt, vtx, idx_cnt, idx), m_idx_cnt(idx_cnt) {}

    virtual void execute(GLenum mode = GL_LINES) const {
        glDrawElements(mode, m_idx_cnt, GL_UNSIGNED_INT, 0);
    }

protected:
    const GLsizei m_idx_cnt;
};

using GeometryIndex2D = GeometryIndex<2>;
using GeometryIndex3D = GeometryIndex<3>;

// ============================= Primitive =================================

std::unique_ptr<const Geometry2D> Rectangle(GLfloat x, GLfloat y, GLfloat w,
                                            GLfloat h) {
    const Vec2 rectangle_vtx[] = {
        {{x, y}}, {{x + w, y}}, {{x + w, y + h}}, {{x, y + h}}};
    std::unique_ptr<const Geometry2D> shape(
        new Geometry2D(2, 4, rectangle_vtx));
    return shape;
}

std::unique_ptr<const Geometry3D> Octahedron(GLfloat s = 1.0f) {
    const Vec3 octahedron_vtx[] = {
        {{0.0f, s, 0.0f}},  {{-s, 0.0f, 0.0f}}, {{0.0f, -s, 0.0f}},
        {{s, 0.0f, 0.0f}},  {{0.0f, s, 0.0f}},  {{0.0f, 0.0f, s}},
        {{0.0f, -s, 0.0f}}, {{0.0f, 0.0f, -s}}, {{-s, 0.0f, 0.0f}},
        {{0.0f, 0.0f, s}},  {{s, 0.0f, 0.0f}},  {{0.0f, 0.0f, -s}}};
    std::unique_ptr<const Geometry3D> shape(
        new Geometry3D(3, 12, octahedron_vtx));
    return shape;
}

std::unique_ptr<const GeometryIndex3D> WireCube(GLfloat s = 1.0f,
                                                GLfloat d = 0.8f,
                                                GLfloat t = 0.1f) {
    const Vec3 cube_vtx[] = {
        {{-s, -s, -s}, {t, t, t}},  // 0
        {{-s, -s, s}, {t, t, d}},   // 1
        {{-s, s, s}, {t, d, t}},    // 2
        {{-s, s, -s}, {t, d, d}},   // 3
        {{s, s, -s}, {d, t, t}},    // 4
        {{s, -s, -s}, {d, t, d}},   // 5
        {{s, -s, s}, {d, d, t}},    // 6
        {{s, s, s}, {d, d, d}}      // 7
    };
    const GLuint cube_idx[] = {
        1, 0,  //
        2, 7,  //
        3, 0,  //
        4, 7,  //
        5, 0,  //
        6, 7,  //
        1, 2,  //
        2, 3,  //
        3, 4,  //
        4, 5,  //
        5, 6,  //
        6, 1   //
    };
    std::unique_ptr<const GeometryIndex3D> shape(
        new GeometryIndex3D(3, 8, cube_vtx, 24, cube_idx));
    return shape;
}

std::unique_ptr<const GeometryIndex3D> SolidCube(GLfloat s = 1.0f) {
    const Vec3 cube_vtx[] = {
        // left
        {{-s, -s, -s}, {-1.0f, 0.0f, 0.0f}},
        {{-s, -s, s}, {-1.0f, 0.0f, 0.0f}},
        {{-s, s, s}, {-1.0f, 0.0f, 0.0f}},
        {{-s, -s, -s}, {-1.0f, 0.0f, 0.0f}},
        {{-s, s, s}, {-1.0f, 0.0f, 0.0f}},
        {{-s, s, -s}, {-1.0f, 0.0f, 0.0f}},

        // back
        {{s, -s, -s}, {0.0f, 0.0f, -1.0f}},
        {{-s, -s, -s}, {0.0f, 0.0f, -1.0f}},
        {{-s, s, -s}, {0.0f, 0.0f, -1.0f}},
        {{s, -s, -s}, {0.0f, 0.0f, -1.0f}},
        {{-s, s, -s}, {0.0f, 0.0f, -1.0f}},
        {{s, s, -s}, {0.0f, 0.0f, -1.0f}},

        // bottom
        {{-s, -s, -s}, {0.0f, -1.0f, 0.0f}},
        {{s, -s, -s}, {0.0f, -1.0f, 0.0f}},
        {{s, -s, s}, {0.0f, -1.0f, 0.0f}},
        {{-s, -s, -s}, {0.0f, -1.0f, 0.0f}},
        {{s, -s, s}, {0.0f, -1.0f, 0.0f}},
        {{-s, -s, s}, {0.0f, -1.0f, 0.0f}},

        // right
        {{s, -s, s}, {1.0f, 0.0f, 0.0f}},
        {{s, -s, -s}, {1.0f, 0.0f, 0.0f}},
        {{s, s, -s}, {1.0f, 0.0f, 0.0f}},
        {{s, -s, s}, {1.0f, 0.0f, 0.0f}},
        {{s, s, -s}, {1.0f, 0.0f, 0.0f}},
        {{s, s, s}, {1.0f, 0.0f, 0.0f}},

        // top
        {{-s, s, -s}, {0.0f, 1.0f, 0.0f}},
        {{-s, s, s}, {0.0f, 1.0f, 0.0f}},
        {{s, s, s}, {0.0f, 1.0f, 0.0f}},
        {{-s, s, -s}, {0.0f, 1.0f, 0.0f}},
        {{s, s, s}, {0.0f, 1.0f, 0.0f}},
        {{s, s, -s}, {0.0f, 1.0f, 0.0f}},

        // front
        {{-s, -s, s}, {0.0f, 0.0f, 1.0f}},
        {{s, -s, s}, {0.0f, 0.0f, 1.0f}},
        {{s, s, s}, {0.0f, 0.0f, 1.0f}},
        {{-s, -s, s}, {0.0f, 0.0f, 1.0f}},
        {{s, s, s}, {0.0f, 0.0f, 1.0f}},
        {{-s, s, s}, {0.0f, 0.0f, 1.0f}},
    };

    const GLuint cube_idx[] = {
        0,  1,  2,  3,  4,  5,   // left
        6,  7,  8,  9,  10, 11,  // back
        12, 13, 14, 15, 16, 17,  // bottom
        18, 19, 20, 21, 22, 23,  // right
        24, 25, 26, 27, 28, 29,  // top
        30, 31, 32, 33, 34, 35   // front
    };

    std::unique_ptr<const GeometryIndex3D> shape(
        new GeometryIndex3D(3, 36, cube_vtx, 36, cube_idx));
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

inline GLuint CreateProgram(const char* vsrc, const char* fsrc,
                            bool use_normal = false) {
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
    if (use_normal) {
        glBindAttribLocation(program, 1, "normal");
    }
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
                          const std::string frag_shader_file,
                          bool use_normal = false) {
    std::vector<GLchar> vsrc, fsrc;
    const bool vst(ReadShaderSource(vert_shader_file, vsrc));
    const bool fst(ReadShaderSource(frag_shader_file, fsrc));
    return vst && fst ? CreateProgram(vsrc.data(), fsrc.data(), use_normal) : 0;
}

// ============================== GUI ===================================
Window::Window(int width, int height, const char* title, GLFWmonitor* monitor,
               GLFWwindow* share)
    : m_window(glfwCreateWindow(width, height, title, monitor, share)),
      m_width(width),
      m_height(height),
      m_scale(100.0f),
      m_location{0.0f, 0.0f} {
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
    glfwPollEvents();

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

void Matrix::GetNormalMatrix(GLfloat* m) const {
    m[0] = m_matrix[5] * m_matrix[10] - m_matrix[6] * m_matrix[9];
    m[1] = m_matrix[6] * m_matrix[8] - m_matrix[4] * m_matrix[10];
    m[2] = m_matrix[5] * m_matrix[9] - m_matrix[5] * m_matrix[8];
    m[3] = m_matrix[9] * m_matrix[2] - m_matrix[10] * m_matrix[1];
    m[4] = m_matrix[10] * m_matrix[0] - m_matrix[8] * m_matrix[2];
    m[5] = m_matrix[8] * m_matrix[1] - m_matrix[9] * m_matrix[0];
    m[6] = m_matrix[1] * m_matrix[6] - m_matrix[2] * m_matrix[5];
    m[7] = m_matrix[2] * m_matrix[4] - m_matrix[0] * m_matrix[6];
    m[8] = m_matrix[0] * m_matrix[5] - m_matrix[1] * m_matrix[4];
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
