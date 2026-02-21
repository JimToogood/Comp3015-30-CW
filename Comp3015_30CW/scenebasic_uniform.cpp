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
    camera(1280, 720),
    deltaTime(0.0f),
    lastFrame(0.0f)
{
    mesh = ObjMesh::load("media/pig_triangulated.obj", true);
}

void SceneBasic_Uniform::initScene(GLFWwindow* winIn) {
    compile();
    window = winIn;

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);    // Automatically bind cursor to window & hide pointer

    // Model transforms
    torusModel = rotate(torusModel, radians(-35.0f), vec3(1.0f, 0.0f, 0.0f));
    torusModel = rotate(torusModel, radians(15.0f), vec3(0.0f, 1.0f, 0.0f));

    planeModel = translate(planeModel, vec3(0.0f, -1.0f, 0.0f));

    meshModel = rotate(meshModel, radians(90.0f), vec3(0.0f, 1.0f, 0.0f));
    meshModel = translate(meshModel, vec3(2.0f, 0.0f, 2.0f));

    projection = mat4(1.0f);
    view = camera.GetView();

    prog.setUniform("numLights", 2);

    // Position
    prog.setUniform("lights[0].Position", vec4(5.0f, 5.0f, 5.0f, 1.0f));
    prog.setUniform("lights[1].Position", vec4(-15.0f, 5.0f, 15.0f, 1.0f));
    //prog.setUniform("lights[2].Position", vec4(-5.0f, 5.0f, 2.0f, 1.0f));

    // Diffuse (Colour)
    prog.setUniform("lights[0].Ld", vec3(1.0f));
    prog.setUniform("lights[1].Ld", vec3(1.0f));
    //prog.setUniform("lights[2].Ld", vec3(0.0f, 0.0f, 0.8f));

    // Ambient
    prog.setUniform("lights[0].La", vec3(0.3f));
    prog.setUniform("lights[1].La", vec3(0.3f));
    //prog.setUniform("lights[2].La", vec3(0.0f, 0.0f, 0.1f));

    // Specular
    prog.setUniform("lights[0].Ls", vec3(1.0f));
    prog.setUniform("lights[1].Ls", vec3(1.0f));
    //prog.setUniform("lights[2].Ls", vec3(0.8f));

    // Fog
    prog.setUniform("Fog.Density", 0.06f);  // 0.08f = dense fog
    prog.setUniform("Fog.Colour", vec3(0.5f));
}

void SceneBasic_Uniform::compile() {
	try {
		prog.compileShader("shader/basic_uniform.vert");
		prog.compileShader("shader/basic_uniform.frag");
		prog.link();
		prog.use();
	} catch (GLSLProgramException &e) {
		cerr << e.what() << endl;
		exit(EXIT_FAILURE);
	}
}

void SceneBasic_Uniform::update(float t) {
    // Calculate delta time
    deltaTime = t - lastFrame;
    lastFrame = t;

    prog.setUniform("CameraPos", camera.GetPos());

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
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    view = camera.GetView();

    // -=-=- Torus -=-=-
    // Materials
    prog.setUniform("Material.Shininess", 100.0f);
    prog.setUniform("Material.Kd", vec3(0.9f, 0.55f, 0.2f));
    prog.setUniform("Material.Ka", vec3(0.7f, 0.35f, 0.1f));
    prog.setUniform("Material.Ks", vec3(0.8f));
    
    // Render
    setMatrices(torusModel);
    torus.render();

    // -=-=- Plane -=-=-
    // Materials
    prog.setUniform("Material.Shininess", 120.0f);
    prog.setUniform("Material.Kd", vec3(0.1f, 0.3f, 0.1f));
    prog.setUniform("Material.Ka", vec3(0.05f, 0.2f, 0.05f));
    prog.setUniform("Material.Ks", vec3(0.05f));

    // Render
    setMatrices(planeModel);
    plane.render();

    // -=-=- Mesh -=-=-
    // Materials
    prog.setUniform("Material.Shininess", 100.0f);
    prog.setUniform("Material.Kd", vec3(0.55f, 0.2f, 0.9f));
    prog.setUniform("Material.Ka", vec3(0.35f, 0.1f, 0.7f));
    prog.setUniform("Material.Ks", vec3(0.9f));

    // Render
    setMatrices(meshModel);
    mesh->render();
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
