#version 460

// Vertex attributes
layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexNormal;

// Outputs to frag
out vec3 FragPos;
out vec3 Normal;

// Matrices
uniform mat4 ModelViewMatrix;
uniform mat3 NormalMatrix;
uniform mat4 MVP;


void main() {
    // Convert vertex pos and normal into view space
    FragPos = (ModelViewMatrix * vec4(VertexPosition, 1.0f)).xyz;
    Normal = normalize(NormalMatrix * VertexNormal);

    gl_Position = MVP * vec4(VertexPosition, 1.0f);
}
