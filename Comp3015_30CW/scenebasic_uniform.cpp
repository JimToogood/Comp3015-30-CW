#include <cstdio>
#include <cstdlib>
#include <string>
#include <iostream>
#include "scenebasic_uniform.h"
#include "helper/glutils.h"

using namespace std;
using namespace glm;


SceneBasic_Uniform::SceneBasic_Uniform() :
    window(nullptr),
    torus(0.7f, 0.3f, 100, 100),
    plane(10.0f, 10.0f, 1, 1),
    torusModel(mat4(1.0f)),
    planeModel(mat4(1.0f)),
    meshModel(mat4(1.0f)),
    skyboxDayTexture(0),
    skyboxNightTexture(0),
    diffuseTexture(0),
    normalTexture(0),
    FBO(0),
    FBOColourTexture(0),
    FBODepthTexture(0),
    screenQuadVAO(0),
    screenQuadVBO(0),
    camera(1280, 720),
    deltaTime(0.0f),
    lastFrame(0.0f),
    timeOfDay(0.0f),
    dayLength(60.0f)    // in seconds
{
    mesh = ObjMesh::load("media/pig_triangulated.obj", true);
    skybox = new SkyBox(50.0f);
}

void SceneBasic_Uniform::initScene(GLFWwindow* winIn) {
    compile();
    window = winIn;

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);    // Automatically bind cursor to window & hide pointer

    initFBO(1280, 720);

    // Load textures
    skyboxDayTexture = Texture::loadHdrCubeMap("media/skybox/day_skybox");
    skyboxNightTexture = Texture::loadHdrCubeMap("media/skybox/night_skybox");

    diffuseTexture = Texture::loadTexture("media/grass.jpg");
    normalTexture = Texture::loadTexture("media/grass_normal.jpg");

    // Assign textures to shaders
    skyboxProg.use();
    skyboxProg.setUniform("daySkyboxTex", 0);
    skyboxProg.setUniform("nightSkyboxTex", 1);

    prog.use();
    prog.setUniform("DiffuseTex", 0);
    prog.setUniform("NormalTex", 1);

    // Model transforms 
    torusModel = rotate(torusModel, radians(-35.0f), vec3(1.0f, 0.0f, 0.0f));
    torusModel = rotate(torusModel, radians(15.0f), vec3(0.0f, 1.0f, 0.0f));
    torusModel = translate(torusModel, vec3(0.0f, 1.1f, 0.0f));

    meshModel = rotate(meshModel, radians(90.0f), vec3(0.0f, 1.0f, 0.0f));
    meshModel = translate(meshModel, vec3(2.0f, 0.0f, 2.0f));
    meshModel = translate(meshModel, vec3(0.0f, 0.5f, 0.0f));

    projection = mat4(1.0f);
    view = camera.GetView();

    // lights[0] = sun, lights[1] = moon
    prog.setUniform("numLights", 2);
}

