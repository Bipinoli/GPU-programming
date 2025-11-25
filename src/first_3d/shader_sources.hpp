const char* vertexShaderSource = R"(
#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
out vec4 vCol;
out vec2 vTexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
  gl_Position = projection * view * model * vec4(aPos, 1.0);
  vTexCoord = aTexCoord;  
}
)";


const char* fragmentShaderSource = R"(
#version 330 core

in vec4 vCol;
in vec2 vTexCoord;
out vec4 FragColor;

uniform sampler2D texture1Data;
uniform sampler2D texture2Data;

void main() {
  FragColor = mix(texture(texture1Data, vTexCoord), texture(texture2Data, vTexCoord), 0.3f);
}
)";
