## 30% Coursework for the Comp3015 Games Graphics Pipelines Module in C++ OpenGL, by Jim Toogood  

---

## Assets  
All textures and models used in this project are copyright-free and licensed for free use. All rights remain with their respective creators as listed below. This project does not claim ownership of any of the third-party assets cited.  

**Models Used:**  
house.obj - https://sketchfab.com/3d-models/fantasy-house-b107e7918892434289c0166b23c6c204  
lamp.obj - https://sketchfab.com/3d-models/modular-lowpoly-medieval-environment-5bf0a1562b7e401e9e6d7758ec54d09c  
wall.obj - https://sketchfab.com/3d-models/ruined-rock-fence-75e2716c378e4a68bac3577303671921  
branch.obj & trunk.obj - https://www.turbosquid.com/3d-models/shapespark-low-poly-exterior-plants-kit-1826978  

**Texture Assets Used:**  
day_skybox.hdr - https://polyhaven.com/a/kloofendal_48d_partly_cloudy_puresky  
night_skybox.hdr - https://polyhaven.com/a/qwantani_moon_noon_puresky  
grass.jpg - https://www.poliigon.com/texture/flat-grass-texture/4585  
path.png (based on) - https://freestylized.com/material/ground_stones_01/  

---

## How to run game EXE
1) Download repo  
2) Go to folder `Comp3015-30-CW-main/Comp3015_30CW/`  
3) Ensure following file structure exists there:  
```text
Comp3015_30CW/  
 ├── Project_Template.exe  
 ├── media/  
 │    ├── branch.obj  
 │    ├── branch.png  
 │    ├── grass_normal.jpg  
 │    ├── grass.jpg  
 │    ├── house_normal.png  
 │    ├── house.obj  
 │    ├── house.png  
 │    ├── house2.png  
 │    ├── lamp.obj  
 │    ├── lamp.png  
 │    ├── path_normal.png  
 │    ├── path.png  
 │    ├── trunk.jpg  
 │    ├── trunk.obj  
 │    ├── wall_normal.png  
 │    ├── wall.obj  
 │    ├── wall.png  
 │    ├── skybox/  
 │    │    ├── day_skybox_negx.hdr  
 │    │    ├── day_skybox_negy.hdr  
 │    │    ├── day_skybox_negz.hdr  
 │    │    ├── day_skybox_posx.hdr  
 │    │    ├── day_skybox_posy.hdr  
 │    │    ├── day_skybox_posz.hdr  
 │    │    ├── night_skybox_negx.hdr  
 │    │    ├── night_skybox_negy.hdr  
 │    │    ├── night_skybox_negz.hdr  
 │    │    ├── night_skybox_posx.hdr  
 │    │    ├── night_skybox_posy.hdr  
 │    └──  └── night_skybox_posz.hdr  
 ├── shader/  
 │    ├── basic_uniform.frag  
 │    ├── basic_uniform.vert  
 │    ├── fbo.frag  
 │    ├── fbo.vert  
 │    ├── skybox.frag  
 │    └── skybox.vert  
 └── (any additional folders/files are unrelated to running the .exe)  
```
4) Open `Project_Template.exe` and that's it!  

*Note: If you want to build a new .exe file from inside Visual Studio, it will be built to `Comp3015_30CW/x64/Debug/`. Attempting to run the .exe file from this folder will not work, the .exe must be moved to the folder show above in order to run correctly.*  

---

## Youtube link
(link here)  

---

## Visual Studio and Operating System Versions
The Visual Studio and Operating System versions used to test/write the code locally were:  
- Visual Studio 2022 Version 17.14.19  
- Windows 11 (64-bit) Version 25H2  

---

## Project Overview
This project features a small outdoor scene consisting of a grassy floor, 2 houses, a lamp post, some trees and a path, all surrounded by a stone wall. The lighting is mostly realistic (aka not cartoony), other than slightly exaggerated specularity to show off the lighting. The main features implemented in the project are:  
- Custom model (`.obj`) support  
- Blinn-Phong lighting (done in the `basic_uniform.frag` fragment shader)  
- Multiple dynamic lights that interact and overlap  
- Distance-based fog that changes colour and intensity depending on the day/night cycle  
- Full diffuse texturing with normal maps and alpha support (has both alpha from the image texture, and custom material alpha)  
- Night and day skybox with a smooth transition between the two  
- Light animation in the form of a full day/night cycle with two lights (sun and moon) that rotate around the scene, and change colour and intensity depending on the time of day  
- Gamma correction (done in the `skybox.frag` fragment shader)  
- Framebuffer-based post processing, with a brightness/exposure modifier, and a screen vignette that changes strength depending on the day/night cycle  
- Fully controllable camera with keyboard and mouse *(see control scheme below)*  

Controls (mouse & keyboard):  
- **Move Horizontally** left and right with `A` and `D`, forwards and backwards with `W` and `S`  
- **Move Vertically** up with `Space`, down with `LShift`  
- **Look Around** by moving the mouse  
- **Exit Program** with `Escape`  

