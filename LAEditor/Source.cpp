#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Framebuffer.h"
#include "Shader.h"
#include "Camera.h"
#include "Model.h"
#include "Grid.h"
#include "Gui.h"

#include <iostream>
#include <string>

GLuint VAO, VBO;

void create_triangle()
{
	float vertices[] = {
		-1.0f, -1.0f, 0.0f, // 1. vertex x, y, z
		1.0f, -1.0f, 0.0f, // 2. vertex ...
		0.0f, 1.0f, 0.0f // etc... 
	};

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}
bool startGlfw();
GLFWwindow* createWindow(int width, int height, const char* name);
void setCallBackFuncs(GLFWwindow* window);
bool loadGlad();
void renderConfig();
void createDockSpace(ImGuiIO& io);
void showDemoWindow();
ImGuiIO& getImGuiIO();
void initImGui(GLFWwindow* window );
void ImGuiNewFrames();
void setImGuiTexture(); 
void processInput(GLFWwindow* window);
void updateDeltaTime();
unsigned int loadCubemap(std::vector<std::string> faces);
void setCursorPos(GLFWwindow* window);
void showDebugGui();


// callback functions
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

// window
const int WIDTH = 800, HEIGHT = 600;
int currWidth = WIDTH, currHeight = HEIGHT;
Framebuffer frame;
// Gui
// cursor
float lastX = WIDTH / 2.0f;
float lastY = HEIGHT / 2.0f;
bool keepCursorInFrame = false;
bool firstMouse = true;
// camera
glm::vec3 cameraPos = glm::vec3(10.0f, 3.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
float yaw = -90.0;
float pitch = glm::radians(30.0f);
float fov = 45.0f;
Camera camera(cameraPos, cameraUp, yaw, pitch, true);
// timing
float deltaTime = 0.0f; // time between current frame and last frame
float lastFrame = 0.0f; // time of last frame
// debug
float uv0 = 0.0, uv1 = 1.0, uv2 = 1.0, uv3 = 0.0;

int main()
{
	if (!startGlfw())
		return 1;
	GLFWwindow* mainWindow = createWindow(WIDTH, HEIGHT, "Llama Asset Editor");
	if (!mainWindow)
		return 1;

	int bufferWidth, bufferHeight;
	glfwGetFramebufferSize(mainWindow, &bufferWidth, &bufferHeight);
	glfwMakeContextCurrent(mainWindow);

	setCallBackFuncs(mainWindow);

	if (!loadGlad())
		return -1;

	glViewport(0, 0, bufferWidth, bufferHeight);

	renderConfig();
	// load shaders
	Shader basicShader = Shader("basic.vs", "basic.fs");
	Shader modelShader("model.vs", "model.fs");
	// load models
	create_triangle();
	Model ourModel("backpack/backpack.obj");
	// create grid
	Grid grid = Grid();
	// init framebuffers
	frame.initFrameBuffer(WIDTH, HEIGHT); 
	// create gui
	Gui gui = Gui(mainWindow, &ourModel);

	ImGuiIO& io = getImGuiIO();
	initImGui(mainWindow);

	while (!glfwWindowShouldClose(mainWindow))
	{
		updateDeltaTime();
		processInput(mainWindow);
		ImGuiNewFrames(); 
		createDockSpace(io);
		ImGui::NewFrame();
		gui.showMainMenuBar();
		showDemoWindow();
		showDebugGui();
		ImGui::Begin("3D Viewer");
		setCursorPos(mainWindow);
		setImGuiTexture();
		const float window_width = ImGui::GetContentRegionAvail().x;
		const float window_height = ImGui::GetContentRegionAvail().y;
		ImGui::End();
		ImGui::Render();

		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)window_width / (float)window_height, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		frame.bindFramebuffer();

		glEnable(GL_DEPTH_TEST);
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		modelShader.use();
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f));
		model = glm::scale(model, glm::vec3(1.0f));
		modelShader.setMatrices(projection, view, model);
		modelShader.setVec3("cameraPos", camera.Position); 
		ourModel.Draw(modelShader);

		grid.render(projection, view, camera.Position, camera.Radius);

		frame.unbindFramebuffer();

		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			GLFWwindow* backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);
		}

		glfwSwapBuffers(mainWindow);
		glfwPollEvents();
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	frame.cleanUp();

	glfwDestroyWindow(mainWindow);
	glfwTerminate();

	return 0;
}

