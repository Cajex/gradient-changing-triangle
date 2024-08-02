#include <fstream>
#include <iostream>
#include <sstream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

constexpr int X = 1200;
constexpr int Y = 800;

std::string read(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) return "";
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

namespace interface {
    GLuint VAO, VBO;
    GLuint defaultShaderProgram;

    void init() {
        glViewport(0, 0, X, Y);

        constexpr GLfloat vertices[] = {
            // position         // color
            -1.0f, -1.0f, 1.0f,  1.0f, 0.0f, 0.0f,
             1.0f, -1.0f, 1.0f,  0.0f, 1.0f, 0.0f,
             0.0f,  1.0f, 1.0f,  0.0f, 0.0f, 1.0f,
        };

        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        /* position attributes */
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), static_cast<void *>(nullptr));
        glEnableVertexAttribArray(0);

        /* color attributes */
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), reinterpret_cast<void *>(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        const auto vertexSource = read("vertex.glsl");
        const auto fragmentSource = read("fragment.glsl");

        const GLuint vertex = glCreateShader(GL_VERTEX_SHADER);
        const GLuint fragment = glCreateShader(GL_FRAGMENT_SHADER);


        const GLchar* vc = vertexSource.c_str();
        const GLchar* fc = fragmentSource.c_str();
        glShaderSource(vertex, 1, &vc, nullptr);
        glShaderSource(fragment, 1, &fc, nullptr);
        glCompileShader(vertex);
        glCompileShader(fragment);

        defaultShaderProgram = glCreateProgram();
        glAttachShader(defaultShaderProgram, vertex);
        glAttachShader(defaultShaderProgram, fragment);

        int result;
        char log[512];

        glLinkProgram(defaultShaderProgram);
        glGetProgramiv(defaultShaderProgram, GL_LINK_STATUS, &result);
        if (!result) {
            glGetProgramInfoLog(defaultShaderProgram, 512, nullptr, log);
            std::cerr << "ERROR::PROGRAM::LINKING_FAILED\n" << log << "\n";
            std::cerr << "vertex: " << "\n" << vc << "\n";
            std::cerr << "fragment: " << "\n" << fc << "\n";
        }

        glValidateProgram(defaultShaderProgram);
        glGetProgramiv(defaultShaderProgram, GL_VALIDATE_STATUS, &result);
        if (!result) {
            glGetProgramInfoLog(defaultShaderProgram, 512, nullptr, log);
            std::cerr << "ERROR::PROGRAM::VALIDATING_FAILED\n" << log << std::endl;
        }

        glDeleteShader(vertex);
        glDeleteShader(fragment);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

    }

    void updateColors(const float time) {
        const GLfloat dynamicColors[] = {
            1.0f * sin(time), 0.0f, 0.0f,  // Untere linke Ecke
            0.0f, 1.0f * sin(time), 0.0f,  // Untere rechte Ecke
            0.0f, 0.0f, 1.0f * sin(time)   // Obere Ecke
        };

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 3 * sizeof(float), 3 * sizeof(float), &dynamicColors[0]);
        glBufferSubData(GL_ARRAY_BUFFER, 9 * sizeof(float), 3 * sizeof(float), &dynamicColors[3]);
        glBufferSubData(GL_ARRAY_BUFFER, 15 * sizeof(float), 3 * sizeof(float), &dynamicColors[6]);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    bool update(GLFWwindow* const window) {
        glfwPollEvents();

        glClearColor(1.0, 1.0, 1.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(defaultShaderProgram);
        const auto time = static_cast<float>(glfwGetTime());
        updateColors(time);

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glBindVertexArray(0);

        glfwSwapBuffers(window);
        return true;
    }
}

int main() {
    if (!glfwInit()) return 1;
    glfwSetErrorCallback([](auto, auto desc) {printf("%d", desc);});
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, false);

    GLFWwindow* const window = glfwCreateWindow(X, Y, "OpenGL", nullptr, nullptr);
    glfwMakeContextCurrent(window);
    if (glewInit()) return 1;
    glewExperimental = true;

    interface::init();
    while (!glfwWindowShouldClose(window)) {
        /* ReSharper disable once CppDFAConstantConditions */
        if (!interface::update(window))
            glfwSetWindowShouldClose(window, true);
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}