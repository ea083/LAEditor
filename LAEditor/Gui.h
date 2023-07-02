#pragma once

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <windows.h>
#include <commdlg.h>
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#include <stdlib.h>
#include <atlstr.h>

#include "Model.h"
#include "Model2.h"
#include "Window.h"
#include "Mouse.h"
#include "Framebuffer.h"
#include "Camera.h"
#include "Application.h"
#include "VerticeDataFramebuffer.h"
#include "Outliner.h"

class Gui {
public:

	float VertexX = 0.0f, VertexY = 0.0f, VertexZ = 0.0f;
	int vertexIndex = 0;

	Gui() {	}

	void initGui(Window* window, Model* model, Mouse* mouse, Framebuffer* framebuffer, Camera* camera, Utilities::MVP* mats, Model2::Model2* model2, VerticeDataFramebuffer* verticeDataFramebuffer, Outliner* outliner) {
		this->window = window;
		this->model = model;
		this->mouse = mouse;
		this->framebuffer = framebuffer;
		this->camera = camera;
		this->mats = mats;
		this->model2 = model2;
		this->verticeDataFramebuffer = verticeDataFramebuffer;
		this->outliner = outliner;
		getImGuiIO();
		initImGui();
	}

	void displayGui() {
		newFrames();
		createDockSpace();
		ImGui::NewFrame();
		showMainMenuBar();
		showDemoWindow();
		showDebugGui();
		showObjectInspector();
		show3DViewer();
		ImGui::Render();
	}

