#version 460

// Inputs from vert
in vec3 FragPos;
in vec2 TexCoord;
in mat3 TBN;

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

uniform struct FogData {
    float Density;
    vec3 Colour;
}Fog;

uniform sampler2D DiffuseTex;
uniform sampler2D NormalTex;
uniform bool UseTexture;
uniform vec3 CameraPos;


vec3 blinnPhong(int light, vec3 pos, vec3 normal, vec3 baseColour) {
    vec3 ambient = lights[light].La * Material.Ka;

    vec3 lightVector = vec3(lights[light].Position) - pos;
    vec3 lightDir = normalize(lightVector);
    float distance = length(lightVector);

    // Lighting affects objects less when they are further away
    float attenuation  = 7.0f / distance;

    float sDotN = max(dot(normal, lightDir), 0.0f);
    vec3 diffuse = lights[light].Ld * baseColour * sDotN;

    vec3 specular = vec3(0.0f);

    // Only calculate specular if surface faces light
    if (sDotN > 0.0f) {
        vec3 viewDir = normalize(CameraPos - pos);
        vec3 halfVector = normalize(lightDir + viewDir);
        
        // Blinn-Phong specular calculation
        specular = lights[light].Ls * Material.Ks * pow(max(dot(normal, halfVector), 0.0f), Material.Shininess);
    }

    return (ambient + diffuse + specular) * attenuation;
}


void main() {
    vec3 baseColour;
    vec3 normal;

    if (UseTexture) {
        baseColour = texture(DiffuseTex, TexCoord).rgb;

        vec3 tangentNormal = texture(NormalTex, TexCoord).rgb;
        tangentNormal = 2.0f * tangentNormal - 1.0f;
        normal = normalize(TBN * tangentNormal);
    } else {
        baseColour = Material.Kd;

        normal = normalize(TBN[2]);
    }

    float cameraDistance = length(CameraPos - FragPos);
    float fogFactor = clamp(exp(-pow(Fog.Density * cameraDistance, 2.0f)), 0.0f, 1.0f);
    
    vec3 shadingColour = vec3(0.0f);

    for (int i = 0; i < numLights; i++) {
        shadingColour += blinnPhong(i, FragPos, normal, baseColour);
    }

    vec3 finalColour = mix(Fog.Colour, shadingColour, fogFactor);

    FragColor = vec4(finalColour, 1.0f);
}
