#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>

class Shader {
  public:
    Shader(const std::string& vertShaderPath, const std::string& fragShaderPath);
    void use();
    void setUniform1f(const std::string& name, float val);
    void setUniform1i(const std::string& name, int val);
    void setUnifromMatrix4fv(const std::string& name, const GLfloat* value);
  private:
    GLuint shaderProgramId;
};

