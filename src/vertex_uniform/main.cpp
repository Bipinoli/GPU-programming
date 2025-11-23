#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "shaders.hpp"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

GLuint submitShaderProgram(const GLchar* vertexShaderSource, const GLchar* fragmentShaderSource);
GLuint getUniformLocation(GLuint shaderProgramId, const char* uniformName);

int main() {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

  GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
  if (window == NULL) {
      std::cout << "Failed to create GLFW window" << std::endl;
      glfwTerminate();
      return -1;
  }
  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
      std::cout << "Failed to initialize GLAD" << std::endl;
      return -1;
  }

  GLuint shaderProgramId = submitShaderProgram(vertexShaderSource, fragmentShaderSource);

  float vertices[] = {
    // vec3 vertex, vec3 color
    -0.5f, 0.2f, 0.0f, 1.0f, 0.0f, 0.0f,
    0.3f, 0.8f, 0.0f, 0.0f, 1.0f, 0.0f,
    0.0f, -0.8f, 0.0f, 0.0f, 0.0f, 1.0f,
    0.5f, -0.8f, 0.0f, 0.0f, 0.5f, 0.5f,
  };
  unsigned int indices[] = {
    0, 1, 2,
    1, 2, 3
  };

  GLuint VBO, EBO, VAO;
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);

  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  while (!glfwWindowShouldClose(window)) {
    processInput(window);
      
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    float time = glfwGetTime();
    GLuint uniformTimeLoc = getUniformLocation(shaderProgramId, "time");

    glUseProgram(shaderProgramId);
    glUniform1f(uniformTimeLoc, time);
    glBindVertexArray(VAO);
    // glDrawArrays(GL_TRIANGLES, 0, 3);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glDeleteBuffers(1, &VBO);
  glDeleteVertexArrays(1, &VAO);
  glDeleteProgram(shaderProgramId);

  glfwTerminate();
  return 0;
}

void processInput(GLFWwindow *window) {
  if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
      glfwSetWindowShouldClose(window, true);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
  glViewport(0, 0, width, height);
}


GLuint submitShader(const GLchar* source, GLenum shaderType) {
  GLuint shaderId = glCreateShader(shaderType);
  glShaderSource(shaderId, 1, &source, nullptr);
  glCompileShader(shaderId);
  int success;
  glGetShaderiv(shaderId, GL_COMPILE_STATUS, &success);
  if (!success) {
    char log[512];
    glGetShaderInfoLog(shaderId, 512, nullptr, log);
    std::cout << "ERROR! shader compilation failed (" 
      << (shaderType == GL_VERTEX_SHADER ? "VERTEX_SHADER" : "FRAGMENT_SHADER")
      << "): " << log << std::endl;
    exit(1);
  }
  return shaderId;
}

GLuint submitShaderProgram(const GLchar* vertexShaderSource, const GLchar* fragmentShaderSource) {
  GLuint vertexShaderId = submitShader(vertexShaderSource, GL_VERTEX_SHADER);
  GLuint fragmentShaderId = submitShader(fragmentShaderSource, GL_FRAGMENT_SHADER);
  GLuint shaderProgramId = glCreateProgram();
  glAttachShader(shaderProgramId, vertexShaderId);
  glAttachShader(shaderProgramId, fragmentShaderId);
  glLinkProgram(shaderProgramId);
  int success;
  glGetProgramiv(shaderProgramId, GL_LINK_STATUS, &success);
  if (!success) {
    char log[512];
    glGetProgramInfoLog(shaderProgramId, 512, nullptr, log);
    std::cout << "ERROR! shader link failed: " << log << std::endl;
    exit(1);
  }
  glDeleteShader(vertexShaderId);
  glDeleteShader(fragmentShaderId);
  return shaderProgramId;
}

GLuint getUniformLocation(GLuint shaderProgramId, const char* uniformName) {
  int uniformLoc = glGetUniformLocation(shaderProgramId, uniformName);
  if (uniformLoc == -1) {
    std::cout << "ERROR! couldn't locate the uniform: " << uniformName << std::endl;
    exit(1);
  }
  return uniformLoc;
}
