#include "Application.h"

static void frameBufferSizeCallbackFWD(GLFWwindow* window, int width, int height);
static void mouseCallbackFWD(GLFWwindow* window, double xpos, double ypos);
static void scrollCallbackFWD(GLFWwindow* window, double xpos, double ypos);
static void mouseButtonCallbackFWD(GLFWwindow* window, int button, int action, int mods);

Application* callBackPointer = nullptr;
int main() {
	Application app = Application();
	//app = Application();
	callBackPointer = &app;

	app.init();
	app.RenderLoop();
	printMemoryUsage();
	return 0;
}

Application::Application() : window(Window())
{
	
}
Application::~Application()
{
	cleanUpGui();
	framebuffer.cleanUp();
	verticeDataFramebuffer.cleanUp();
	glfwDestroyWindow(window.getPointer());
	glfwTerminate();
}
void Application::RenderLoop() {
	while (!glfwWindowShouldClose(window.getPointer())) {
		updateDeltaTime();
		processInput();

		// display gui
		displayGui();

		renderModel();

		// end gui frame
		endFrame();

		mouse.setIn3DWindow(getDataAtMousePos().b == 1);

		glfwSwapBuffers(window.getPointer());
		glfwPollEvents();
	}
}

void Application::init() {
	if (!startGLFW())
		return;
	window.createWindow(Settings::WIN_WIDTH, Settings::WIN_HEIGHT, Settings::mainWinName);
	setCallBackFuncs();
	if (!loadGlad())
		return;
	glViewport(0, 0, window.getBufferSize().x, window.getBufferSize().y);
	configureRender();

	outliner.init();

	loadShaders();
	loadModel();
	grid.initGrid();
	gizmo.init();
	gizmo.addToOutliner(&outliner);

	framebuffer.initFrameBuffer(window.getBufferSize().x, window.getBufferSize().y);
	verticeDataFramebuffer.initVerticeDataFramebuffer(window.getBufferSize().x, window.getBufferSize().y);
	initGui();

	log.Init();
	log.GetCoreLogger()->warn("Initialized Log!");
	log.logConsole("initialized again");
	log.logConsoleError("error test");
	log.logConsoleWarning("warning test");

	setAppPointers();
	actionStateMachine.init(std::make_shared<ASM::IdleState>(), appPointers);
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
	glfwSetFramebufferSizeCallback(window.getPointer(), frameBufferSizeCallbackFWD);
	glfwSetCursorPosCallback(window.getPointer(), mouseCallbackFWD);
	glfwSetScrollCallback(window.getPointer(), scrollCallbackFWD);
	glfwSetMouseButtonCallback(window.getPointer(), mouseButtonCallbackFWD);
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
	selectedVertShader = Shader("vertice.vert", "selectedVertice.frag");
	edgeShader = Shader("edge.vert", "edge.frag");
	verticeDataShader = Shader("verticeData.vert", "verticeData.frag");
	gizmoShader = Shader("gizmo.vert", "gizmo.frag");
	uiElementDataShader = Shader("uiElementData.vert", "uiElementData.frag");
}

void Application::loadModel() {
	loadModel("test.obj");
}
void Application::loadModel(const std::string name) {
	model = Model(name);
	outliner.addModel(&model);
}
void Application::setAppPointers() {
	appPointers.mouse = &mouse;
	appPointers.model = &model;
	appPointers.camera = &camera;
	appPointers.window = &window;
	appPointers.log = &log;
	appPointers.verticeDataFramebuffer = &verticeDataFramebuffer;
	appPointers.outliner = &outliner;
}

