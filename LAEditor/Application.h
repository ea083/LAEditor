#pragma once
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Utilities.h"

#include "Framebuffer.h"
//#include "Shader.h"
#include "Camera.h"
#include "Model.h"
#include "Grid.h"
#include "Gui.h"
#include "Mouse.h"
#include "Window.h"
#include "VerticeDataFramebuffer.h"
#include "Gizmo.h"

#include "Outliner.h"

#include "Model2.h"

#include <iostream>
#include <string>

static const unsigned int WIN_WIDTH = 1200, WIN_HEIGHT = 600;

class Application
{
public:
	Application();
	~Application();

	void init();

	void RenderLoop();

	void framebufferSizeCallback(GLFWwindow* window, int width, int height);
	void mouseCallback(GLFWwindow* window, double xpos, double ypos);
	void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
	void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);

	Window* getWindowPointer() { return &window; }
	Window getWindow() { return window; }
	Model* getModelPointer() { return &model; }
	Mouse* getMousePointer() { return &mouse; }
	Utilities::MVP* getMatsPointer() { return &mats; }
	Utilities::MVP getMats() { return mats; }
	Mouse getMouse() { return mouse; }
	Model getModel() { return model; }
	Camera getCamera() { return camera;}
	Framebuffer getFramebuffer() { return framebuffer; }
	Framebuffer* getFramebufferPointer() { return &framebuffer; }

private:
	Gui gui;
	Mouse mouse;
	Camera camera;
	Window window;
	Shader basicShader, modelShader, vertShader, selectedVertShader, edgeShader;
	Shader verticeDataShader, uiElementDataShader;
	Shader gizmoShader;
	Model model;
	Model2::Model2 model2;
	Grid grid;
	Framebuffer framebuffer;
	VerticeDataFramebuffer verticeDataFramebuffer;
	Gizmo gizmo;

	Outliner outliner;
	Utilities::MVP mats;

	float deltaTime, lastFrame;

	bool startGLFW();
	void setCallBackFuncs();
	bool loadGlad();
	void configureRender();
	void loadShaders();
	void loadModel(const std::string name);
	void loadModel();
	// render loop funcs
	void updateDeltaTime();
	void processInput();
	void renderModel();

	Utilities::PixelData getDataAtMousePos();
};
