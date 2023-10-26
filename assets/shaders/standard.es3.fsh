#version 300 es

precision mediump float;

in float vOcclusion;
out vec4 FragColor;

void main() {
  vec3 color = vec3(1.0, 1.0, 1.0);
  FragColor = vec4(vec3(color * vOcclusion), 1.0);
}
