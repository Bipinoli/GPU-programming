#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <stb_image.h>
#include <random>
#include "shader.hpp"
#include "shader_sources.hpp"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
float randomFloat();

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;


GLuint loadTexture(const char* imgPath);


int main() {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

  GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Water Ripple", NULL, NULL);
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

  Shader shader(vertexShaderSource, fragmentShaderSource);
  glfwSetWindowUserPointer(window, &shader);
  GLuint textureId = loadTexture("assets/swimming_pool.jpg");

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, textureId);

  float vertices[] = {
    // positions            // texture coords
    1.0f,  1.0f, 0.0f,      1.0f, 1.0f,   // top right
    1.0f, -1.0f, 0.0f,      1.0f, 0.0f,   // bottom right
    -1.0f, -1.0f, 0.0f,     0.0f, 0.0f,   // bottom left
    -1.0f,  1.0f, 0.0f,     0.0f, 1.0f    // top left   
  };
  unsigned int indices[] = {
    0, 1, 2,
    3, 0, 2
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

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  shader.use();
  shader.setUniform1i("bgImage", 0); // GL_TEXTURE0
  shader.setUniform1f("t", 1.0f);
  shader.setUniform2f("aspect", 1.0f, (float)SCR_WIDTH / SCR_HEIGHT);
  shader.setUniform2f("centre", randomFloat(), randomFloat());

  const int FRAMES_TO_COUNT = 60;
  int counter = 60;

  while (!glfwWindowShouldClose(window)) {
    processInput(window);

    counter--;
    if (counter == 0) {
      counter = FRAMES_TO_COUNT;
      shader.use();
      shader.setUniform2f("centre", randomFloat(), randomFloat());
    }

    shader.use();
    float t = (float)(FRAMES_TO_COUNT - counter) / FRAMES_TO_COUNT;
    shader.setUniform1f("t", t);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glDeleteBuffers(1, &VBO);
  glDeleteVertexArrays(1, &VAO);

  glfwTerminate();
  return 0;
}

void processInput(GLFWwindow *window) {
  if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
      glfwSetWindowShouldClose(window, true);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
  glViewport(0, 0, width, height);
  Shader* shader = static_cast<Shader*>(glfwGetWindowUserPointer(window));
  shader->use();
  shader->setUniform2f("aspect", 1.0f, (float)width / height);
}



GLuint loadTexture(const char* imgPath) {
  GLuint texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  stbi_set_flip_vertically_on_load(true);
  int width, height, nrChannels;
  unsigned char *data = stbi_load(imgPath, &width, &height, &nrChannels, 0);
  if (!data) {
    std::cout << "ERROR! couldn't load the texture image: " << imgPath << std::endl;
    exit(1);
  }
  GLenum imgFmt = nrChannels == 4 ? GL_RGBA : GL_RGB;
  glTexImage2D(GL_TEXTURE_2D, 0, imgFmt, width, height, 0, imgFmt, GL_UNSIGNED_BYTE, data);
  glGenerateMipmap(GL_TEXTURE_2D);

  stbi_image_free(data);
  return texture;
}

float randomFloat() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<float> dist(0.2f, 0.7f);
    return dist(gen);
}
