#pragma once

#include <cstdint>
#include "Mesh.hpp"

constexpr int CHUNK_SIZE = 16;
constexpr float NOISE_SCALE = 0.1f;
constexpr float HEIGHT_SCALE = 8.0f;

class Chunk {
public:
  Chunk(int worldX, int worldZ, uint32_t seed);

  ~Chunk();

  void updateMesh();

  float getOcclusion(glm::vec3 position) const;

  void tryUpdateMesh();

  void render() const;

  bool isSolid(int x, int y, int z) const;

private:
  uint8_t data[CHUNK_SIZE][CHUNK_SIZE][CHUNK_SIZE];
  Mesh mesh;

  long long seed;

  bool isDirty = true;

  void addFace(glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3 d, glm::vec3 normal, glm::vec3 origin);

  void uploadToGPU();
};
