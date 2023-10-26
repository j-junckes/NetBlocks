#version 460 core

in float vOcclusion;
in vec3 vNormal;
out vec4 FragColor;

void main() {
  vec3 color = vec3(1.0, 1.0, 1.0);
  FragColor = vec4(color * vOcclusion, 1.0);
  //FragColor = vec4(vNormal, 1.0);
}
