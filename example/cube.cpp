#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "tiny_glfw_renderer.h"

using namespace tiny_glfw_renderer;

const std::string SHADER_DIR = "../example/shaders/";
const std::string MVP_VERT = SHADER_DIR + "color_mvp.vert";
const std::string FRAG = SHADER_DIR + "normal_point.frag";

int main() {
    Initialize();
    Window window(640, 480, "Test");

    const GLuint program(LoadProgram(MVP_VERT, FRAG, true));
    const GLint model_location(glGetUniformLocation(program, "model"));
    const GLint view_location(glGetUniformLocation(program, "view"));
    const GLint proj_location(glGetUniformLocation(program, "projection"));

    auto shape = SolidCube(1.0f);

    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);

    // Back Culling
    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);

    // Depth Buffer
    glClearDepth(1.0);
    glDepthFunc(GL_LESS);
    glEnable(GL_DEPTH_TEST);

    glfwSetTime(0.0);
    while (window.ShouldClose() == GL_FALSE) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(program);

        // translation
        const GLfloat *const position(window.GetLocation());
        const Matrix r(Matrix::Rotate(glfwGetTime(), 0.0f, 1.0f, 0.0f));
        const Matrix translation(
            Matrix::Translate(position[0], position[1], 0.0f) * r);

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

        shape->draw(GL_TRIANGLES);

        const Matrix model2(translation * Matrix::Translate(0.0f, 0.0f, 3.0f));
        glUniformMatrix4fv(model_location, 1, GL_FALSE, model2.Data());
        shape->draw(GL_TRIANGLES);

        window.SwapBuffers();
    }
}
