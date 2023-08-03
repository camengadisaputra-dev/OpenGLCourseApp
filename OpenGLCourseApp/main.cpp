#include <stdio.h>
#include <string.h>
#include <math.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

const GLint WIDTH = 800, HEIGHT = 600;
const float toRadians = (22.0f/7.0f) / 180.0f;

GLuint VAO, VBO, shader, uniformModel;

bool direction = true;
float triOffset = 0.0f;
float triMaxOffset = 0.7f;
float triIncrement = 0.005f;

// Vertex Shader
static const char* vShader = "                                      \n\
#version 330                                                        \n\
                                                                    \n\
layout (location = 0) in vec3 pos;                                  \n\
                                                                    \n\
uniform mat4 model;                                                 \n\
                                                                    \n\
void main() {                                                       \n\
    gl_Position = model * vec4(pos, 1.0); \n\
}";

// Fragment Shader
static const char* fShader = "                                  \n\
#version 330                                                    \n\
                                                                \n\
out vec4 color;                                                 \n\
                                                                \n\
void main() {                                                   \n\
    color = vec4(1.0, 0.0, 0.0, 1.0);                           \n\
}";

void CreateTriangle() {
    GLfloat vertices[] = {
        -1.0f, -1.0f, 0.0f,
        1.0f, -1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
    };

    //gen and bin VAO
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    //gen and bin VBO
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    //unbind buffer
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    //unbind vertex array
    glBindVertexArray(0);
}

void AddsShader(GLuint theProgram, const char* shaderCode, GLenum shaderType) {
    GLuint theShader = glCreateShader(shaderType);

    const GLchar* theCode[1];
    theCode[0] = shaderCode;

    GLint codeLength[1];
    codeLength[0] = strlen(shaderCode);

    glShaderSource(theShader, 1, theCode, codeLength);
    glCompileShader(theShader);

    GLint result = 0;
    GLchar eLog[1024] = { 0 };

    glGetShaderiv(theShader, GL_COMPILE_STATUS, &result);
    if (!result) {
        glGetShaderInfoLog(theShader, sizeof(eLog), NULL, eLog);
        printf("Error compiling the %d shader: '%s'\n", shaderType ,eLog);
        return;
    }

    glAttachShader(theProgram, theShader);
}

void CompileShaders() {
    shader = glCreateProgram();

    if (!shader) {
        printf("Error creating shader program!");
        return;
    }

    AddsShader(shader, vShader, GL_VERTEX_SHADER);
    AddsShader(shader, fShader, GL_FRAGMENT_SHADER);

    GLint result = 0;
    GLchar eLog[1024] = { 0 };

    glLinkProgram(shader);
    glGetProgramiv(shader, GL_LINK_STATUS, &result);
    if (!result) {
        glGetProgramInfoLog(shader, sizeof(eLog), NULL, eLog);
        printf("Error linking program: '%s'\n", eLog);
        return;
    }

    glValidateProgram(shader);
    glGetProgramiv(shader, GL_VALIDATE_STATUS, &result);
    if (!result) {
        glGetProgramInfoLog(shader, sizeof(eLog), NULL, eLog);
        printf("Error validate program: '%s'\n", eLog);
        return;
    }

    uniformModel = glGetUniformLocation(shader, "model");
}


int main()
{
    //initialize GLFW
    if (!glfwInit()) {
        printf("GFLW initialisation failed!");
        glfwTerminate();
        return 1;
    }

    //Setup GLFW window properties
    //OpenGL version
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    //Core profile = No Backward Compability
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //Allow forward compability
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow* mainWindow = glfwCreateWindow(WIDTH, HEIGHT, "OPEN GL WINDOW", NULL, NULL);
    if (!mainWindow) {
        printf("GLFW window creation failed!");
        glfwTerminate();
        return 1;
    }

    //Get Buffer size information
    int bufferWidth, bufferHight;
    glfwGetFramebufferSize(mainWindow, &bufferWidth, &bufferHight);

    //Set contex for GLEW to use
    glfwMakeContextCurrent(mainWindow);

    //Allow modern extension features
    glewExperimental = GL_TRUE;

    if (glewInit() != GLEW_OK) {
        printf("GLEW initialisation FAILED!");
        glfwDestroyWindow(mainWindow);
        glfwTerminate();
        return 1;
    }

    //Setup Viewport Size
    glViewport(0, 0, bufferWidth, bufferWidth);

    CreateTriangle();
    CompileShaders();
    int rotation = 0;
    float scale = 0.4f;
    //Loop until window close
    while (!glfwWindowShouldClose(mainWindow)) {
        // Get + Hanlde user input events
        glfwPollEvents();

        if (direction) {
            triOffset += triIncrement;
        }
        else {
            triOffset -= triIncrement;
        }

        if (fabs(triOffset) >= triMaxOffset) {
            direction = !direction;
        }

        rotation = (rotation + 1) % 360;

        // Clear window
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shader);

        glm::mat4 model(1.0f);
        model = glm::translate(model, glm::vec3(triOffset, 0.0f, 0.0f));
        model = glm::rotate(model, rotation * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::scale(model, glm::vec3(scale, scale, 1.0f));

        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glBindVertexArray(0);


        glUseProgram(0);

        glfwSwapBuffers(mainWindow);
    }

    return 0;
}