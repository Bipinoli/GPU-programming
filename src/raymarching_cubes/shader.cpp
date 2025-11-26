#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "shader.hpp"


GLuint submitShaderProgram(const GLchar* vertexShaderSource, const GLchar* fragmentShaderSource);
GLuint getUniformLocation(GLuint shaderProgramId, const std::string& uniformName);
std::string readFileAsText(const std::string& filePath);


Shader::Shader(const std::string& vertShaderPath, const std::string& fragShaderPath) {
  std::string vertShader = readFileAsText(vertShaderPath);
  std::string fragShader = readFileAsText(fragShaderPath);
  shaderProgramId = submitShaderProgram(vertShader.c_str(), fragShader.c_str());
}
void Shader::use() {
  glUseProgram(shaderProgramId);
}
void Shader::setUniform1f(const std::string& name, float val) {
  int uniformLoc = getUniformLocation(shaderProgramId, name);
  glUniform1f(uniformLoc, val);
}
void Shader::setUniform1i(const std::string& name, int val) {
  int uniformLoc = getUniformLocation(shaderProgramId, name);
  glUniform1i(uniformLoc, val);
}
void Shader::setUnifromMatrix4fv(const std::string& name, const GLfloat* value){
  int uniformLoc = getUniformLocation(shaderProgramId, name);
  glUniformMatrix4fv(uniformLoc, 1, GL_FALSE, value);
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

GLuint getUniformLocation(GLuint shaderProgramId, const std::string& uniformName) {
  int uniformLoc = glGetUniformLocation(shaderProgramId, uniformName.c_str());
  if (uniformLoc == -1) {
    std::cout << "ERROR! couldn't locate the uniform: " << uniformName << std::endl;
    exit(1);
  }
  return uniformLoc;
}

std::string readFileAsText(const std::string& filePath) {
  std::ifstream in(filePath);
  if (!in.is_open()) {
      throw std::runtime_error("Failed to open file: " + filePath);
  }
  std::stringstream buffer;
  buffer << in.rdbuf();
  return buffer.str();
}