	void endFrame() {
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

	void cleanUp() {
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}
private:

	Outliner* outliner;

	Window* window;
	Model* model;
	Model2::Model2* model2;
	Mouse* mouse;
	Framebuffer* framebuffer;
	VerticeDataFramebuffer* verticeDataFramebuffer;
	Camera* camera;
	Utilities::MVP* mats;
	OPENFILENAME ofn;
	LPWSTR szFile;
	HWND hwnd;
	HANDLE hf;

	float debug1 = 0.0f;

	void openObjFile() {
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
				if(path[i] == '\\') {
					path[i] = '/';
				}
			}
			//model->replaceModel(path);
			model->replaceModel(path);
		}
	}
	void getImGuiIO() {
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
	}
	void initImGui() {
		ImGui_ImplGlfw_InitForOpenGL(window->windowPointer, true);
		ImGui_ImplOpenGL3_Init("#version 330");
	}

	void showDemoWindow() {
		bool show_demo_window = true;
		ImGui::ShowDemoWindow(&show_demo_window);
	}

	void newFrames() {
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
	}
	void createDockSpace() {
		bool* p_open;	
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
	void showMainMenuBar() {
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
	void showDebugGui() {
		ImGui::Begin("Debug");
		ImGui::DragInt("SelectedIndex", &(model->selectedIndex), 1, 0);
		ImGui::End();
	}
	void showObjectInspector() {

		if (mouse->getisLMBPressed()) {
			model2->setSelectedVertices(
				verticeDataFramebuffer->getDataAtPixel(
					mouse->getxPos(), 
					mouse->getyPos(), 
					window->viewerWidth, 
					window->viewerHeight, 
					window->bufferWidth, 
					window->bufferHeight));

			//glm::vec3 ray = mouse->Raycast(window, *mats);
			//model->getInterceptedVertices(ray, camera->Position);
			//model2->getInterceptedVertices(ray, camera->Position);
		}

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


			ImGui::DragFloat("XPos", &VertexX, 0.005f);
			ImGui::DragFloat("YPos", &VertexY, 0.005f);
			ImGui::DragFloat("ZPos", &VertexZ, 0.005f);
			ImGui::DragInt("index", &vertexIndex, 1);

			model->origVertices[vertexIndex].setPos(glm::vec3(VertexX, VertexY, VertexZ));

			if (ImGui::BeginTable("Vertices", 4, flags))
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
						if(column == 0)
							sprintf_s(buf, "%i ", row);
						else if(column == 1)
							if(model->origVertices[row].Position.x < 0)
								sprintf_s(buf, "%f", model->origVertices[row].Position.x);
							else
								sprintf_s(buf, " %f", model->origVertices[row].Position.x);
						else if(column == 2)
							if (model->origVertices[row].Position.y < 0)
								sprintf_s(buf, "%f", model->origVertices[row].Position.y);
							else
								sprintf_s(buf, " %f", model->origVertices[row].Position.y);
						else if(column == 3)
							if (model->origVertices[row].Position.z < 0)
								sprintf_s(buf, "%f", model->origVertices[row].Position.z);
							else
								sprintf_s(buf, " %f", model->origVertices[row].Position.z);

						ImGui::TextUnformatted(buf);
					}
				}
				ImGui::EndTable();
			}

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

				for (int row = 0; row < model2->getNumVertices(); row++)
				{
					ImGui::TableNextRow();
					for (int column = 0; column < 5; column++)
					{
						ImGui::TableSetColumnIndex(column);
						Utilities::Vertex currVertex = model2->getVertex(row);
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

				for (int row = 0; row < model2->getNumFaces(); row++)
				{
					ImGui::TableNextRow();
					for (int column = 0; column < 2; column++)
					{
						ImGui::TableSetColumnIndex(column);
						Model2::face currFace = model2->getFace(row);
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

				for (int row = 0; row < model2->getNumIndices()/3; row++)
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
								indices += std::to_string(model2->getIndice(row*3 + i)) + ", ";
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

				for (int row = 0; row < model2->getNumEdgeIndices() / 2; row++)
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
								indices += std::to_string(model2->getEdgeIndice(row * 2 + i)) + ", ";
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
				for (int i = 0; i < outliner->getNumModels(); i++) {
					if (ImGui::TreeNode(outliner->getModel(i)->getName().c_str())) {
						showModelTables(outliner->getModel(i));
						ImGui::TreePop();
					}
				}

				ImGui::TreePop();
			}

			if (ImGui::TreeNode("UI Elements Outliner")) {
				for (int i = 0; i < outliner->getNumUIElements(); i++) {
					if (ImGui::TreeNode(outliner->getUIElement(i)->getName().c_str())) {
						showUIElementTables(outliner->getUIElement(i));
						ImGui::TreePop();
					}
				}
				ImGui::TreePop();
			}
		}
		ImGui::End();

	}
	void show3DViewer() {
		ImGui::Begin("3D Viewer");
		mouse->setis3DViewerFocused(ImGui::IsWindowFocused);
		setCursorPos();
		updateImGuiTexture();
		ImGui::End();
	}

	void setCursorPos() {
		ImVec2 pos = ImGui::GetCursorScreenPos();
		window->viewerXPos = pos.x;
		window->viewerYPos = pos.y;
		const float windowWidth = ImGui::GetContentRegionAvail().x;
		const float windowHeight = ImGui::GetContentRegionAvail().y;
		window->viewerWidth = windowWidth;
		window->viewerHeight = windowHeight;
		if (mouse->getkeepCursorInFrame()) {
			double xPos, yPos;
			glfwGetCursorPos(window->windowPointer, &xPos, &yPos);
			mouse->setxPos(xPos);
			mouse->setyPos(yPos);
			if (xPos > window->bufferWidth)
				xPos = xPos - window->bufferWidth;
			if (xPos < 0)
				xPos += window->bufferWidth;
			if (yPos > window->bufferHeight)
				yPos -= window->bufferHeight;
			if (yPos < 0)
				yPos += window->bufferHeight;
			glfwSetCursorPos(window->windowPointer, xPos, yPos);
		}
	}

	void updateImGuiTexture() {
		framebuffer->rescaleFramebuffer(
			window->viewerWidth + (window->bufferWidth - window->viewerWidth),
			window->viewerHeight + (window->bufferHeight - window->viewerHeight));
		verticeDataFramebuffer->rescaleFramebuffer(
			window->viewerWidth + (window->bufferWidth - window->viewerWidth),
			window->viewerHeight + (window->bufferHeight - window->viewerHeight));

		ImGui::GetWindowDrawList()->AddImage(
			(void*)framebuffer->getTextureID(),
			ImVec2(window->viewerXPos, window->viewerYPos),
			ImVec2(window->viewerXPos + window->viewerWidth, window->viewerYPos + window->viewerHeight),
			ImVec2(0, 1),
			ImVec2(1, 0)
		);

		/*ImGui::GetWindowDrawList()->AddImage(
			(void*)verticeDataFramebuffer->getTextureID(),
			ImVec2(window->viewerXPos, window->viewerYPos),
			ImVec2(window->viewerXPos + window->viewerWidth, window->viewerYPos + window->viewerHeight),
			ImVec2(0, 1),
			ImVec2(1, 0)
		);*/
	}

	void showModelTables(Model2::Model2* model) {
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

				for (int row = 0; row < model2->getNumVertices(); row++)
				{
					ImGui::TableNextRow();
					for (int column = 0; column < 5; column++)
					{
						ImGui::TableSetColumnIndex(column);
						Utilities::Vertex currVertex = model2->getVertex(row);
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
		if(ImGui::TreeNode("Faces")) {
			if (ImGui::BeginTable("Faces", 2, flags, outer_size))
			{
				ImGui::TableSetupScrollFreeze(1, 1);
				ImGui::TableSetupColumn(" ");
				ImGui::TableSetupColumn("Vertex Indices");
				ImGui::TableHeadersRow();

				for (int row = 0; row < model2->getNumFaces(); row++)
				{
					ImGui::TableNextRow();
					for (int column = 0; column < 2; column++)
					{
						ImGui::TableSetColumnIndex(column);
						Model2::face currFace = model2->getFace(row);
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

				for (int row = 0; row < model2->getNumIndices() / 3; row++)
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
								indices += std::to_string(model2->getIndice(row * 3 + i)) + ", ";
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

	void showUIElementTables(UIElement* uiElement) {
		/*
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

				for (int row = 0; row < uiElement->getNumVertices(); row++)
				{
					ImGui::TableNextRow();
					for (int column = 0; column < 5; column++)
					{
						ImGui::TableSetColumnIndex(column);
						Utilities::Vertex currVertex = uiElement->getVertex(row);
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
		*/
		uiElement->showTables();
	}
};