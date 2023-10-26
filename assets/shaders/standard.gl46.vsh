#version 460 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in float occlusion;

out float vOcclusion;
out vec3 vNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
  vOcclusion = occlusion;
  vNormal = normal;
  gl_Position = projection * view * model * vec4(position, 1.0);
}