bool startGlfw() {
	if (!glfwInit()) {
		std::cout << "GLFW initialisation failed!\n";
		glfwTerminate();
		return false;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	return true;
}
GLFWwindow* createWindow(int width, int height, const char* name) {
	GLFWwindow* window = glfwCreateWindow(width, height, name, NULL, NULL);
	if (!window) {
		std::cout << "GLFW creation failed\n";
		glfwTerminate();
	}
	return window;
}
void setCallBackFuncs(GLFWwindow* window) {
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
}
bool loadGlad() {
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return false;
	}
	return true;
}
void renderConfig() {
	// configure global opengl state
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_STENCIL_TEST);
	// enable transparency in shaders
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}
void createDockSpace(ImGuiIO& io) {
	bool* p_open;
	static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;
	static bool opt_fullscreen = true;
	static bool opt_padding = false;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
	if (opt_fullscreen)
	{
		const ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->WorkPos);
		ImGui::SetNextWindowSize(viewport->WorkSize);
		ImGui::SetNextWindowViewport(viewport->ID);
		//ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		//ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
	}
}
void showDemoWindow() {
	bool show_demo_window = true;
	ImGui::ShowDemoWindow(&show_demo_window);
}
ImGuiIO& getImGuiIO() {
	ImGui::CreateContext();

	IMGUI_CHECKVERSION();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

	ImGui::StyleColorsDark();
	ImGuiStyle& style = ImGui::GetStyle();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}
	return io;
}
void initImGui(GLFWwindow* window) {

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");
}
void ImGuiNewFrames() {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
}
void setCursorPos(GLFWwindow* window) {
	if (keepCursorInFrame) {

		ImVec2 pos = ImGui::GetCursorScreenPos();
		const float window_width = ImGui::GetContentRegionAvail().x;
		const float window_height = ImGui::GetContentRegionAvail().y;

		double xPos, yPos;
		glfwGetCursorPos(window, &xPos, &yPos);
		if (xPos > currWidth)
			xPos = xPos - currWidth;
		if (xPos < 0)
			xPos += currWidth;
		if (yPos > currHeight)
			yPos -= currHeight;
		if (yPos < 0)
			yPos += currHeight;
		glfwSetCursorPos(window, xPos, yPos);
	}
}
void setImGuiTexture() {
	const float window_width = ImGui::GetContentRegionAvail().x;
	const float window_height = ImGui::GetContentRegionAvail().y;

	frame.rescaleFramebuffer(window_width + (currWidth-window_width), window_height + (currHeight - window_height));
	ImVec2 pos = ImGui::GetCursorScreenPos();
	
	glm::vec4 uv = glm::vec4(0, 1, 1, 0);
	ImGui::GetWindowDrawList()->AddImage(
		(void*)frame.getTextureID(),
		ImVec2(pos.x, pos.y),
		ImVec2(pos.x + window_width, pos.y + window_height),
		ImVec2(uv[0], uv[1]),
		ImVec2(uv[2], uv[3])
	);

}
void processInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		camera.ProcessKeyboard(DOWN, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		camera.ProcessKeyboard(UP, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		camera.isShiftPan = true;
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
		camera.isShiftPan = false;
}
void updateDeltaTime() {
	float currentFrame = glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;
}
unsigned int loadCubemap(std::vector<std::string> faces) {
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrChannels;
	for (unsigned int i = 0; i < faces.size(); i++) {
		unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
		if (data) {
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
		else {
			std::cout << "Cubemap tex failed to load at paht: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
}
void showDebugGui() {

	ImGui::Begin("Debug");
	ImGui::DragFloat("UV0", &uv0, 0.5f);
	ImGui::DragFloat("UV1", &uv1, 0.5f);
	ImGui::DragFloat("UV2", &uv2, 0.5f);
	ImGui::DragFloat("UV3", &uv3, 0.5f);
	ImGui::End();
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	//const float window_width = ImGui::GetContentRegionAvail().x;
	//const float window_height = ImGui::GetContentRegionAvail().y;

	//frame.rescaleFramebuffer(window_width, window_height);
	glViewport(0, 0, width, height);
	currWidth = width;
	currHeight = height;
}
void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	if (firstMouse) {
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}
	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates range from bottom to top.
	lastX = xpos;
	lastY = ypos;
	if (xoffset > WIDTH / 2.0)
		return;
	if (yoffset > HEIGHT / 2.0)
		return;
	if (xoffset < (-1 * (int)WIDTH) / 2.0)
		return;
	if (yoffset < (-1 * (int)HEIGHT) / 2.0)
		return;
	camera.ProcessMouseMovement(xoffset, yoffset, true);
}
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	camera.ProcessMouseScroll((float)yoffset);
}
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
	if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS) {
		keepCursorInFrame = true;
		camera.canOrbit = true;
	}
	if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_RELEASE) {
		keepCursorInFrame = false;
		camera.canOrbit = false;
	}
	if (button == GLFW_MOUSE_BUTTON_5 && action == GLFW_PRESS) {
		keepCursorInFrame = true;
		camera.isMousePan = true;
		camera.canOrbit = true;
	}
	if (button == GLFW_MOUSE_BUTTON_5 && action == GLFW_RELEASE) {
		keepCursorInFrame = false;
		camera.isMousePan = false;
		camera.canOrbit = false;
	}
}