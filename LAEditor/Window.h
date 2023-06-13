#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include "Utilities.h"

class Window
{
public:
	GLFWwindow* windowPointer;
	int bufferWidth, bufferHeight;
	double viewerWidth, viewerHeight, viewerXPos, viewerYPos;

	Window() {
		GLFWwindow* windowPointer = NULL;
	}

	~Window() {}

	void createWindow(int width, int height, const char* name) {
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);


		GLFWwindow* temp = glfwCreateWindow(width, height, name, NULL, NULL);
		if (!temp) {
			std::cout << "ERROR::APPLICATION::GLFW window creation failed" << std::endl;
			glfwTerminate();
			return;
		}
		windowPointer = temp;
		glfwGetFramebufferSize(windowPointer, &bufferWidth, &bufferHeight);
		glfwMakeContextCurrent(windowPointer);

		return;
	}

	glm::vec2 getWindowSize() {
		return glm::vec2(bufferWidth, bufferHeight);
	}

	glm::vec2 getViewerSize() {
		return glm::vec2(viewerWidth, viewerHeight);
	}

	glm::vec2 getViewerPos() {
		return glm::vec2(viewerXPos, viewerYPos);
	}

private:
};