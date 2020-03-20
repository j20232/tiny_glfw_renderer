#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "tiny_glfw_renderer.h"

using namespace tiny_glfw_renderer;

const std::string SHADER_DIR = "../example/shaders/";
const std::string MVP_VERT = SHADER_DIR + "mvp.vert";
const std::string FRAG = SHADER_DIR + "point.frag";
const Vec2 rectangle_vtx[] = {
    {-0.5f, -0.5f}, {0.5f, -0.5f}, {0.5f, 0.5f}, {-0.5f, 0.5f}};

int main() {
    Initialize();
    Window window(640, 480, "Test");

    const GLuint program(LoadProgram(MVP_VERT, FRAG));
    const GLint model_location(glGetUniformLocation(program, "model"));
    const GLint view_location(glGetUniformLocation(program, "view"));
    const GLint proj_location(glGetUniformLocation(program, "projection"));

    std::unique_ptr<const Geometry2D> shape(
        new Geometry2D(2, 4, rectangle_vtx));
    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
    while (window.ShouldClose() == GL_FALSE) {
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(program);

        const GLfloat width = window.GetWidth();
        const GLfloat height = window.GetHeight();
        const GLfloat scale = window.GetScale() * 2.0f;

        // Scaling
        const Matrix scaling(
            Matrix::Scale(scale / width, scale / height, 1.0f));

        // translation
        const GLfloat *const position(window.GetLocation());
        const Matrix translation(
            Matrix::Translate(position[0], position[1], 0.0f));

        // model matrix
        const Matrix model(translation * scaling);
        glUniformMatrix4fv(model_location, 1, GL_FALSE, model.Data());

        // view matrix
        const Matrix view(Matrix::LookAt(3.0f, 4.0f, 5.0f, 0.0f, 0.0f, 0.0f,
                                         0.0f, 1.0f, 0.0f));
        glUniformMatrix4fv(view_location, 1, GL_FALSE, view.Data());

        // projection matrix
        const GLfloat w(width / scale), h(height / scale);
        const Matrix projection(Matrix::Orthogonal(-w, w, -h, h, 1.0f, 10.0f));
        glUniformMatrix4fv(proj_location, 1, GL_FALSE, projection.Data());

        shape->draw();
        window.SwapBuffers();
    }
}
