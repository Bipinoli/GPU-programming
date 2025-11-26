#version 330 core

in vec4 vCol;
in vec2 vTexCoord;
out vec4 FragColor;

uniform sampler2D texture1Data;
uniform sampler2D texture2Data;

void main() {
  FragColor = mix(texture(texture1Data, vTexCoord), texture(texture2Data, vTexCoord), 0.3f);
}
