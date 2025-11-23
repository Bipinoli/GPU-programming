#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "shader.hpp"


GLuint submitShaderProgram(const GLchar* vertexShaderSource, const GLchar* fragmentShaderSource);
GLuint getUniformLocation(GLuint shaderProgramId, const char* uniformName);

Shader::Shader(const GLchar* vertexShaderSource, const GLchar* fragmentShaderSource) {
  shaderProgramId = submitShaderProgram(vertexShaderSource, fragmentShaderSource);
}
void Shader::use() {
  glUseProgram(shaderProgramId);
}
void Shader::setUniform1f(const char* name, float val) {
  int uniformLoc = getUniformLocation(shaderProgramId, name);
  if (uniformLoc == -1) {
    std::cout << "ERROR! couldn't locate the uniform: " << name << std::endl;
    exit(1);
  }
  glUniform1f(uniformLoc, val);
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


