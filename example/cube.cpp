#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "tiny_glfw_renderer.h"

using namespace tiny_glfw_renderer;

const std::string SHADER_DIR = "../example/shaders/";
const std::string MVP_VERT = SHADER_DIR + "naive_mvp.vert";
const std::string FRAG = SHADER_DIR + "normal_point.frag";

int main() {
    Initialize();
    Window window(640, 480, "Test");

    const GLuint program(LoadProgram(MVP_VERT, FRAG, true));

    // mvp
    const GLint model_location(glGetUniformLocation(program, "model"));
    const GLint view_location(glGetUniformLocation(program, "view"));
    const GLint proj_location(glGetUniformLocation(program, "projection"));

    // light
    const GLint normal_location(glGetUniformLocation(program, "normal_mat"));
    const GLint Lpos_location(glGetUniformLocation(program, "Lpos"));
    const GLint Lamb_location(glGetUniformLocation(program, "Lamb"));
    const GLint Ldiff_location(glGetUniformLocation(program, "Ldiff"));
    const GLint Lspec_location(glGetUniformLocation(program, "Lspec"));

    // material
    const GLint material_location(glGetUniformBlockIndex(program, "material"));
    glUniformBlockBinding(program, material_location, 0);
    static constexpr Material color[] = {{{{0.6f, 0.6f, 0.2f}},  // Kamb
                                          {{0.6f, 0.6f, 0.2f}},  // Kdiff
                                          {{0.3f, 0.3f, 0.3f}},  // Kspec
                                          30.0f},                // Kshi
                                         {{{0.1f, 0.1f, 0.5f}},
                                          {{0.1f, 0.1f, 0.5f}},
                                          {{0.4f, 0.4f, 0.4f}},
                                          60.0f}};
    const Uniform<Material> material(color, 2);

    // geometry
    auto cube = SolidCube(1.0f);
    auto sphere = SolidSphere(24);

    // light
    GLfloat normal_mat[9];
    static constexpr int Lcount(2);
    static constexpr Vector Lpos[] = {{{0.0f, 0.0f, 5.0f, 1.0f}},
                                      {{8.0f, 0.0f, 0.0f, 1.0f}}};
    static constexpr GLfloat Lamb[] = {0.2f, 0.1f, 0.1f, 0.1f, 0.1f, 0.1f};
    static constexpr GLfloat Ldiff[] = {1.0f, 0.5f, 0.5f, 0.9f, 0.9f, 0.9f};
    static constexpr GLfloat Lspec[] = {1.0f, 0.5f, 0.5f, 0.9f, 0.9f, 0.9f};

    glClearColor(0.1f, 0.1f, 0.4f, 0.0f);

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

        // normal
        const Matrix modelview(view * model);
        modelview.GetNormalMatrix(normal_mat);
        glUniformMatrix3fv(normal_location, 1, GL_FALSE, normal_mat);

        // projection matrix
        const GLfloat fovy(window.GetScale() * 0.01f);
        const GLfloat aspect(window.GetAspect());
        const Matrix projection(Matrix::Perspective(fovy, aspect, 1.0f, 10.0f));
        glUniformMatrix4fv(proj_location, 1, GL_FALSE, projection.Data());

        // light
        for (int i = 0; i < Lcount; i++) {
            glUniform4fv(Lpos_location + i, 1, (view * Lpos[i]).data());
        }
        glUniform3fv(Lamb_location, Lcount, Lamb);
        glUniform3fv(Ldiff_location, Lcount, Ldiff);
        glUniform3fv(Lspec_location, Lcount, Lspec);

        material.select(0);
        sphere->draw(GL_TRIANGLES);

        // model2
        const Matrix model2(translation * Matrix::Translate(0.0f, 0.0f, 3.0f));
        glUniformMatrix4fv(model_location, 1, GL_FALSE, model2.Data());

        // normals
        const Matrix modelview2(view * model2);
        modelview2.GetNormalMatrix(normal_mat);
        glUniformMatrix3fv(normal_location, 1, GL_FALSE, normal_mat);

        material.select(1);
        cube->draw(GL_TRIANGLES);

        window.SwapBuffers();
    }
}
