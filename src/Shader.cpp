#include "Shader.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/gtc/type_ptr.hpp>
#include "Exit.hpp"

Shader::Shader(const std::string &vertexPath, const std::string &fragmentPath) {
  auto vertexShader = compileShader(vertexPath, GL_VERTEX_SHADER);
  auto fragmentShader = compileShader(fragmentPath, GL_FRAGMENT_SHADER);

  program = glCreateProgram();
  glAttachShader(program, vertexShader);
  glAttachShader(program, fragmentShader);
  glLinkProgram(program);

  checkLinkErrors(program);

  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);
}

Shader::~Shader() {
  glDeleteProgram(program);
}

void Shader::use() const {
  glUseProgram(program);
}

void Shader::setBool(const std::string &name, bool value) const {
  glUniform1i(glGetUniformLocation(program, name.c_str()), static_cast<int>(value));
}

void Shader::setInt(const std::string &name, int value) const {
  glUniform1i(glGetUniformLocation(program, name.c_str()), value);
}

void Shader::setFloat(const std::string &name, float value) const {
  glUniform1f(glGetUniformLocation(program, name.c_str()), value);
}

void Shader::setMat4(const std::string &name, glm::mat4 value) const {
  glUniformMatrix4fv(glGetUniformLocation(program, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}

GLuint Shader::compileShader(const std::string &path, GLenum type) const {
  std::ifstream file;
  file.open(path);

  if (!file.is_open()) {
    std::cerr << "Failed to open shader file: " << path << std::endl;
    exitGame(EXIT_FAILURE);
  }

  std::stringstream stream;
  stream << file.rdbuf();
  file.close();

  std::string source = stream.str();
  const char *sourcePtr = source.c_str();

  GLuint shader = glCreateShader(type);
  glShaderSource(shader, 1, &sourcePtr, nullptr);
  glCompileShader(shader);

  checkCompileErrors(shader, path);

  return shader;
}

void Shader::checkCompileErrors(GLuint shader, const std::string &path) const {
  GLint success;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

  if (!success) {
    char infoLog[512];
    glGetShaderInfoLog(shader, 512, nullptr, infoLog);
    std::cerr << "Failed to compile standardShader '" << path << "': " << infoLog << std::endl;
    exitGame(EXIT_FAILURE);
  }
}

void Shader::checkLinkErrors(GLuint programId) const {
  GLint success;
  glGetProgramiv(programId, GL_LINK_STATUS, &success);

  if (!success) {
    char infoLog[512];
    glGetProgramInfoLog(programId, 512, nullptr, infoLog);
    std::cerr << "Failed to link standardShader programId: " << infoLog << std::endl;
    exitGame(EXIT_FAILURE);
  }
}
