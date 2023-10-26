#pragma once

#include <string>
#include "gl.hpp"
#include <glm/glm.hpp>

class Shader {
public:
  Shader(const std::string &vertexPath, const std::string &fragmentPath);

  ~Shader();

  void use() const;

  void setBool(const std::string &name, bool value) const;

  void setInt(const std::string &name, int value) const;

  void setFloat(const std::string &name, float value) const;

  void setMat4(const std::string &name, glm::mat4 value) const;

private:
  GLuint program;

  [[nodiscard]] GLuint compileShader(const std::string &path, GLenum type) const;

  void checkCompileErrors(GLuint shader, const std::string &path) const;

  void checkLinkErrors(GLuint programId) const;
};