void Application::updateDeltaTime() {
	float currentFrame = (float)glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;
}
void Application::processInput() {
	if (glfwGetKey(window.getPointer(), GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window.getPointer(), true);
	if (glfwGetKey(window.getPointer(), GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window.getPointer(), GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window.getPointer(), GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window.getPointer(), GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
	if (glfwGetKey(window.getPointer(), GLFW_KEY_Q) == GLFW_PRESS &&
		glfwGetKey(window.getPointer(), GLFW_MOD_SHIFT) == GLFW_PRESS)
		camera.ProcessKeyboard(DOWN, deltaTime);
	if (glfwGetKey(window.getPointer(), GLFW_KEY_E) == GLFW_PRESS)
		camera.ProcessKeyboard(UP, deltaTime);
	if (glfwGetKey(window.getPointer(), GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		camera.isShiftPan = true;
	if (glfwGetKey(window.getPointer(), GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
		camera.isShiftPan = false;

	ASM::ASMInputFlags inputFlags = 0;
	if (glfwGetKey(window.getPointer(), GLFW_KEY_G) == GLFW_PRESS) {
		inputFlags = inputFlags | ASM::gKey;
		//setGrabState();
	} if (glfwGetKey(window.getPointer(), GLFW_KEY_Q) == GLFW_PRESS) {
		inputFlags = inputFlags | ASM::qKey;
		//endAction();
	} if (glfwGetKey(window.getPointer(), GLFW_KEY_X) == GLFW_PRESS)
		inputFlags = inputFlags | ASM::xKey;
	if (glfwGetKey(window.getPointer(), GLFW_KEY_Y) == GLFW_PRESS)
		inputFlags = inputFlags | ASM::yKey;
	if (glfwGetKey(window.getPointer(), GLFW_KEY_Z) == GLFW_PRESS)
		inputFlags = inputFlags | ASM::zKey;
	if (glfwGetKey(window.getPointer(), GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		inputFlags = inputFlags | ASM::shiftKey;
	if (mouse.getisLMBPressed())
		inputFlags = inputFlags | ASM::m1Key;
	if (glfwGetKey(window.getPointer(), GLFW_KEY_S) == GLFW_PRESS)
		inputFlags = inputFlags | ASM::sKey;
	actionStateMachine.setFlags(inputFlags);


	// TEMP
	if (glfwGetKey(window.getPointer(), GLFW_KEY_O) == GLFW_PRESS) {
		camera.canOrbit = !camera.canOrbit;
	}
}
void Application::renderModel() {
	mats.projection = glm::perspective(glm::radians(camera.Zoom), (float)window.getViewerSize().x / (float)window.getViewerSize().y, 0.1f, 100.0f);
	mats.view = camera.GetViewMatrix();

	framebuffer.bindFramebuffer();

	glEnable(GL_DEPTH_TEST);
	glClearColor(0.15f, 0.161f, 0.22f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	modelShader.use();
	mats.model = glm::mat4(1.0f);
	modelShader.setMatrices(mats.projection, mats.view, mats.model);
	modelShader.setVec3("cameraPos", camera.Position);
	vertShader.use();
	vertShader.setMatrices(mats.projection, mats.view, mats.model);
	vertShader.setVec3("viewPos", camera.Position);
	selectedVertShader.use();
	selectedVertShader.setMatrices(mats.projection, mats.view, mats.model);
	selectedVertShader.setVec3("viewPos", camera.Position);
	edgeShader.use();
	edgeShader.setMatrices(mats.projection, mats.view, mats.model);
	edgeShader.setVec3("viewPos", camera.Position);

	//gizmoShader.setMatrices(mats);
	gizmoShader.use();
	gizmoShader.setMatrices(mats.projection, mats.view, mats.model);
	gizmoShader.setVec3("viewPos", camera.Position);


	//model.Draw(modelShader, &vertShader, false);
	model.Draw(modelShader, &vertShader, &selectedVertShader, &edgeShader);
	gizmo.draw(&gizmoShader);
	grid.render(mats.projection, mats.view, camera.Position, camera.Radius);

	framebuffer.unbindFramebuffer();

	verticeDataFramebuffer.bindFramebuffer();
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	verticeDataShader.use();
	verticeDataShader.setMatrices(mats.projection, mats.view, mats.model);
	verticeDataShader.setVec3("viewPos", camera.Position);
	model.Draw(verticeDataShader, &verticeDataShader, &verticeDataShader, NULL, true);
	uiElementDataShader.use();
	uiElementDataShader.setMatrices(mats.projection, mats.view, mats.model);
	uiElementDataShader.setVec3("viewPos", camera.Position);
	gizmo.draw(&uiElementDataShader);
	verticeDataFramebuffer.unbindFramebuffer();
}

void Application::initGui() {
	getImGuiIO();
	initImGui();
}
void Application::getImGuiIO() {
	ImGui::CreateContext();

	IMGUI_CHECKVERSION();
	ImGuiIO& io = ImGui::GetIO(); (void)io; // TODO: figure out why this is here

	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

	ImGui::StyleColorsDark();
	ImGuiStyle& style = ImGui::GetStyle();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

	io.Fonts->AddFontFromFileTTF("fonts/FreeMonoBold.ttf", 13);
	io.Fonts->Build();
}
void Application::displayGui() {
	newFrames();
	createDockSpace();
	ImGui::NewFrame();
	showMainMenuBar();
	showDemoWindow();
	log.showConsole();
	showDebugGui();
	processGuiInput();
	runCurrentState();
	showObjectInspector();
	show3DViewer();
	ImGui::Render();
}
void Application::endFrame() {
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		GLFWwindow* backup_current_context = glfwGetCurrentContext();
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		glfwMakeContextCurrent(backup_current_context);
	}
}
void Application::cleanUpGui() {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}
void Application::initImGui() {
	ImGui_ImplGlfw_InitForOpenGL(window.getPointer(), true);
	ImGui_ImplOpenGL3_Init("#version 330");
}
void Application::showDemoWindow() {
	bool show_demo_window = true;
	ImGui::ShowDemoWindow(&show_demo_window);
}

void Application::newFrames() {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
}
void Application::createDockSpace() {
	//bool* p_open;	
	static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;
	static bool opt_fullscreen = true;
	static bool opt_padding = false;
	ImGuiIO& io = ImGui::GetIO(); (void)io;
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
void Application::showMainMenuBar() {
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Save", "CTRL+S")) {}
			if (ImGui::MenuItem("Load", ".obj")) {
				openObjFile();
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Edit"))
		{
			if (ImGui::MenuItem("Undo", "CTRL+Z")) {}
			if (ImGui::MenuItem("Redo", "CTRL+Y", false, false)) {}  // Disabled item
			ImGui::Separator();
			if (ImGui::MenuItem("Cut", "CTRL+X")) {}
			if (ImGui::MenuItem("Copy", "CTRL+C")) {}
			if (ImGui::MenuItem("Paste", "CTRL+V")) {}
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}
}
void Application::showDebugGui() {
	ImGui::Begin("Debug");
	if (ImGui::TreeNode("Memory")) {
		if (Utilities::startDebugTable("Usage", "Name", "Value")) {
			Utilities::nameVariableDebugTable(getMemoryUsage(), "Memory Usage (bytes)");
			ImGui::EndTable();
		}
		ImGui::TreePop();
	}

	mouse.degbug();
	actionStateMachine.debug();
	//Log::debug();
	model.debug();
	ImGui::Checkbox("Show UI Framebuffer", &displayUIFramebuffer);
	ImGui::End();
}
void Application::processGuiInput() {
	// TODO: rework this to be more adaptive with state machine
	// TODO: ACTUALLY DO THAT ^
	/*
	if (mouse.getisLMBPressed() && firstClick) {
		firstClick = false;
		Utilities::PixelData pixelData = verticeDataFramebuffer.getDataAtPixel(
			mouse.getxPos(),
			mouse.getyPos(),
			(float)window.getViewerSize().x,
			(float)window.getViewerSize().y,
			(float)window.getBufferSize().x,
			(float)window.getBufferSize().y);
		log.logConsole("R: " + std::to_string(pixelData.r) +
			" G: " + std::to_string(pixelData.g) +
			" B: " + std::to_string(pixelData.b));
		if (pixelData.g == 1.0f) { // UI Elements
			UIElement* uiElement = outliner.getUIElement((int)(pixelData.r - 1));
			Utilities::UIElementType type = uiElement->getType();
			switch (type) {
			case Utilities::GizmoXAxisArrow:
			case Utilities::GizmoYAxisArrow:
			case Utilities::GizmoZAxisArrow:
			case Utilities::GizmoCenter:
				uiElement->setIsHighlighted(true);
				break;
			default:
				break;
			}
		}
		else { // vertices or other stuff
			model.setSelectedVertices(pixelData);
		}
	}
	else {
		if (!mouse.getisLMBPressed())
			firstClick = true;
		for (int i = 0; i < outliner.getNumUIElements(); i++) {
			outliner.getUIElement(i)->setIsHighlighted(false);
		}
	}
	*/
}
void Application::runCurrentState() {
	//std::cout << "Running current state" << std::endl;
	actionStateMachine.update();
}
void Application::showObjectInspector() {
	ImGui::Begin("Object Inspector");

	static ImGuiTableFlags flags =
		ImGuiTableFlags_Borders |
		ImGuiTableFlags_RowBg |
		ImGuiTableFlags_SizingFixedFit;

	if (ImGui::CollapsingHeader("Old Stuff")) {
		if (ImGui::TreeNode("How to do this "))
		{
			if (ImGui::CollapsingHeader("Category B")) { ImGui::Text("Blah blah blah"); };
			if (ImGui::CollapsingHeader("Category C")) { ImGui::Text("Blah blah blah"); };

			ImGui::TreePop();
		}
		//const float TEXT_BASE_WIDTH = ImGui::CalcTextSize("A").x;
		//const float TEXT_BASE_HEIGHT = ImGui::GetTextLineHeightWithSpacing();


		//ImGui::DragFloat("XPos", &VertexX, 0.005f);
		//ImGui::DragFloat("YPos", &VertexY, 0.005f);
		//ImGui::DragFloat("ZPos", &VertexZ, 0.005f);
		//ImGui::DragInt("index", &vertexIndex, 1);
		//model->origVertices[vertexIndex].setPos(glm::vec3(VertexX, VertexY, VertexZ));

		/*if (ImGui::BeginTable("Vertices", 4, flags))
		{

			ImGui::TableSetupColumn(" ");
			ImGui::TableSetupColumn("X");
			ImGui::TableSetupColumn("Y");
			ImGui::TableSetupColumn("Z");
			ImGui::TableHeadersRow();

			for (int row = 0; row < model->origVertices.size(); row++)
			{
				ImGui::TableNextRow();
				for (int column = 0; column < 4; column++)
				{
					ImGui::TableSetColumnIndex(column);
					char buf[32];
					if (column == 0)
						sprintf_s(buf, "%i ", row);
					else if (column == 1)
						if (model->origVertices[row].Position.x < 0)
							sprintf_s(buf, "%f", model->origVertices[row].Position.x);
						else
							sprintf_s(buf, " %f", model->origVertices[row].Position.x);
					else if (column == 2)
						if (model->origVertices[row].Position.y < 0)
							sprintf_s(buf, "%f", model->origVertices[row].Position.y);
						else
							sprintf_s(buf, " %f", model->origVertices[row].Position.y);
					else if (column == 3)
						if (model->origVertices[row].Position.z < 0)
							sprintf_s(buf, "%f", model->origVertices[row].Position.z);
						else
							sprintf_s(buf, " %f", model->origVertices[row].Position.z);

					ImGui::TextUnformatted(buf);
				}
			}
			ImGui::EndTable();
		}
		*/

		ImGui::Text("Model 2");
		ImGui::Text("Vertices");
		if (ImGui::BeginTable("Model 2 Vertices", 5, flags))
		{

			ImGui::TableSetupColumn(" ");
			ImGui::TableSetupColumn("X");
			ImGui::TableSetupColumn("Y");
			ImGui::TableSetupColumn("Z");
			ImGui::TableSetupColumn("Connected Indices");
			ImGui::TableHeadersRow();

			for (int row = 0; row < model.getNumVertices(); row++)
			{
				ImGui::TableNextRow();
				for (int column = 0; column < 5; column++)
				{
					ImGui::TableSetColumnIndex(column);
					Utilities::Vertex currVertex = model.getVertex(row);
					char buf[64];
					if (column == 0)
						sprintf_s(buf, "%i ", row);
					else if (column == 1)
						if (currVertex.Position.x < 0)
							sprintf_s(buf, "%f", currVertex.Position.x);
						else
							sprintf_s(buf, " %f", currVertex.Position.x);
					else if (column == 2)
						if (currVertex.Position.y < 0)
							sprintf_s(buf, "%f", currVertex.Position.y);
						else
							sprintf_s(buf, " %f", currVertex.Position.y);
					else if (column == 3)
						if (currVertex.Position.z < 0)
							sprintf_s(buf, "%f", currVertex.Position.z);
						else
							sprintf_s(buf, " %f", currVertex.Position.z);
					else if (column == 4) {
						std::string indices = "";
						for (int i = 0; i < currVertex.connectedVertsIndices.size(); i++) {
							indices += std::to_string(currVertex.connectedVertsIndices[i]) + ", ";
						}
						sprintf_s(buf, "%s", indices.c_str());
					}
					ImGui::TextUnformatted(buf);
				}
			}
			ImGui::EndTable();
		}

		ImGui::Text("Faces");
		if (ImGui::BeginTable("Model 2 Faces", 2, flags))
		{

			ImGui::TableSetupColumn(" ");
			ImGui::TableSetupColumn("Vertex Indices");
			ImGui::TableHeadersRow();

			for (int row = 0; row < model.getNumFaces(); row++)
			{
				ImGui::TableNextRow();
				for (int column = 0; column < 2; column++)
				{
					ImGui::TableSetColumnIndex(column);
					face currFace = model.getFace(row);
					char buf[64];
					if (column == 0)
						sprintf_s(buf, "%i ", row);
					else if (column == 1) {
						std::string indices = "";
						for (int i = 0; i < currFace.vertIndices.size(); i++) {
							indices += std::to_string(currFace.vertIndices[i]) + ", ";
						}
						sprintf_s(buf, "%s", indices.c_str());
					}
					ImGui::TextUnformatted(buf);
				}
			}
			ImGui::EndTable();
		}

		ImGui::Text("Triangulated Indices");
		if (ImGui::BeginTable("Model 2 Indices", 2, flags))
		{

			ImGui::TableSetupColumn(" ");
			ImGui::TableSetupColumn("Triangle Indices");
			ImGui::TableHeadersRow();

			for (int row = 0; row < model.getNumIndices() / 3; row++)
			{
				ImGui::TableNextRow();
				for (int column = 0; column < 2; column++)
				{
					ImGui::TableSetColumnIndex(column);
					char buf[64];
					if (column == 0)
						sprintf_s(buf, "%i ", row);
					else if (column == 1) {
						std::string indices = "";
						for (int i = 0; i < 3; i++) {
							indices += std::to_string(model.getIndice(row * 3 + i)) + ", ";
						}
						sprintf_s(buf, "%s", indices.c_str());
					}
					ImGui::TextUnformatted(buf);
				}
			}
			ImGui::EndTable();
		}

		ImGui::Text("Edge Indices");
		if (ImGui::BeginTable("Model 2 Edge Indices", 2, flags))
		{

			ImGui::TableSetupColumn(" ");
			ImGui::TableSetupColumn("Edge Indices");
			ImGui::TableHeadersRow();

			for (int row = 0; row < model.getNumEdgeIndices() / 2; row++)
			{
				ImGui::TableNextRow();
				for (int column = 0; column < 2; column++)
				{
					ImGui::TableSetColumnIndex(column);
					char buf[64];
					if (column == 0)
						sprintf_s(buf, "%i ", row);
					else if (column == 1) {
						std::string indices = "";
						for (int i = 0; i < 2; i++) {
							indices += std::to_string(model.getEdgeIndice(row * 2 + i)) + ", ";
						}
						sprintf_s(buf, "%s", indices.c_str());
					}
					ImGui::TextUnformatted(buf);
				}
			}
			ImGui::EndTable();
		}

	};

	if (ImGui::CollapsingHeader("Objects Outliner")) {
		if (ImGui::TreeNode("Models")) {
			for (int i = 0; i < outliner.getNumModels(); i++) {
				if (ImGui::TreeNode(outliner.getModel(i)->getName().c_str())) {
					showModelTables(outliner.getModel(i));
					ImGui::TreePop();
				}
			}

			ImGui::TreePop();
		}

		if (ImGui::TreeNode("UI Elements Outliner")) {
			for (int i = 0; i < outliner.getNumUIElements(); i++) {
				if (ImGui::TreeNode(Utilities::getUIElementNameFromType(outliner.getUIElement(i)->getType()).c_str())) {
					showUIElementTables(outliner.getUIElement(i));
					ImGui::TreePop();
				}
			}
			ImGui::TreePop();
		}
	}
	ImGui::End();

}
void Application::show3DViewer() {
	ImGui::Begin("3D Viewer");
	mouse.setis3DViewerFocused(ImGui::IsWindowFocused);
	setCursorPos();
	updateImGuiTexture();
	ImGui::End();
}

void Application::setGrabState() {
	// TODO: have the state check for this, not like this
	actionStateMachine.setState(std::make_shared<ASM::GrabState>());
}
void Application::endAction()  {
	actionStateMachine.setState(std::make_shared<ASM::IdleState>());
}
void Application::grab() {
	if (Utilities::getCurrentTimeInMS() - mouse.getMovingTime() < 5) {
		model.translateSelectedVertices(mouse.getOffset().y * 0.005f, camera.Up);
		model.translateSelectedVertices(mouse.getOffset().x * 0.005f, camera.Right);
	}
}

void Application::setCursorPos() {
	ImVec2 pos = ImGui::GetCursorScreenPos();
	//window.viewerXPos = pos.x;
	//window.viewerYPos = pos.y;
	window.setViewerPos(glm::vec2(pos.x, pos.y));
	const float windowWidth = ImGui::GetContentRegionAvail().x;
	const float windowHeight = ImGui::GetContentRegionAvail().y;
	//window.viewerWidth = windowWidth;
	//window->viewerHeight = windowHeight;
	window.setViewerSize(glm::vec2(windowWidth, windowHeight));
	if (mouse.getkeepCursorInFrame()) {
		double xPos, yPos;
		glfwGetCursorPos(window.getPointer(), &xPos, &yPos);
		mouse.setxPos((int)xPos);
		mouse.setyPos((int)yPos);
		if (xPos > window.getViewerSize().x)
			xPos = xPos - window.getViewerSize().x;
		if (xPos < 0)
			xPos += window.getViewerSize().x;
		if (yPos > window.getViewerSize().y)
			yPos -= window.getViewerSize().x;
		if (yPos < 0)
			yPos += window.getViewerSize().x;

		glfwSetCursorPos(window.getPointer(), xPos, yPos);
	}
}
void Application::updateImGuiTexture() {
	framebuffer.rescaleFramebuffer(
		(float)(window.getViewerSize().x + (window.getBufferSize().x - window.getViewerSize().x)),
		(float)(window.getViewerSize().y + (window.getBufferSize().y - window.getViewerSize().y)));
	verticeDataFramebuffer.rescaleFramebuffer(
		(float)(window.getViewerSize().x + (window.getBufferSize().x - window.getViewerSize().x)),
		(float)(window.getViewerSize().y + (window.getBufferSize().y - window.getViewerSize().y)));

	ImGui::GetWindowDrawList()->AddImage(
		(void*)framebuffer.getTextureID(),
		ImVec2((float)window.getViewerPos().x, (float)window.getViewerPos().y),
		ImVec2((float)(window.getViewerPos().x + window.getViewerSize().x), (float)(window.getViewerPos().y + window.getViewerSize().y)),
		ImVec2(0, 1),
		ImVec2(1, 0)
	);
	if (displayUIFramebuffer) {
		ImGui::GetWindowDrawList()->AddImage(
			(void*)verticeDataFramebuffer.getTextureID(),
			ImVec2((float)window.getViewerPos().x, (float)window.getViewerPos().y),
			ImVec2((float)(window.getViewerPos().x + window.getViewerSize().x), (float)(window.getViewerPos().y + window.getViewerSize().y)),
			ImVec2(0, 1),
			ImVec2(1, 0)
		);
	}
}
void Application::openObjFile() {
	// common dialog box structure, setting all fields to 0 is important
	OPENFILENAME ofn = { 0 };
	TCHAR szFile[260] = { 0 };
	// Initialize remaining fields of OPENFILENAME structure
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hwnd;
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = L".obj\0*.obj\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	if (GetOpenFileName(&ofn) == TRUE)
	{
		// use ofn.lpstrFile here
		std::cout << (CW2A)ofn.lpstrFile << std::endl;
		std::string path = (std::string)(CW2A)ofn.lpstrFile;
		for (unsigned int i = 0; i < path.length(); i++) {
			if (path[i] == '\\') {
				path[i] = '/';
			}
		}
		//model->replaceModel(path);
		//model->replaceModel(path);
	}
}
void Application::showModelTables(Model* model) {
	static ImGuiTableFlags flags =
		ImGuiTableFlags_Borders |
		ImGuiTableFlags_RowBg |
		ImGuiTableFlags_SizingFixedFit |
		ImGuiTableFlags_ScrollY |
		ImGuiTableFlags_ScrollX;
	ImVec2 outer_size = ImVec2(0.0f, ImGui::GetTextLineHeightWithSpacing() * 8);
	if (ImGui::TreeNode("Vertices")) {
		if (ImGui::BeginTable("Vertices", 5, flags, outer_size))
		{
			ImGui::TableSetupScrollFreeze(1, 1);
			ImGui::TableSetupColumn(" ");
			ImGui::TableSetupColumn("X");
			ImGui::TableSetupColumn("Y");
			ImGui::TableSetupColumn("Z");
			ImGui::TableSetupColumn("Connected Indices");
			ImGui::TableHeadersRow();

			for (int row = 0; row < model->getNumVertices(); row++)
			{
				ImGui::TableNextRow();
				for (int column = 0; column < 5; column++)
				{
					ImGui::TableSetColumnIndex(column);
					Utilities::Vertex currVertex = model->getVertex(row);
					char buf[64];
					if (column == 0)
						sprintf_s(buf, "%i ", row);
					else if (column == 1)
						if (currVertex.Position.x < 0)
							sprintf_s(buf, "%f", currVertex.Position.x);
						else
							sprintf_s(buf, " %f", currVertex.Position.x);
					else if (column == 2)
						if (currVertex.Position.y < 0)
							sprintf_s(buf, "%f", currVertex.Position.y);
						else
							sprintf_s(buf, " %f", currVertex.Position.y);
					else if (column == 3)
						if (currVertex.Position.z < 0)
							sprintf_s(buf, "%f", currVertex.Position.z);
						else
							sprintf_s(buf, " %f", currVertex.Position.z);
					else if (column == 4) {
						std::string indices = "";
						for (int i = 0; i < currVertex.connectedVertsIndices.size(); i++) {
							indices += std::to_string(currVertex.connectedVertsIndices[i]) + ", ";
						}
						sprintf_s(buf, "%s", indices.c_str());
					}
					ImGui::TextUnformatted(buf);
				}
			}
			ImGui::EndTable();
		}
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Faces")) {
		if (ImGui::BeginTable("Faces", 2, flags, outer_size))
		{
			ImGui::TableSetupScrollFreeze(1, 1);
			ImGui::TableSetupColumn(" ");
			ImGui::TableSetupColumn("Vertex Indices");
			ImGui::TableHeadersRow();

			for (int row = 0; row < model->getNumFaces(); row++)
			{
				ImGui::TableNextRow();
				for (int column = 0; column < 2; column++)
				{
					ImGui::TableSetColumnIndex(column);
					face currFace = model->getFace(row);
					char buf[64];
					if (column == 0)
						sprintf_s(buf, "%i ", row);
					else if (column == 1) {
						std::string indices = "";
						for (int i = 0; i < currFace.vertIndices.size(); i++) {
							indices += std::to_string(currFace.vertIndices[i]) + ", ";
						}
						sprintf_s(buf, "%s", indices.c_str());
					}
					ImGui::TextUnformatted(buf);
				}
			}
			ImGui::EndTable();
		}
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Triangle Indices")) {
		if (ImGui::BeginTable("Indices", 2, flags, outer_size))
		{
			ImGui::TableSetupScrollFreeze(1, 1);
			ImGui::TableSetupColumn(" ");
			ImGui::TableSetupColumn("Triangle Indices");
			ImGui::TableHeadersRow();

			for (int row = 0; row < model->getNumIndices() / 3; row++)
			{
				ImGui::TableNextRow();
				for (int column = 0; column < 2; column++)
				{
					ImGui::TableSetColumnIndex(column);
					char buf[64];
					if (column == 0)
						sprintf_s(buf, "%i ", row);
					else if (column == 1) {
						std::string indices = "";
						for (int i = 0; i < 3; i++) {
							indices += std::to_string(model->getIndice(row * 3 + i)) + ", ";
						}
						sprintf_s(buf, "%s", indices.c_str());
					}
					ImGui::TextUnformatted(buf);
				}
			}
			ImGui::EndTable();
		}
		ImGui::TreePop();
	}
}
void Application::showUIElementTables(UIElement* uiElement) {
	uiElement->showTables();
}

Utilities::PixelData Application::getDataAtMousePos() {
	/*return verticeDataFramebuffer.getDataAtPixel(
		mouse.getxPos(),
		mouse.getyPos(),
		(float)window.viewerWidth,
		(float)window.viewerHeight,
		(float)window.bufferWidth,
		(float)window.bufferHeight
	);*/

	return verticeDataFramebuffer.getDataAtPixel(
		mouse.getxPos(),
		mouse.getyPos(),
		(float)window.getViewerSize().x,
		(float)window.getViewerSize().y,
		(float)window.getBufferSize().x,
		(float)window.getBufferSize().y
	);
}

void Application::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
	if (mouse.getis3DViewerFocused() && mouse.getIn3DWindow()) {
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
void Application::framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	this->window.setBufferSize(glm::ivec2(width, height));
	//this->window.bufferWidth = width;
	//this->window.bufferHeight = height;
}
void Application::mouseCallback(GLFWwindow* window, double xpos, double ypos)
{
	mouse.setxPos((int)xpos);
	mouse.setyPos((int)ypos);
	if (mouse.getfirstMouse()) {
		mouse.setLastX((int)xpos);
		mouse.setLastY((int)ypos);
		mouse.setfirstMouse(false);
	}
	float xoffset = xpos - mouse.getLastX();
	float yoffset = mouse.getLastY() - ypos; // reversed since y-coordinates range from bottom to top.
	mouse.setLastX(xpos);
	mouse.setLastY(ypos);
	if (xoffset > this->window.getViewerSize().x / 2.0)
		return;
	if (yoffset > this->window.getViewerSize().y / 2.0)
		return;
	if (xoffset < (-1 * (int)this->window.getViewerSize().x) / 2.0)
		return;
	if (yoffset < (-1 * (int)this->window.getViewerSize().y) / 2.0)
		return;
	camera.ProcessMouseMovement(xoffset, yoffset, true);
	mouse.setOffset(xoffset, yoffset);
	mouse.setMovingTime();
}
void Application::scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
	if (mouse.getis3DViewerFocused() && mouse.getIn3DWindow())
		camera.ProcessMouseScroll((float)yoffset);
}

static void frameBufferSizeCallbackFWD(GLFWwindow* window, int width, int height) {
	callBackPointer->framebufferSizeCallback(window, width, height);
}
static void mouseCallbackFWD(GLFWwindow* window, double xpos, double ypos) {
	callBackPointer->mouseCallback(window, xpos, ypos);
}
static void scrollCallbackFWD(GLFWwindow* window, double xpos, double ypos) {
	callBackPointer->scrollCallback(window, xpos, ypos);
}
static void mouseButtonCallbackFWD(GLFWwindow* window, int button, int action, int mods) {
	callBackPointer->mouseButtonCallback(window, button, action, mods);
}