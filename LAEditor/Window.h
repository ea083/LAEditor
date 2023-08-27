#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include "Utilities.h"
#include "Settings.h"


class Window
{
public:
	Window() :
		bufferWidth(Settings::WIN_WIDTH),
		bufferHeight(Settings::WIN_HEIGHT),
		viewerWidth(Settings::WIN_WIDTH),
		viewerHeight(Settings::WIN_HEIGHT),
		viewerXPos(0),
		viewerYPos(0),
		windowPointer(nullptr) {
	}

	~Window() {
	}

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

	glm::ivec2 getBufferSize() {
		return glm::vec2(bufferWidth, bufferHeight);
	}

	void setBufferSize(glm::ivec2 newSize) {
		bufferWidth = newSize.x;
		bufferHeight = newSize.y;
	}

	glm::vec2 getViewerSize() {
		return glm::vec2(viewerWidth, viewerHeight);
	}

	void setViewerSize(glm::vec2 newSize) {
		viewerWidth = newSize.x;
		viewerHeight = newSize.y;
	}

	glm::vec2 getViewerPos() {
		return glm::vec2(viewerXPos, viewerYPos);
	}

	void setViewerPos(glm::vec2 newPos) {
		viewerXPos = newPos.x;
		viewerYPos = newPos.y;
	}

	GLFWwindow* getPointer() {
		return windowPointer;
	}

private:
	GLFWwindow* windowPointer;
	int bufferWidth, bufferHeight;
	double viewerWidth, viewerHeight, viewerXPos, viewerYPos;
};