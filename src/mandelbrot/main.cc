#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <imgui.h>

#include <cstddef>
#include <iostream>
#include <iterator>
#include <optional>
#include <string>
#include <string_view>
#include <tuple>
#include <vector>

#include "include/utils.h"

constexpr std::pair glfw_winsize = {1000, 1000};

int main() {
    if (!glfwInit()) std::exit(1);
    glfwSetErrorCallback([](int ec, const char* desc) { std::cerr << "ec: " << ec << "desc: " << desc << std::endl; });
    auto* window = glfwCreateWindow(glfw_winsize.first, glfw_winsize.second, "GLFW", nullptr, nullptr);

    if (!window) {
        glfwTerminate();
        std::exit(1);
    }

    glfwMakeContextCurrent(window);
    glfwWindowHint(GLFW_SAMPLES, 8);

    if (!glewInit()) {
        glfwTerminate();
        std::exit(1);
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    auto& imgui_io = ImGui::GetIO();
    auto& imgui_style = ImGui::GetStyle();
    imgui_io.IniFilename = nullptr;
    imgui_style.WindowRounding = 5.0;
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 460");

    std::cout << "OpenGL Version:" << glGetString(GL_VERSION) << std::endl;
    std::cout << "Vendor:" << glGetString(GL_VENDOR) << std::endl;
    std::cout << "Renderer:" << glGetString(GL_RENDERER) << std::endl;
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(
        [](GLenum source, GLenum type, GLuint id, GLenum severity, [[maybe_unused]] GLsizei length,
           const GLchar* message, [[maybe_unused]] const void* userParam) {
            std::cerr << "-----glDebugMessageCallback-----" << std::endl
                      << "source: " << source << std::endl
                      << "type: " << type << std::endl
                      << "id: " << id << std::endl
                      << "severity: " << severity << std::endl
                      << "message: " << message << std::endl;
        },
        nullptr);

    auto [mandelbrot_program, mandelbrot_vao, mandelbrot_vao_len] = []() {
        auto vsrc = read_file("shader/mandelbrot.vert").value();
        auto fsrc = read_file("shader/mandelbrot.frag").value();

        auto program = create_program(vsrc.data(), fsrc.data()).value();

        auto in_location = glGetAttribLocation(program, "position");
        std::vector<Vertex> vertexes;
        vertexes.emplace_back(-1.0, -1.0);
        vertexes.emplace_back(-1.0, 1.0);
        vertexes.emplace_back(1.0, 1.0);
        vertexes.emplace_back(1.0, -1.0);

        GLuint vbo;
        glCreateBuffers(1, &vbo);

        glNamedBufferData(vbo, std::size(vertexes) * sizeof(decltype(vertexes)::value_type), std::data(vertexes),
                          GL_STATIC_DRAW);

        GLuint vao;
        glCreateVertexArrays(1, &vao);
        glVertexArrayVertexBuffer(vao, 0, vbo, 0, sizeof(Vertex));
        glVertexArrayAttribFormat(vao, in_location, 2, GL_FLOAT, GL_FALSE, 0);
        glEnableVertexArrayAttrib(vao, in_location);
        glVertexArrayAttribBinding(vao, in_location, 0);

        return std::tuple{program, vao, std::size(vertexes)};
    }();

    auto [julia_program, julia_vao, julia_vao_len] = []() {
        auto vsrc = read_file("shader/julia.vert").value();
        auto fsrc = read_file("shader/julia.frag").value();

        auto program = create_program(vsrc.data(), fsrc.data()).value();

        auto in_location = glGetAttribLocation(program, "position");
        std::vector<Vertex> vertexes;
        vertexes.emplace_back(-1.0, -1.0);
        vertexes.emplace_back(-1.0, 1.0);
        vertexes.emplace_back(1.0, 1.0);
        vertexes.emplace_back(1.0, -1.0);

        GLuint vbo;
        glCreateBuffers(1, &vbo);

        glNamedBufferData(vbo, std::size(vertexes) * sizeof(decltype(vertexes)::value_type), std::data(vertexes),
                          GL_STATIC_DRAW);

        GLuint vao;
        glCreateVertexArrays(1, &vao);
        glVertexArrayVertexBuffer(vao, 0, vbo, 0, sizeof(Vertex));
        glVertexArrayAttribFormat(vao, in_location, 2, GL_FLOAT, GL_FALSE, 0);
        glEnableVertexArrayAttrib(vao, in_location);
        glVertexArrayAttribBinding(vao, in_location, 0);

        return std::tuple{program, vao, std::size(vertexes)};
    }();

    glClearColor(0.0, 0.0, 0.0, 1.0);

    float init[2] = {};

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);

        int winsize[2];
        glfwGetWindowSize(window, &winsize[0], &winsize[1]);

        double mouse[2];
        glfwGetCursorPos(window, &mouse[0], &mouse[1]);
        mouse[0] = (2.0 * mouse[0] - winsize[0]) / winsize[0];
        mouse[1] = (-2.0 * mouse[1] + winsize[1]) / winsize[1];

        glViewport(0, 0, winsize[0] / 2, winsize[1]);
        glUseProgram(mandelbrot_program);
        glUniform2f(glGetUniformLocation(mandelbrot_program, "winsize"), winsize[0] / 2.0, winsize[1]);
        glBindVertexArray(mandelbrot_vao);
        glDrawArrays(GL_TRIANGLE_FAN, 0, mandelbrot_vao_len);
        glBindVertexArray(0);
        glUseProgram(0);

        glViewport(winsize[0] / 2, 0, winsize[0] / 2, winsize[1]);
        glUseProgram(julia_program);
        glUniform2f(glGetUniformLocation(julia_program, "winsize"), winsize[0] / 2.0, winsize[1]);
        init[0] = mouse[0] * 2 + 1;
        init[1] = mouse[1];
        glUniform2f(glGetUniformLocation(julia_program, "init"), init[0], init[1]);
        glBindVertexArray(julia_vao);
        glDrawArrays(GL_TRIANGLE_FAN, 0, julia_vao_len);
        glBindVertexArray(0);
        glUseProgram(0);

        glViewport(0, 0, winsize[0], winsize[1]);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Settings");
        ImGui::SliderFloat2("init", init, -2.0, 2.0);
        ImGui::End();
        ImGui::Render();

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glViewport(0, 0, winsize[0], winsize[1]);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glDeleteProgram(mandelbrot_program);
    glDeleteProgram(julia_program);
    glfwTerminate();
}
