#pragma once

#include <stdio.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

class GlWindow
{
public:
	GlWindow();
	GlWindow(GLint windowWidth, GLint windowHeight);

	int Initialise();

	GLfloat getBufferWidth() { return bufferWidth; }
	GLfloat getBufferHeight() { return bufferHeight;  }

	bool getShouldClose() { return !glfwWindowShouldClose(mainWindow);  }

	bool* getsKeys() { return keys; };
	GLfloat getXChange();
	GLfloat getYChange();

	void swapBuffers() { glfwSwapBuffers(mainWindow); }

	~GlWindow();

private:
	GLFWwindow* mainWindow;
	GLint width, height;
	
	GLint bufferWidth, bufferHeight;

	bool keys[1024]; //asci caracter

	GLfloat lastX;
	GLfloat lastY;
	GLfloat xChange;
	GLfloat yChange;
	bool mouseFirstMoved;


	void createCallbacks();
	static void handleKeys(GLFWwindow* window, int key, int code, int action, int mode);
	static void handleMouse(GLFWwindow* window, double xPos, double yPos);
};

