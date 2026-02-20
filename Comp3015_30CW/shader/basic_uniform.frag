#version 460

// Inputs from vert
in vec3 FragPos;
in vec3 Normal;

layout (location = 0) out vec4 FragColor;

uniform struct LightData {
    vec4 Position;
    vec3 Ld;        // Diffuse
    vec3 La;        // Ambient
    vec3 Ls;        // Specular
}Light;

uniform struct MaterialData {
    float Shininess;
    vec3 Kd;        // Diffuse
    vec3 Ka;        // Ambient
    vec3 Ks;        // Specular
}Material;


void main() {
    // Re-normalise interpolated normal
    vec3 norm = normalize(Normal);

    vec3 ambient = Light.La * Material.Ka;
    vec3 lightDir = normalize(vec3(Light.Position) - FragPos);

    float sDotN = max(dot(norm, lightDir), 0.0f);
    vec3 diffuse = Light.Ld * Material.Kd * sDotN;

    vec3 specular = vec3(0.0f);

    // Only calculate specular if surface faces light
    if (sDotN > 0.0f) {
        vec3 viewDir = normalize(-FragPos);
        vec3 halfVector = normalize(lightDir + viewDir);
        
        // Blinn-Phong specular calculation
        specular = Light.Ls * Material.Ks * pow(max(dot(norm, halfVector), 0.0f), Material.Shininess);
    }

    vec3 finalColor = ambient + diffuse + specular;

    FragColor = vec4(finalColor, 1.0f);
}
