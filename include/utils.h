#ifndef PRACC_GL_UTILS_H
#define PRACC_GL_UTILS_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <fstream>
#include <optional>
#include <string>

struct Vertex {
    float x_;
    float y_;
#ifndef __cpp_aggregate_paren_init
    constexpr Vertex() : x_{}, y_{} {}
    constexpr Vertex(float x, float y) : x_{x}, y_{y} {}
#endif
};

struct Color {
    float r_;
    float g_;
    float b_;
#ifndef __cpp_aggregate_paren_init
    constexpr Color() : r_{}, g_{}, b_{} {}
    constexpr Color(float r, float g, float b) : r_{r}, g_{g}, b_{b} {}
#endif
};

std::optional<std::string> read_file(const char* path) {
    std::ifstream in(path);

    if (in) {
        return std::string{std::istreambuf_iterator{in}, std::istreambuf_iterator<decltype(in)::char_type>{}};
    }

    return std::nullopt;
}

const char* ec_to_string(GLenum ec) {
    switch (ec) {
        case GL_NO_ERROR:
            return "GL_NO_ERROR";
        case GL_INVALID_ENUM:
            return "GL_INVALID_ENUM";
        case GL_INVALID_VALUE:
            return "GL_INVALID_VALUE";
        case GL_INVALID_OPERATION:
            return "GL_INVALID_OPERATION";
        case GL_INVALID_FRAMEBUFFER_OPERATION:
            return "GL_INVALID_FRAMEBUFFER_OPERATION";
        case GL_OUT_OF_MEMORY:
            return "GL_OUT_OF_MEMORY";
        case GL_STACK_UNDERFLOW:
            return "GL_STACK_UNDERFLOW";
        case GL_STACK_OVERFLOW:
            return "GL_STACK_OVERFLOW";
        default:
            return "No match error";
    }
}

GLboolean print_shader_log(GLuint shader, std::string_view str) {
    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);

    if (!status) std::cerr << "compile error: " << str << std::endl;

    GLsizei bufsize;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &bufsize);

    if (bufsize) {
        // https://yohhoy.hatenadiary.jp/entry/20160327/p1
        // https://qiita.com/yumetodo/items/24d21d97e04977b78b45
        std::basic_string<GLchar> log(bufsize - 1, GLchar{});
        GLsizei len;
        glGetShaderInfoLog(shader, bufsize, &len, log.data());
        std::cout << log << std::endl;
    }

    return status;
}

GLboolean print_program_log(GLuint program) {
    GLint status;
    glGetProgramiv(program, GL_LINK_STATUS, &status);

    if (!status) std::cerr << "link error" << std::endl;

    GLsizei bufsize;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufsize);

    if (bufsize) {
        std::basic_string<GLchar> log(bufsize - 1, GLchar{});
        GLsizei len;
        glGetProgramInfoLog(program, bufsize, &len, log.data());
        std::cout << log << std::endl;
    }

    return status;
}

std::optional<GLuint> create_program(const char* vsrc, const char* fsrc) {
    const auto program = glCreateProgram();

    if (!(vsrc && fsrc)) return std::nullopt;

    const auto vjob = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vjob, 1, &vsrc, nullptr);
    glCompileShader(vjob);
    if (print_shader_log(vjob, "vertex shader")) {
        glAttachShader(program, vjob);
    }
    glDeleteShader(vjob);

    const auto fjob = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fjob, 1, &fsrc, nullptr);
    glCompileShader(fjob);
    if (print_shader_log(fjob, "fragment shader")) {
        glAttachShader(program, fjob);
    }
    glDeleteShader(fjob);

    glLinkProgram(program);

    if (print_program_log(program)) return program;

    glDeleteProgram(program);
    return std::nullopt;
}

#endif // PRACC_GL_UTILS_H
