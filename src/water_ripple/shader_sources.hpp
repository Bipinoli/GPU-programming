// Based on: https://www.youtube.com/watch?v=ZcRptHYY3zM



const char* vertexShaderSource = R"(
#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
out vec2 pos;

void main() {
  gl_Position = vec4(aPos, 1.0f);
  pos = aTexCoord;  
}
)";



// ---------------------------------------------------------

const char* fragmentShaderSource = R"(
#version 330 core

in vec2 pos;
out vec4 color;

uniform sampler2D bgImage;
uniform vec2 aspect;
uniform vec2 centre;
uniform float t; // position in the timescale of animation

const float maxRadius = 0.25;

float getOffsetStrength(float t, vec2 dir) {
  float d = length(dir/aspect) - t * maxRadius; // SDF of circle
  d *= 1.0 - smoothstep(0., 0.05, abs(d)); // mask only to a boundary near to circle
  d *= smoothstep(0., 0.05, t); // smooth intro
  d *= 1.0 - smoothstep(0.5, 1., t); // smooth outro
  return d;
}

void main() {
  vec2 dir = pos - centre;
  float d = getOffsetStrength(t, dir);
  dir = normalize(dir);
  // SDF on inner ring is -ve so subtracts the brigthness
  // SDF on outside ring is +ve so adds the brightness
  float shadow = d * 6.; 
  vec4 tex = texture(bgImage, pos + dir * d);
  color = tex + shadow;
}
)";
