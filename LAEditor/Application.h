#pragma once
//#include "imgui.h"
//#include "imgui_impl_glfw.h"
//#include "imgui_impl_opengl3.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

//#include <glm/glm.hpp>
//#include <glm/gtc/matrix_transform.hpp>
//#include <glm/gtc/type_ptr.hpp>

//#include "Framebuffer.h"
//#include "Shader.h"
#include "Camera.h"
//#include "Model.h"
//#include "Grid.h"
//#include "Gui.h"
#include "Mouse.h"
//#include "PickingTexture.h"

#include <iostream>
#include <string>


struct Window {
	int bufferWidth, bufferHeight;
};

static const unsigned int WIN_WIDTH = 800, WIN_HEIGHT = 600;

class Application
{
public:
	Application();
	~Application();

private:
	static GLFWwindow* mainWindow;
	static Window mainWindowInfo;
	static Mouse mouse;
	static Camera camera;

	static bool startGLFW();
	static bool createWindow(int width, int height, const char* name);
	static bool createDefaultWindow();
	
	static void framebufferSizeCallback(GLFWwindow* window, int width, int height);
	static void mouseCallback(GLFWwindow* window, double xpos, double ypos);
};
