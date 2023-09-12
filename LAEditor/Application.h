#pragma once

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"


#include <memory>
#include <windows.h>
#include <commdlg.h>
#include <stdlib.h>
#include <atlstr.h>
#include <sstream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <string>

#include "Settings.h"

#include "Log.h"

#include "Framebuffer.h"
#include "Shader.h"
#include "Camera.h"
#include "Model.h"
#include "Grid.h"
#include "Mouse.h"
#include "Window.h"
#include "VerticeDataFramebuffer.h"
#include "Gizmo.h"
#include "Outliner.h"
#include "ActionStateMachine.h"


#ifdef DEBUG
struct AllocationMetrics {
	uint32_t TotalAllocated = 0;
	uint32_t TotalFreed = 0;

	uint32_t currentUsage() { return TotalAllocated - TotalFreed; }
};

static AllocationMetrics s_AllocationMetrics;

void* operator new(size_t size) {
	s_AllocationMetrics.TotalAllocated += size;
	return malloc(size);
}
void operator delete(void* memory, size_t size) {
	s_AllocationMetrics.TotalFreed += size;
	free(memory);
}
static void printMemoryUsage() {
	std::cout << "Memory Usage: " << s_AllocationMetrics.currentUsage() << " bytes" << std::endl;
}
static uint32_t getMemoryUsage() {
	return s_AllocationMetrics.currentUsage();
}
#endif // DEBUG

#ifdef NDEBUG
static void printMemoryUsage() {}

static uint32_t getMemoryUsage() {}
#endif // NDEBUG


class Application {
public:
	Application();
	~Application();

	void init();
	void RenderLoop();

	Window* getWindow() {
		return &window;
	}

	void framebufferSizeCallback(GLFWwindow* window, int width, int height);
	void mouseCallback(GLFWwindow* window, double xpos, double ypos);
	void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
	void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);

private:
	Log log;

	ASM::StateMachine actionStateMachine;

	Window window;
	Outliner outliner;
	Camera camera;
	Mouse mouse;

	Shader basicShader, modelShader, vertShader, selectedVertShader, edgeShader;
	Shader verticeDataShader, uiElementDataShader;
	Shader gizmoShader;

	Model model;

	Grid grid;

	Gizmo gizmo;

	Framebuffer framebuffer;
	VerticeDataFramebuffer verticeDataFramebuffer;

	Utilities::MVP mats;
	Utilities::AppPointers appPointers;

	// TODO: grug
	HWND hwnd;
	OPENFILENAME ofn;
	LPWSTR szFile;
	HANDLE hf;

	float deltaTime, lastFrame;
	bool displayUIFramebuffer = false;
	bool firstClick = true;

	// init functions
	bool startGLFW();
	void setCallBackFuncs();
	bool loadGlad();
	void configureRender();
	void loadShaders();
	void loadModel();
	void loadModel(const std::string name);
	void setAppPointers();
	// render loop functions
	void updateDeltaTime();
	void processInput();
	void renderModel();
	// gui functions
	void initGui();
	void displayGui();
	void endFrame();
	void cleanUpGui();
	void getImGuiIO();
	void initImGui();
	void showDemoWindow();
	// displayGUI loop
	void newFrames();
	void createDockSpace();
	void showMainMenuBar();
	void showDebugGui();
	void processGuiInput();
	void runCurrentState();
	void showObjectInspector();
	void show3DViewer();

	// user Actions
	void startGrab();
	void endAction();
	//legacy actions
	void grab();

	void setCursorPos();
	void updateImGuiTexture();
	void openObjFile();
	void showModelTables(Model* model);
	void showUIElementTables(UIElement* uiElement);

	Utilities::PixelData getDataAtMousePos();
};
