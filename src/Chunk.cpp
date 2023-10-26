#include "Chunk.hpp"
#include <glm/gtc/noise.hpp>

Chunk::Chunk(int worldX, int worldZ, uint32_t seed) {
  this->seed = seed;

  for (auto x = 0; x < CHUNK_SIZE; ++x) {
    for (auto y = 0; y < CHUNK_SIZE; ++y) {
      for (auto z = 0; z < CHUNK_SIZE; ++z) {
        data[x][y][z] = 0;
      }
    }
  }

  for (auto x = 0; x < CHUNK_SIZE; ++x) {
    for (auto z = 0; z < CHUNK_SIZE; ++z) {
      float seededX = (float) (worldX + x) * NOISE_SCALE + (float) seed;
      float seededZ = (float) (worldZ + z) * NOISE_SCALE + (float) seed;

      auto noiseVal = glm::simplex(glm::vec2(seededX, seededZ));
      noiseVal = (noiseVal + 1.0f) / 2.0f;
      auto height = static_cast<int>(noiseVal * HEIGHT_SCALE);

      for (auto y = 0; y < height && y < CHUNK_SIZE; ++y) {
        data[x][y][z] = 1;
      }
    }
  }

  updateMesh();
}

Chunk::~Chunk() {
  glDeleteVertexArrays(1, &mesh.vao);
  glDeleteBuffers(1, &mesh.vbo);
  glDeleteBuffers(1, &mesh.ebo);
}

void Chunk::updateMesh() {
  mesh = Mesh();
  for (auto x = 0; x < CHUNK_SIZE; ++x) {
    for (auto y = 0; y < CHUNK_SIZE; ++y) {
      for (auto z = 0; z < CHUNK_SIZE; ++z) {
        if (isSolid(x, y, z)) {
          if (!isSolid(x + 1, y, z))
            addFace({x + 1, y, z}, {x + 1, y + 1, z}, {x + 1, y + 1, z + 1}, {x + 1, y, z + 1}, {1, 0, 0}, {x, y, z});
          if (!isSolid(x - 1, y, z))
            addFace({x, y, z + 1}, {x, y + 1, z + 1}, {x, y + 1, z}, {x, y, z}, {-1, 0, 0}, {x, y, z});

          if (!isSolid(x, y + 1, z))
            addFace({x, y + 1, z + 1}, {x + 1, y + 1, z + 1}, {x + 1, y + 1, z}, {x, y + 1, z}, {0, 1, 0}, {x, y, z});
          if (!isSolid(x, y - 1, z))
            addFace({x, y, z}, {x + 1, y, z}, {x + 1, y, z + 1}, {x, y, z + 1}, {0, -1, 0}, {x, y, z});

          if (!isSolid(x, y, z + 1))
            addFace({x, y, z + 1}, {x + 1, y, z + 1}, {x + 1, y + 1, z + 1}, {x, y + 1, z + 1}, {0, 0, 1}, {x, y, z});
          if (!isSolid(x, y, z - 1))
            addFace({x + 1, y + 1, z}, {x, y + 1, z}, {x, y, z}, {x + 1, y, z}, {0, 0, -1}, {x, y, z});
        }
      }
    }
  }

  uploadToGPU();

  isDirty = false;
}

float Chunk::getOcclusion(glm::vec3 position) const {
  int solidCount = 0;
  for (int dx = -1; dx <= 1; ++dx) {
    for (int dy = -1; dy <= 1; ++dy) {
      for (int dz = -1; dz <= 1; ++dz) {
        glm::vec3 neighborPos = position + glm::vec3(dx, dy, dz);
        // Ensure the neighbor position is within bounds
        if (isSolid(neighborPos.x, neighborPos.y, neighborPos.z)) {
          solidCount++;
        }
      }
    }
  }
  // Normalize the count to a value between 0 and 1
  float occlusion = 1.0f - (solidCount / 27.0f);
  return occlusion;
}