---

## How it works
- The main game class `SceneBasic_Uniform` is defined in `scenebasic_uniform.h`, all the needed variables and functions are defined here.  

- Next, `camera.h` where the camera class is defined. The camera class consist of handling the keyboard and mouse inputs from the user, applying that to the camera transforms, and getters that relay that data to the main class.  
```c++
void HandleKeyboard(GLFWwindow* window, float deltaTime) {
    const float speed = 3.0f * deltaTime;

    // Horizontal controls
    vec3 horizontalFront = normalize(vec3(front.x, 0.0f, front.z));
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        position += speed * horizontalFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        position -= normalize(cross(horizontalFront, up)) * speed;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        position -= speed * horizontalFront;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        position += normalize(cross(horizontalFront, up)) * speed;

    // Vertical controls
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        position += speed * up;
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        position -= speed * up;
}

void HandleMouse(double xpos, double ypos) {
    // Initialise last x and y positions
    if (isFirstFrame) {
        lastXPos = (float)xpos;
        lastYPos = (float)ypos;
        isFirstFrame = false;
    }

    float xOffset = (float)xpos - lastXPos;
    float yOffset = lastYPos - (float)ypos;
    lastXPos = (float)xpos;
    lastYPos = (float)ypos;

    // Mouse sensitivity
    const float sensitivity = 0.04f;
    xOffset *= sensitivity;
    yOffset *= sensitivity;

    yaw += xOffset;
    pitch += yOffset;

    // Clamp pitch to avoid turning up & down too far
    pitch = clamp(pitch, -89.0f, 89.0f);

    // Calculate camera direction
    front = normalize(vec3(
        cos(radians(yaw)) * cos(radians(pitch)),
        sin(radians(pitch)),
        sin(radians(yaw)) * cos(radians(pitch))
    ));
}
```

- Moving onto `scenebasic_uniform.cpp` this is where the bulk of the code is. First we define starting values for all class variables in the initialiser, then we have the `initScene()` function to initialise everything else the program needs before running. This is where all files likes textures and objs are loaded and model transforms are set. This also calls the funciton `initFBO()` that initialises everything needed for framebuffer post processing.  
```c++
// -=-=- Load models -=-=-
houseMesh = ObjMesh::load("media/house.obj", false, true);
lampMesh = ObjMesh::load("media/lamp.obj", false);
wallMesh = ObjMesh::load("media/wall.obj", false, true);

etc...

// -=-=- Load textures -=-=-
skyboxDayTexture = Texture::loadHdrCubeMap("media/skybox/day_skybox");
skyboxNightTexture = Texture::loadHdrCubeMap("media/skybox/night_skybox");
groundTexture = Texture::loadTexture("media/grass.jpg");
groundNormal = Texture::loadTexture("media/grass_normal.jpg");

etc...

// Model transforms 
houseModel = rotate(houseModel, radians(90.0f), vec3(0.0f, 1.0f, 0.0f));
houseModel = scale(houseModel, vec3(0.5f));
houseModel = translate(houseModel, vec3(0.91f, 0.0f, 8.98f));

etc...
```

- After that is `update()` next, this handles all logic that needs to be ran every frame that isnt related to direct rendering. This is where deltaTime is calculated, to then be used to calculate the day/night cycle. The needed variables for the sun and moon lights, the fog, and the vignette are then passed to the relevant shaders, and all keyboards inputs are processed.  
```c++
timeOfDay += deltaTime;
if (timeOfDay > dayLength) { timeOfDay -= dayLength; }

float sunAngle = (timeOfDay / dayLength) * 2.0f * pi<float>();

// Sun and moon orbit opposite to each other
vec3 sunDirection = normalize(vec3(cos(sunAngle), sin(sunAngle), 0.0f));
vec3 moonDirection = -sunDirection;

// Define length of overlap between sun and moon at sunset/sunrise
const float fade = 0.35f;
float sunIntensity = clamp((sunDirection.y + fade) / (fade * 2.0f), 0.0f, 1.0f);
float moonIntensity = clamp((moonDirection.y + fade) / (fade * 2.0f), 0.0f, 1.0f);

// Pass skybox variables to skybox shader
skyboxProg.use();
skyboxProg.setUniform("blendFactor", sunIntensity);
skyboxProg.setUniform("sunDirection", sunDirection);
skyboxProg.setUniform("moonDirection", moonDirection);
skyboxProg.setUniform("sunColour", sunColour * sunIntensity);
skyboxProg.setUniform("moonColour", moonColour * moonIntensity);

// Pass lighting and fog variables to default shader
prog.setUniform("lights[0].Position", vec4((sunDirection * 12.0f), 0.0f));
prog.setUniform("lights[0].Ld", sunColour * sunIntensity);
prog.setUniform("lights[0].La", (sunColour * 0.2f) * sunIntensity);
prog.setUniform("lights[0].Ls", sunColour * sunIntensity);

prog.setUniform("lights[1].Position", vec4((moonDirection * 9.0f), 0.0f));
prog.setUniform("lights[1].Ld", moonColour * moonIntensity);
prog.setUniform("lights[1].La", (moonColour * 0.25f) * moonIntensity);
prog.setUniform("lights[1].Ls", (moonColour * 0.55f) * moonIntensity);

prog.setUniform("Fog.Density", mix(0.04f, 0.12f, moonIntensity));
prog.setUniform("Fog.Colour", mix(fogDay, fogNight, moonIntensity));
```

