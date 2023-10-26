#pragma once

#include <vector>
#include <glm/glm.hpp>
#include "gl.hpp"

struct Mesh {
  std::vector<glm::vec3> vertices;
  std::vector<glm::vec3> normals;
  std::vector<float> occlusion;
  std::vector<GLuint> indices;

  GLuint vao, vbo, ebo;

  Mesh() : vao(0), vbo(0), ebo(0) {}
};