void Chunk::render() const {
  glBindVertexArray(mesh.vao);
  glDrawElements(GL_TRIANGLES, (GLsizei) mesh.indices.size(), GL_UNSIGNED_INT, nullptr);
  glBindVertexArray(0);
}

bool Chunk::isSolid(int x, int y, int z) const {
  if (x < 0 || x >= CHUNK_SIZE || y < 0 || y >= CHUNK_SIZE || z < 0 || z >= CHUNK_SIZE) {
    return false;
  }
  return data[x][y][z] != 0;
}

void Chunk::addFace(glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3 d, glm::vec3 normal, glm::vec3 origin) {
  GLuint startIndex = mesh.vertices.size();

  mesh.vertices.push_back(a);
  mesh.vertices.push_back(b);
  mesh.vertices.push_back(c);
  mesh.vertices.push_back(d);

  float a00 = getOcclusion(a);
  float a01 = getOcclusion(b);
  float a10 = getOcclusion(d);
  float a11 = getOcclusion(c);

  mesh.occlusion.push_back(a00);
  mesh.occlusion.push_back(a01);
  mesh.occlusion.push_back(a11);
  mesh.occlusion.push_back(a10);

  mesh.normals.push_back(normal);
  mesh.normals.push_back(normal);
  mesh.normals.push_back(normal);
  mesh.normals.push_back(normal);

  if (a00 + a11 > a01 + a10) {
    // Flipped quad
    mesh.indices.push_back(startIndex);
    mesh.indices.push_back(startIndex + 1);
    mesh.indices.push_back(startIndex + 3);

    mesh.indices.push_back(startIndex + 1);
    mesh.indices.push_back(startIndex + 2);
    mesh.indices.push_back(startIndex + 3);
  } else {
    // Normal quad
    mesh.indices.push_back(startIndex);
    mesh.indices.push_back(startIndex + 1);
    mesh.indices.push_back(startIndex + 2);

    mesh.indices.push_back(startIndex);
    mesh.indices.push_back(startIndex + 2);
    mesh.indices.push_back(startIndex + 3);
  }
}

void Chunk::uploadToGPU() {
  glGenVertexArrays(1, &mesh.vao);
  glGenBuffers(1, &mesh.vbo);
  glGenBuffers(1, &mesh.ebo);

  glBindVertexArray(mesh.vao);

  GLsizeiptr totalSize = (mesh.vertices.size() * sizeof(glm::vec3)) +
                         (mesh.normals.size() * sizeof(glm::vec3)) +
                         (mesh.occlusion.size() * sizeof(float));

  glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
  glBufferData(GL_ARRAY_BUFFER, totalSize, nullptr, GL_STATIC_DRAW);

  glBufferSubData(GL_ARRAY_BUFFER, 0, mesh.vertices.size() * sizeof(glm::vec3), mesh.vertices.data());
  glBufferSubData(GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(glm::vec3), mesh.normals.size() * sizeof(glm::vec3),
                  mesh.normals.data());
  glBufferSubData(GL_ARRAY_BUFFER, (mesh.vertices.size() + mesh.normals.size()) * sizeof(glm::vec3),
                  mesh.occlusion.size() * sizeof(float), mesh.occlusion.data());

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (GLvoid *) nullptr);
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3),
                        (GLvoid *) (mesh.vertices.size() * sizeof(glm::vec3)));
  glEnableVertexAttribArray(1);

  glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(float),
                        (GLvoid *) ((mesh.vertices.size() + mesh.normals.size()) * sizeof(glm::vec3)));
  glEnableVertexAttribArray(2);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size() * sizeof(GLuint), mesh.indices.data(), GL_STATIC_DRAW);

  glBindVertexArray(0);

//  mesh.vertices.clear();

//  mesh.indices.clear();
//  mesh.vertices.shrink_to_fit();
//  mesh.indices.shrink_to_fit();
}

void Chunk::tryUpdateMesh() {
  if (isDirty) {
    updateMesh();
    isDirty = false;
  }
}
