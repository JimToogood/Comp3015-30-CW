#version 460

// Inputs from vert
in vec3 TexCoords;

layout (location = 0) out vec4 FragColor;

uniform samplerCube daySkyboxTex;


void main() {
    vec3 texColour = texture(daySkyboxTex, normalize(TexCoords)).rgb;
    texColour = pow(texColour, vec3(1.0f / 2.2f));

    FragColor = vec4(texColour, 1.0f);
}