- Next is `renderSceneObjects()` which calls the needed shaders, passes the needed variables and material properties to each shaders, calls `setMatrices`, and renders all the objects in the scene itself (the house, the skybox, the lamp, etc.).  
```c++
// -=-=- Grass Plane -=-=-
prog.use();

glActiveTexture(GL_TEXTURE0);
glBindTexture(GL_TEXTURE_2D, groundTexture);
glActiveTexture(GL_TEXTURE1);
glBindTexture(GL_TEXTURE_2D, groundNormal);

// Materials
prog.setUniform("UVScale", 2.0f);
prog.setUniform("UseTexture", true);
prog.setUniform("UseNormal", true);
prog.setUniform("Material.Shininess", 120.0f);
prog.setUniform("Material.Alpha", 1.0f);
prog.setUniform("Material.Ka", vec3(0.6f));
prog.setUniform("Material.Ks", vec3(0.1f));

// Render
setMatrices(planeModel);
plane.render();
```

- After that is `render()` which handles the framebuffer post processing, by calling `renderSceneObjects()` and rendering the scene onto a fullscreen quad that can then be rerendered by the fbo shaders, and the post processing applied.  
```c++
// Render scene into FBO
glBindFramebuffer(GL_FRAMEBUFFER, FBO);
glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

renderSceneObjects();

glBindFramebuffer(GL_FRAMEBUFFER, 0);

// Render fullscreen as quad using FBO shaders
glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

FBOProg.use();
glActiveTexture(GL_TEXTURE0);
glBindTexture(GL_TEXTURE_2D, FBOColourTexture);

glBindVertexArray(screenQuadVAO);
glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
```

- Finally there is the smaller utility functions `compile()`, `resize`, and `setMatrices` which handle various needed OpenGL logic, such as shader compilation and passing matrix values to shaders.  

- Moving onto the shaders, we have `basic_uniform.frag` and `basic_uniform.vert` which handle all the scene objects except for the skybox. These shaders handle all the lighting (including the blinn phong algorithm), materials, texturing, normals, alpha and fog.  
```glsl
vec3 blinnPhong(int light, vec3 pos, vec3 normal, vec3 baseColour) {
    vec3 ambient = lights[light].La * baseColour;

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
```

- Next is `skybox.frag` and `skybox.vert` which handle all the skybox logic. This includes mixing the day and night skyboxes, adding the coloured highlights/glares of where the sun and moon are onto the skybox, and gamma correction.  
```glsl
void main() {
    vec3 dayColour = (texture(daySkyboxTex, normalize(TexCoords)).rgb) * 1.5f;
    vec3 nightColour = texture(nightSkyboxTex, normalize(TexCoords)).rgb;

    vec3 skyColour = mix(nightColour, dayColour, blendFactor);

    // Calculate sun and moon visuals on skybox
    vec3 dir = normalize(TexCoords);
    skyColour += sunColour * pow(max(dot(dir, sunDirection), 0.0f), 100.0f);
    skyColour += moonColour * pow(max(dot(dir, moonDirection), 0.0f), 250.0f);

    // Gamma correction
    skyColour = pow(skyColour, vec3(1.0f / 2.2f));

    FragColor = vec4(skyColour, 1.0f);
}
```

- Finally there is `fbo.frag` and `fbo.vert` which handle the framebuffer post processing, including the vignette and brightness correction.  
```glsl
void main() {
    // Sample HDR scene
    vec3 colour = texture(sceneTex, FragUV).rgb;

    // Increase brightness/exposure slightly
    colour *= 1.3f;

    // Calculate distance from centre for vignette
    vec2 centredUV = FragUV - vec2(0.5f);
    float distance = length(centredUV);

    // Apply vignette
    float vignette = smoothstep(vignetteStrength, 0.95f, 1.0f - distance);
    colour *= vignette;

    FragColor = vec4(colour, 1.0f);
}
```

---

## Evaluation
Overall, I believe this project meets the objectives and expectations of the coursework well, successfully demonstrating an OpenGL project utilising custom shading models implemented in GLSL, with custom lighting and showcasing an aestheticly pleasing scene. I have achieved all the basic features listed and most of the advanced features, such as a original working product that has blinn-phong lighting with fog and multi-light, alpha support, normalmaps, skyboxes, cohesive light animation and keyboard and mouse controls, as well as other advanced features.  

As far as potential improvements to the project are concerned, I would like to have added HDR bloom to the framebuffer post processing shader to improve the look of the lighting and highlights. This is especially true during the day, which I think currently looks worse than the night, due to the less interesting/varied lighting which bloom could help to fix.  