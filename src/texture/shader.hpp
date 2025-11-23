#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

class Shader {
  public:
    Shader(const GLchar* vertexShaderSource, const GLchar* fragmentShaderSource);
    void use();
    void setUniform1f(const char* name, float val);
  private:
    GLuint shaderProgramId;
};

