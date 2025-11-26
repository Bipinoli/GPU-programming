//// Note:: the code below is adapted from the chatGPT generated code for reference

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>
#include <iostream>
#include <cmath>

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// Vertex shader for fullscreen quad
const char* quadVertShaderSrc = R"glsl(
#version 330 core
layout (location = 0) in vec2 aPos;
out vec2 TexCoords;
void main() {
    TexCoords = aPos * 0.5 + 0.5;
    gl_Position = vec4(aPos, 0.0, 1.0);
}
)glsl";

// Fragment shader (from previous response)
const char* raymarchFragShaderSrc = R"glsl(
#version 330 core
out vec4 FragColor;
in vec2 TexCoords;
uniform sampler2D tex;
uniform vec3 camPos;
uniform mat3 camRot;
uniform float time;

// ---------- SDF Functions ----------
float sdBox(vec3 p, vec3 b) {
    vec3 q = abs(p) - b;
    return length(max(q,0.0)) + min(max(q.x,max(q.y,q.z)), 0.0);
}
float opSmoothUnion(float d1, float d2, float k) {
    float h = clamp(0.5 + 0.5*(d2 - d1)/k, 0.0, 1.0);
    return mix(d2,d1,h) - k*h*(1.0 - h);
}

// ---------- Scene ----------
vec3 pos1, pos2;
vec3 cubeHalf = vec3(0.5);

float sceneSDF(vec3 p) {
    float earlyMerge = 1.2; // distance at which merging starts
    float c1 = sdBox(p - pos1, cubeHalf) - earlyMerge;
    float c2 = sdBox(p - pos2, cubeHalf) - earlyMerge;
    return opSmoothUnion(c1, c2, 0.2);
}

// ---------- Normals ----------
vec3 getNormal(vec3 p) {
    float eps = 0.0005;
    return normalize(vec3(
        sceneSDF(p + vec3(eps,0,0)) - sceneSDF(p - vec3(eps,0,0)),
        sceneSDF(p + vec3(0,eps,0)) - sceneSDF(p - vec3(0,eps,0)),
        sceneSDF(p + vec3(0,0,eps)) - sceneSDF(p - vec3(0,0,eps))
    ));
}

// ---------- Triplanar Texture ----------
vec3 triplanar(sampler2D tex, vec3 p, vec3 n) {
    vec3 an = abs(n);
    vec3 xproj = texture(tex, p.yz).rgb;
    vec3 yproj = texture(tex, p.zx).rgb;
    vec3 zproj = texture(tex, p.xy).rgb;
    return (xproj*an.x + yproj*an.y + zproj*an.z) / (an.x + an.y + an.z);
}

// ---------- Raymarch ----------
float raymarch(vec3 ro, vec3 rd, out vec3 pos) {
    float t = 0.0;
    for(int i=0;i<100;i++) {
        pos = ro + t*rd;
        float d = sceneSDF(pos);
        if(d < 0.001) break;
        t += d;
        if(t>50.0) break;
    }
    return t;
}

// ---------- Main ----------
void main() {
    vec2 uv = TexCoords*2.0 - 1.0;
    uv.x *= 800.0/600.0;

    vec3 ro = camPos;
    vec3 rd = normalize(camRot * vec3(uv.xy, -1.0));

    float mergeDist = 1.5;
    float t = sin(time)*0.5 + 0.5;
    pos1 = vec3(-mergeDist*(1.0-t),0,0);
    pos2 = vec3( mergeDist*(1.0-t),0,0);

    vec3 p;
    float dist = raymarch(ro, rd, p);
    if(dist>50.0) { FragColor = vec4(0.2,0.3,0.3,1.0); return; }

    vec3 n = getNormal(p);
    vec3 lightDir = normalize(vec3(0.5,1.0,0.7));
    float diff = max(dot(n, lightDir),0.0);
    vec3 col = triplanar(tex, p, n);

    FragColor = vec4(col*diff,1.0);
}
)glsl";

// Function to compile shader
GLuint compileShader(GLenum type, const char* src) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);
    int success; char info[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if(!success) { glGetShaderInfoLog(shader, 512, nullptr, info); std::cout << info << std::endl; }
    return shader;
}

// Function to create shader program
GLuint createProgram(const char* vsSrc, const char* fsSrc) {
    GLuint vs = compileShader(GL_VERTEX_SHADER, vsSrc);
    GLuint fs = compileShader(GL_FRAGMENT_SHADER, fsSrc);
    GLuint prog = glCreateProgram();
    glAttachShader(prog, vs); glAttachShader(prog, fs);
    glLinkProgram(prog);
    glDeleteShader(vs); glDeleteShader(fs);
    return prog;
}

// Load texture
GLuint loadTexture(const char* path) {
    int w,h,n;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(path, &w, &h, &n, 3);
    if(!data) { std::cout << "Failed to load texture\n"; return 0; }
    GLuint tex; glGenTextures(1,&tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,w,h,0,GL_RGB,GL_UNSIGNED_BYTE,data);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    stbi_image_free(data);
    return tex;
}

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
    glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH,SCR_HEIGHT,"Smooth Merge Cubes",NULL,NULL);
    if(!window) { std::cout << "Failed to create GLFW window\n"; return -1; }
    glfwMakeContextCurrent(window);
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) { std::cout << "GLAD failed\n"; return -1; }

    // Fullscreen quad
    float quadVertices[] = { -1,-1, 1,-1, -1,1, 1,1 };
    GLuint VAO,VBO;
    glGenVertexArrays(1,&VAO); glGenBuffers(1,&VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER,VBO);
    glBufferData(GL_ARRAY_BUFFER,sizeof(quadVertices),quadVertices,GL_STATIC_DRAW);
    glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,2*sizeof(float),(void*)0);
    glEnableVertexAttribArray(0);

    GLuint shaderProg = createProgram(quadVertShaderSrc, raymarchFragShaderSrc);
    GLuint texID = loadTexture("assets/container.jpg"); // <-- your texture path

    glUseProgram(shaderProg);
    glUniform1i(glGetUniformLocation(shaderProg,"tex"),0);

    float lastTime = 0.0f;

    while(!glfwWindowShouldClose(window)) {
        float currentTime = glfwGetTime();
        float deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        glClearColor(0.2f,0.3f,0.3f,1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProg);

        // Camera uniforms (static simple camera)
        glUniform3f(glGetUniformLocation(shaderProg,"camPos"),0.0f,0.0f,3.0f);
        float camRot[9] = {1,0,0,0,1,0,0,0,1};
        glUniformMatrix3fv(glGetUniformLocation(shaderProg,"camRot"),1,GL_FALSE,camRot);

        glUniform1f(glGetUniformLocation(shaderProg,"time"),currentTime);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texID);

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLE_STRIP,0,4);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1,&VAO);
    glDeleteBuffers(1,&VBO);
    glfwTerminate();
    return 0;
}
