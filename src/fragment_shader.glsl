#version 330 core

out vec4 FragColor;

in vec2 TexCoord;

void main() {
  float r = TexCoord.x;
  float g = TexCoord.y;
  float b = 0.0;

  FragColor = vec4(r, g, b, 1.0);
}
