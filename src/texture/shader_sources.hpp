const char* vertexShaderSource = R"(
#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aCol;
layout (location = 2) in vec2 aTexCoord;
out vec4 vCol;
out vec2 vTexCoord;

uniform float time;

void main() {
  float x = sin(aPos.x + time);
  float y = sin(aPos.y + time);
  gl_Position = vec4(x, y, aPos.z, 1.0f);
  vCol = vec4(aCol, 1.0f);
  vTexCoord = aTexCoord;  
}
)";


const char* fragmentShaderSource = R"(
#version 330 core

in vec4 vCol;
in vec2 vTexCoord;
out vec4 FragColor;

uniform sampler2D textureData;

void main() {
  FragColor = texture(textureData, vTexCoord);
}
)";
