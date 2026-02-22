#version 460

// Vertex attributes
layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexNormal;
layout (location = 2) in vec2 VertexTexCoord;

// Outputs to frag
out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoord;

// Matrices
uniform mat4 ModelMatrix;
uniform mat3 NormalMatrix;
uniform mat4 MVP;


void main() {
    // Convert vertex pos and normal into world space
    FragPos = (ModelMatrix * vec4(VertexPosition, 1.0f)).xyz;
    Normal = normalize(NormalMatrix * VertexNormal);

    TexCoord = VertexTexCoord;

    gl_Position = MVP * vec4(VertexPosition, 1.0f);
}
