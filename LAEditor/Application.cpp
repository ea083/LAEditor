#include "Application.h"

Application::Application()
{
	this->mainWindow= nullptr;
	mainWindowInfo = Window();
	mouse = Mouse();
	camera = Camera();
	if (!startGLFW())
		return;
	if (!createDefaultWindow())
		return;

}

Application::~Application()
{
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

bool Application::createWindow(int width, int height, const char* name)
{
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	mainWindow = glfwCreateWindow(width, height, name, NULL, NULL);
	if (!mainWindow) {
		std::cout << "ERROR::APPLICATION::GLFW window creation failed" << std::endl;
		return false;
	}

	glfwGetFramebufferSize(mainWindow, &mainWindowInfo.bufferWidth, &mainWindowInfo.bufferHeight);
	glfwMakeContextCurrent(mainWindow);
	glViewport(0, 0, mainWindowInfo.bufferWidth, mainWindowInfo.bufferHeight);

	return true;
}

bool Application::createDefaultWindow()
{
	return createWindow(800, 600, "Llama Asset Editor");
}

void Application::framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	mainWindowInfo.bufferWidth = width;
	mainWindowInfo.bufferHeight = height;
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
