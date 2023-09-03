#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Mesh.h"
#include "Shader.h"
#include "GlWindow.h"
#include  "Camera.h"
#include "Texture.h"
#include "Light.h"

const float toRadians = (22.0f/7.0f) / 180.0f;

std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;
GlWindow mainWindow;
Camera camera;

Texture brickTexture;
Texture dirtTexture;

Light mainLight;

GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;

// Vertex Shader
static const char* vShader = "Shaders/shader.vert";

// Fragment Shader
static const char* fShader = "Shaders/shader.frag";

void CreateObject() {
    unsigned int indices[] = {
        0, 3, 1,
        1, 3, 2, 
        2, 3, 0,
        0, 1, 2
    };

    GLfloat vertices[] = {
    //  x       y       z       u
        -1.0f, -1.0f, 0.0f,     0.0f, 0.0f,
        0.0f, -1.0f, 1.0f,      0.5f, 0.0f,
        1.0f, -1.0f, 0.0f,      1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,       0.5f, 1.0f,
    };

    GLfloat vertices2[] = {
        -1.0f, -1.0f, 1.0f,     0.0f, 0.0f,
        0.0f, -1.0f, -1.0f,     0.5f, 0.0f,
        1.0f, -1.0f, 1.0f,      1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,       0.5f, 1.0f
    };

    Mesh* obj1 = new Mesh();
    obj1->CreateMesh(vertices, indices, 20, 12);
    meshList.push_back(obj1);

    Mesh* obj2 = new Mesh();
    obj2->CreateMesh(vertices2, indices, 20, 12);
    meshList.push_back(obj2);
}

void CreateShader() {
    Shader* shader1 = new Shader();
    shader1->CreateFromFile(vShader, fShader);
    shaderList.push_back(*shader1);
}

int main()
{
    mainWindow = GlWindow(800, 600);
    mainWindow.Initialise();

    CreateObject();
    CreateShader();

    camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f, 5.0f, 0.5f);

    brickTexture = Texture((char *)"Textures/brick.png");
    brickTexture.LoadTexture();

    dirtTexture = Texture((char*)"Textures/dirt.png");
    dirtTexture.LoadTexture();

    mainLight = Light(1.0f, 1.0f, 1.0f, 0.8f);

    glm::mat4 projection = glm::perspective(glm::radians(45.0f), mainWindow.getBufferWidth()/mainWindow.getBufferHeight(), 0.1f, 100.0f);

    float scale = 0.4f;

    GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformAmbientIntensity = 0, uniformAmbientColour = 0; 


    //Loop until window close
    while (mainWindow.getShouldClose()) {

        GLfloat now = glfwGetTime(); // SDL_GetPerformanceCount();
        deltaTime = now - lastTime; // (now - lastTime) * 1000/SDL_GetPerformanceFrequency()
        lastTime = now;

        // Get + Hanlde user input events
        glfwPollEvents();

        camera.keyControl(mainWindow.getsKeys(), deltaTime);
        camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

        // Clear window
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shaderList[0].UseShader();
        uniformModel = shaderList[0].GetModelLocation();
        uniformProjection = shaderList[0].GetProjectionLocation();
        uniformView = shaderList[0].GetViewLocation();
        uniformAmbientColour = shaderList[0].GetAmbientColourLocation();
        uniformAmbientIntensity = shaderList[0].GetAmbientIntensityLocation();

        mainLight.UseLight(uniformAmbientIntensity, uniformAmbientColour);

        glm::mat4 model(1.0f);
        model = glm::translate(model, glm::vec3(-1.0f, 0.0f, -3.0f));
        model = glm::scale(model, glm::vec3(scale, scale, 1.0f));

        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
        brickTexture.UseTexture();


        meshList[0]->RenderMesh();

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(1.0f, 0.0f, -3.0f));
        model = glm::scale(model, glm::vec3(scale, scale, 1.0f));

        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        dirtTexture.UseTexture();


        meshList[1]->RenderMesh();

        glUseProgram(0);

        mainWindow.swapBuffers();
    }

    return 0;
}