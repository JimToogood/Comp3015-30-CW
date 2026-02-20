#version 460

// Inputs from vert
in vec3 FragPos;
in vec3 Normal;

layout (location = 0) out vec4 FragColor;

uniform int numLights;

uniform struct LightData {
    vec4 Position;
    vec3 Ld;        // Diffuse
    vec3 La;        // Ambient
    vec3 Ls;        // Specular
} lights[5];        // Max 5 lights

uniform struct MaterialData {
    float Shininess;
    vec3 Kd;        // Diffuse
    vec3 Ka;        // Ambient
    vec3 Ks;        // Specular
}Material;

uniform vec3 CameraPos;


vec3 blinnPhong(int light, vec3 pos, vec3 normal) {
    vec3 ambient = lights[light].La * Material.Ka;
    vec3 lightDir = normalize(vec3(lights[light].Position) - pos);

    float sDotN = max(dot(normal, lightDir), 0.0f);
    vec3 diffuse = lights[light].Ld * Material.Kd * sDotN;

    vec3 specular = vec3(0.0f);

    // Only calculate specular if surface faces light
    if (sDotN > 0.0f) {
        vec3 viewDir = normalize(CameraPos - pos);
        vec3 halfVector = normalize(lightDir + viewDir);
        
        // Blinn-Phong specular calculation
        specular = lights[light].Ls * Material.Ks * pow(max(dot(normal, halfVector), 0.0f), Material.Shininess);
    }

    return (ambient + diffuse + specular);
}


void main() {
    // Re-normalise interpolated normal
    vec3 norm = normalize(Normal);
    
    vec3 finalColor = vec3(0.0f);

    for (int i = 0; i < numLights; i++) {
        finalColor += blinnPhong(i, FragPos, norm);
    }

    FragColor = vec4(finalColor, 1.0f);
}
