#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <imgui.h>

#include <iostream>
#include <iterator>
#include <map>
#include <optional>
#include <string>
#include <string_view>
#include <vector>
#include <tuple>

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

    if (glewInit()) {
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

    // https://stackoverflow.com/questions/16380005/opengl-3-4-glvertexattribpointer-stride-and-offset-miscalculation
    auto [program, vao, vao_len] = []() {
        auto vsrc = read_file("shader/newton_fractal.vert").value();
        auto fsrc = read_file("shader/newton_fractal.frag").value();

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

    // clang-format off
    GLfloat roots[] = {
        1.0, 0.0,
        -1.0, 0.0,
        0.0, 1.0,
        0.0, -1.0,
        1.0, 1.0
    };
    GLfloat colors[] = {
        255 / 255.0f, 173 / 255.0f, 173 / 255.0f,
        255 / 255.0f, 173 / 255.0f, 255 / 255.0f,
        173 / 255.0f, 173 / 255.0f, 255 / 255.0f,
        173 / 255.0f, 255 / 255.0f, 255 / 255.0f,
        173 / 255.0f, 255 / 255.0f, 173 / 255.0f
    };
    // clang-format on
    GLfloat scale = 1.0;
    int detail_scale = 1;
    auto im_winsize = ImVec2{200.0, 300.0};
    bool only_first = true;

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(program);
        int winsize[2];
        glfwGetWindowSize(window, &winsize[0], &winsize[1]);
        glUniform2f(glGetUniformLocation(program, "winsize"), winsize[0], winsize[1]);
        glUniform1f(glGetUniformLocation(program, "scale"), scale / detail_scale);

        glUniform2fv(glGetUniformLocation(program, "roots"), std::size(roots) / 2, roots);
        glUniform3fv(glGetUniformLocation(program, "colors"), std::size(colors) / 3, colors);

        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLE_FAN, 0, vao_len);
        glBindVertexArray(0);
        glUseProgram(0);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        if (only_first) ImGui::SetNextWindowSize(im_winsize);
        ImGui::Begin("Settings");
        ImGui::SliderFloat("scale", &scale, 1.0f, 10.0f);
        ImGui::SliderInt("detail scale", &detail_scale, 1, 10);

        ImGui::SliderFloat2("root 1", roots + 0, -10.0f, 10.0f);
        ImGui::SliderFloat2("root 2", roots + 2, -10.0f, 10.0f);
        ImGui::SliderFloat2("root 3", roots + 4, -10.0f, 10.0f);
        ImGui::SliderFloat2("root 4", roots + 6, -10.0f, 10.0f);
        ImGui::SliderFloat2("root 5", roots + 8, -10.0f, 10.0f);

        ImGui::ColorEdit3("color 1", colors + 0);
        ImGui::ColorEdit3("color 2", colors + 3);
        ImGui::ColorEdit3("color 3", colors + 6);
        ImGui::ColorEdit3("color 4", colors + 9);
        ImGui::ColorEdit3("color 5", colors + 12);
        ImGui::End();
        ImGui::Render();

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
        only_first = false;
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glDeleteProgram(program);
    glfwTerminate();
}
