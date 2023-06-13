#include "Application.h"

static void frameBufferSizeCallbackFWD(GLFWwindow* window, int width, int height);
static void mouseCallbackFWD(GLFWwindow* window, double xpos, double ypos);
static void scrollCallbackFWD(GLFWwindow* window, double xpos, double ypos);
static void mouseButtonCallbackFWD(GLFWwindow* window, int button, int action, int mods);

Application::Application()
{
}

void Application::init() {
	if (!startGLFW())
		return;
	window.createWindow(800, 600, "Llama Asset Editor");
	setCallBackFuncs();
	if (!loadGlad())
		return;
	glViewport(0, 0, window.bufferWidth, window.bufferHeight);
	configureRender();

	loadShaders();
	loadModel();
	grid.initGrid();

	framebuffer.initFrameBuffer(window.bufferWidth, window.bufferHeight);
	gui.initGui(&window, &model, &mouse, &framebuffer, &camera, &mats);
	//gui.initGui(this);


}

Application::~Application() {
	gui.cleanUp();
	framebuffer.cleanUp();
	glfwDestroyWindow(window.windowPointer);
	glfwTerminate();
}

void Application::RenderLoop() {
	while (!glfwWindowShouldClose(window.windowPointer)) {
		updateDeltaTime();
		processInput();

		gui.displayGui();

		renderModel();

		gui.endFrame();

		glfwSwapBuffers(window.windowPointer);
		glfwPollEvents();
	}
}

bool Application::startGLFW()
{
	if (!glfwInit()) {
		std::cout << "ERROR::APPLICATION::GLFW initialization failed" << std::endl;
		glfwTerminate();
		return false;
	}
	return true;
}

void Application::setCallBackFuncs()
{
	glfwSetFramebufferSizeCallback(window.windowPointer, frameBufferSizeCallbackFWD);
	glfwSetCursorPosCallback(window.windowPointer, mouseCallbackFWD);
	glfwSetScrollCallback(window.windowPointer, scrollCallbackFWD);
	glfwSetMouseButtonCallback(window.windowPointer, mouseButtonCallbackFWD);
}

void Application::framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	this->window.bufferWidth = width;
	this->window.bufferHeight = height;
}

void Application::mouseCallback(GLFWwindow* window, double xpos, double ypos)
{
	mouse.setxPos(xpos);
	mouse.setyPos(ypos);
	if (mouse.getfirstMouse()) {
		mouse.setLastX(xpos);
		mouse.setLastY(ypos);
		mouse.setfirstMouse(false);
	}
	float xoffset = xpos - mouse.getLastX();
	float yoffset = mouse.getLastY() - ypos; // reversed since y-coordinates range from bottom to top.
	mouse.setLastX(xpos);
	mouse.setLastY(ypos);
	if (xoffset > WIN_WIDTH / 2.0)
		return;
	if (yoffset > WIN_HEIGHT / 2.0)
		return;
	if (xoffset < (-1 * (int)WIN_WIDTH) / 2.0)
		return;
	if (yoffset < (-1 * (int)WIN_HEIGHT) / 2.0)
		return;
	camera.ProcessMouseMovement(xoffset, yoffset, true);
}

void Application::scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
	if (mouse.getis3DViewerFocused())
		camera.ProcessMouseScroll((float)yoffset);
}

void Application::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
	if (mouse.getis3DViewerFocused()) {
		if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS) {
			mouse.setKeepCursorInFrame(true);
			camera.canOrbit = true;
		}
		if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_RELEASE) {
			mouse.setKeepCursorInFrame(false);
			camera.canOrbit = false;
		}
		if (button == GLFW_MOUSE_BUTTON_5 && action == GLFW_PRESS) {
			mouse.setKeepCursorInFrame(true);
			camera.isMousePan = true;
			camera.canOrbit = true;
		}
		if (button == GLFW_MOUSE_BUTTON_5 && action == GLFW_RELEASE) {
			mouse.setKeepCursorInFrame(false);
			camera.isMousePan = false;
			camera.canOrbit = false;
		}
		if (button == GLFW_MOUSE_BUTTON_LEFT) {
			mouse.setisLMBPressed(action == GLFW_PRESS);
		}
	}
}

bool Application::loadGlad() {
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "ERROR::APPLICATION::GLAD::Failed to initialize GLAD" << std::endl;
		return false;
	}
	return true;
}

void Application::configureRender() {
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Application::loadShaders() {
	basicShader = Shader("basic.vert", "basic.frag");
	modelShader = Shader("model.vert", "model.frag");
	vertShader = Shader("vertice.vert", "vertice.frag");
}

void Application::loadModel(const std::string name) {
	model = Model(name);
}

void Application::loadModel() {
	loadModel("test.obj");
}

void Application::updateDeltaTime() {
	float currentFrame = glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;
}

void Application::processInput() {
	if (glfwGetKey(window.windowPointer, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window.windowPointer, true);
	if (glfwGetKey(window.windowPointer, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window.windowPointer, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window.windowPointer, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window.windowPointer, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
	if (glfwGetKey(window.windowPointer, GLFW_KEY_Q) == GLFW_PRESS)
		camera.ProcessKeyboard(DOWN, deltaTime);
	if (glfwGetKey(window.windowPointer, GLFW_KEY_E) == GLFW_PRESS)
		camera.ProcessKeyboard(UP, deltaTime);
	if (glfwGetKey(window.windowPointer, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		camera.isShiftPan = true;
	if (glfwGetKey(window.windowPointer, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
		camera.isShiftPan = false;
}

void Application::renderModel() {
	//Utilities::MVP mats;
	mats.projection = glm::perspective(glm::radians(camera.Zoom), (float)window.viewerWidth / (float)window.viewerHeight, 0.1f, 100.0f);
	mats.view = camera.GetViewMatrix();

	framebuffer.bindFramebuffer();

	glEnable(GL_DEPTH_TEST);
	glClearColor(0.15f, 0.161f, 0.22f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	modelShader.use();
	mats.model = glm::mat4(1.0f);
	modelShader.setMatrices(mats.projection, mats.view, mats.model);
	modelShader.setVec3("cameraPos", camera.Position);
	modelShader.setInt("selectedIndex", 10);
	vertShader.use();
	vertShader.setMatrices(mats.projection, mats.view, mats.model);
	vertShader.setVec3("viewPos", camera.Position);

	model.Draw(modelShader, &vertShader, false);
	grid.render(mats.projection, mats.view, camera.Position, camera.Radius);

	framebuffer.unbindFramebuffer();

	
}

Application* app = NULL;

static void frameBufferSizeCallbackFWD(GLFWwindow* window, int width, int height) {
	app->framebufferSizeCallback(window, width, height);
}
static void mouseCallbackFWD(GLFWwindow* window, double xpos, double ypos) {
	app->mouseCallback(window, xpos, ypos);
}
static void scrollCallbackFWD(GLFWwindow* window, double xpos, double ypos) {
	app->scrollCallback(window, xpos, ypos);
}
static void mouseButtonCallbackFWD(GLFWwindow* window, int button, int action, int mods) {
	app->mouseButtonCallback(window, button, action, mods);
}

int main() {
	app = new Application();
	app->init();
	app->RenderLoop();

	return 0;
}