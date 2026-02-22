#ifndef SCENEBASIC_UNIFORM_H
#define SCENEBASIC_UNIFORM_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "helper/scene.h"
#include "helper/glslprogram.h"
#include "helper/torus.h"
#include "helper/plane.h"
#include "helper/objmesh.h"
#include "helper/texture.h"
#include "camera.h"

using namespace std;
using namespace glm;


class SceneBasic_Uniform : public Scene {
public:
    SceneBasic_Uniform();

    void initScene(GLFWwindow* window);
    void update(float t);
    void render();
    void resize(int, int);

private:
    GLSLProgram prog;
    GLFWwindow* window;

    Torus torus;
    Plane plane;
    unique_ptr<ObjMesh> mesh;

    mat4 torusModel;
    mat4 planeModel;
    mat4 meshModel;

    Camera camera;
    float deltaTime;
    float lastFrame;

    void setMatrices(mat4 model);
    void compile();
};

#endif  // SCENEBASIC_UNIFORM_H
