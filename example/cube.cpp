#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "tiny_glfw_renderer.h"

using namespace tiny_glfw_renderer;

const std::string SHADER_DIR = "../example/shaders/";
const std::string MVP_VERT = SHADER_DIR + "mvp.vert";
const std::string FRAG = SHADER_DIR + "point.frag";

int main() {
    Initialize();
    Window window(640, 480, "Test");

    const GLuint program(LoadProgram(MVP_VERT, FRAG));
    const GLint model_location(glGetUniformLocation(program, "model"));
    const GLint view_location(glGetUniformLocation(program, "view"));
    const GLint proj_location(glGetUniformLocation(program, "projection"));

    auto shape = Cube(1.0f);

    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
    while (window.ShouldClose() == GL_FALSE) {
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(program);

        // translation
        const GLfloat *const position(window.GetLocation());
        const Matrix translation(
            Matrix::Translate(position[0], position[1], 0.0f));

        // model matrix
        const Matrix model(translation);
        glUniformMatrix4fv(model_location, 1, GL_FALSE, model.Data());

        // view matrix
        const Matrix view(Matrix::LookAt(3.0f, 4.0f, 5.0f, 0.0f, 0.0f, 0.0f,
                                         0.0f, 1.0f, 0.0f));
        glUniformMatrix4fv(view_location, 1, GL_FALSE, view.Data());

        // projection matrix
        const GLfloat fovy(window.GetScale() * 0.01f);
        const GLfloat aspect(window.GetAspect());
        const Matrix projection(Matrix::Perspective(fovy, aspect, 1.0f, 10.0f));
        glUniformMatrix4fv(proj_location, 1, GL_FALSE, projection.Data());

        shape->draw();
        window.SwapBuffers();
    }
}