void SceneBasic_Uniform::initFBO(int windowWidth, int windowHeight) {
    // Create framebuffer object (FBO)
    glGenFramebuffers(1, &FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);

    // Colour buffer texture
    glGenTextures(1, &FBOColourTexture);
    glBindTexture(GL_TEXTURE_2D, FBOColourTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, windowWidth, windowHeight, 0, GL_RGB, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, FBOColourTexture, 0);

    // Depth buffer texture
    glGenTextures(1, &FBODepthTexture);
    glBindTexture(GL_TEXTURE_2D, FBODepthTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, windowWidth, windowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, FBODepthTexture, 0);

    // Check FBO initialised correctly
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        cerr << "FBO failed to initialise." << endl;
        exit(EXIT_FAILURE);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Create fullscreen quad
    float quadVertices[] = {
        // Position     // UV
        -1.0f, -1.0f,   0.0f, 0.0f,
         1.0f, -1.0f,   1.0f, 0.0f,
        -1.0f,  1.0f,   0.0f, 1.0f,
         1.0f,  1.0f,   1.0f, 1.0f,
    };

    glGenVertexArrays(1, &screenQuadVAO);
    glGenBuffers(1, &screenQuadVBO);

    glBindVertexArray(screenQuadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, screenQuadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    FBOProg.use();
    FBOProg.setUniform("sceneTex", 0);
    FBOProg.setUniform("texelSize", vec2(1.0f / windowWidth, 1.0f / windowHeight));
}

void SceneBasic_Uniform::compile() {
	try {
        FBOProg.compileShader("shader/fbo.vert");
        FBOProg.compileShader("shader/fbo.frag");
        FBOProg.link();

        skyboxProg.compileShader("shader/skybox.vert");
        skyboxProg.compileShader("shader/skybox.frag");
        skyboxProg.link();

		prog.compileShader("shader/basic_uniform.vert");
		prog.compileShader("shader/basic_uniform.frag");
		prog.link();
	} catch (GLSLProgramException &e) {
		cerr << e.what() << endl;
		exit(EXIT_FAILURE);
	}
}

void SceneBasic_Uniform::update(float t) {
    // Calculate delta time
    deltaTime = t - lastFrame;
    lastFrame = t;

    timeOfDay += deltaTime;
    if (timeOfDay > dayLength) { timeOfDay -= dayLength; }

    float phase = timeOfDay / dayLength;
    float sunAngle = phase * 2.0f * pi<float>();

    // Sun and moon orbit opposite to each other
    vec3 sunDirection = normalize(vec3(cos(sunAngle), sin(sunAngle), 0.0f));
    vec3 moonDirection = -sunDirection;

    // Define length of overlap between sun and moon at sunset/sunrise
    const float fade = 0.35f;

    float sunIntensity = clamp((sunDirection.y + fade) / (fade * 2.0f), 0.0f, 1.0f);
    float moonIntensity = clamp((moonDirection.y + fade) / (fade * 2.0f), 0.0f, 1.0f);

    // Lighting colour
    const vec3 sunColour = vec3(1.0f, 0.95f, 0.7f);
    const vec3 moonColour = vec3(0.3f, 0.3f, 0.45f);

    // Fog colour
    const vec3 fogDay = vec3(0.7f, 0.8f, 0.9f);
    const vec3 fogNight = vec3(0.07f, 0.07f, 0.1f);

    // Pass skybox variables to skybox shader
    skyboxProg.use();
    skyboxProg.setUniform("blendFactor", sunIntensity);
    skyboxProg.setUniform("sunDirection", sunDirection);
    skyboxProg.setUniform("moonDirection", moonDirection);
    skyboxProg.setUniform("sunColour", sunColour * sunIntensity);
    skyboxProg.setUniform("moonColour", moonColour * moonIntensity);

    prog.use();
    prog.setUniform("CameraPos", camera.GetPos());

    // Pass lighting and fog variables to default shader
    prog.setUniform("lights[0].Position", vec4((sunDirection * 10.0f), 0.0f));
    prog.setUniform("lights[0].Ld", sunColour * sunIntensity);
    prog.setUniform("lights[0].La", (sunColour * 0.2f) * sunIntensity);
    prog.setUniform("lights[0].Ls", sunColour * sunIntensity);

    prog.setUniform("lights[1].Position", vec4((moonDirection * 7.5f), 0.0f));
    prog.setUniform("lights[1].Ld", moonColour * moonIntensity);
    prog.setUniform("lights[1].La", (moonColour * 0.4f) * moonIntensity);
    prog.setUniform("lights[1].Ls", (moonColour * 0.55f) * moonIntensity);

    prog.setUniform("Fog.Density", mix(0.03f, 0.08f, moonIntensity));
    prog.setUniform("Fog.Colour", mix(fogDay, fogNight, moonIntensity));

    FBOProg.use();
    FBOProg.setUniform("vignetteStrength", mix(-0.7f, 0.1f, moonIntensity));

    // -=-=- Handle Player Input -=-=-
    // Close window on escape pressed
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }

    camera.HandleKeyboard(window, deltaTime);

    double x, y;
    glfwGetCursorPos(window, &x, &y);
    camera.HandleMouse(x, y);
}

void SceneBasic_Uniform::render() {
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
}

void SceneBasic_Uniform::renderSceneObjects() {
    view = camera.GetView();

    // -=-=- Skybox -=-=-
    glDepthFunc(GL_LEQUAL);
    skyboxProg.use();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxDayTexture);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxNightTexture);

    mat4 skyboxMVP = projection * mat4(mat3(view)) * mat4(1.0f);
    skyboxProg.setUniform("MVP", skyboxMVP);

    skybox->render();

    // -=-=- Plane -=-=-
    glDepthFunc(GL_LESS);
    prog.use();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, diffuseTexture);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, normalTexture);

    // Materials
    prog.setUniform("UseTexture", true);
    prog.setUniform("Material.Shininess", 120.0f);
    prog.setUniform("Material.Ka", vec3(0.5f));
    prog.setUniform("Material.Ks", vec3(0.05f));

    // Render
    setMatrices(planeModel);
    plane.render();

    // -=-=- Mesh -=-=-
    // Materials
    prog.setUniform("UseTexture", false);
    prog.setUniform("Material.Shininess", 90.0f);
    prog.setUniform("Material.Kd", vec3(0.55f, 0.2f, 0.9f));
    prog.setUniform("Material.Ka", vec3(0.55f, 0.2f, 0.9f));
    prog.setUniform("Material.Ks", vec3(0.9f));

    // Render
    setMatrices(meshModel);
    mesh->render();

    // -=-=- Torus -=-=-
    // Materials
    prog.setUniform("Material.Shininess", 100.0f);
    prog.setUniform("Material.Kd", vec3(0.9f, 0.55f, 0.2f));
    prog.setUniform("Material.Ka", vec3(0.9f, 0.55f, 0.2f));
    prog.setUniform("Material.Ks", vec3(0.8f));

    // Render
    setMatrices(torusModel);
    torus.render();
}

void SceneBasic_Uniform::resize(int w, int h) {
    glViewport(0, 0, w, h);
    width = w;
    height = h;
    projection = perspective(radians(70.0f), (float)w / h, 0.3f, 100.0f);
}

void SceneBasic_Uniform::setMatrices(mat4 model) {
    prog.setUniform("ModelMatrix", model);
    prog.setUniform("NormalMatrix", mat3(transpose(inverse(model))));
    prog.setUniform("MVP", projection * view * model);
}
