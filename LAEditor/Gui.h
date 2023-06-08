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

class Gui {
public:

	float VertexX = 0.0f, VertexY = 0.0f, VertexZ = 0.0f;
	int vertexIndex = 0;

	Gui(GLFWwindow* window, Model* model) {
		this->window = window;
		this->model = model;
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
	void showObjectData() {
		ImGui::DragFloat("XPos", &VertexX, 0.005f);
		ImGui::DragFloat("YPos", &VertexY, 0.005f);
		ImGui::DragFloat("ZPos", &VertexZ, 0.005f);
		ImGui::DragInt("index", &vertexIndex, 1);

		//model->meshes[0].vertices[vertexIndex].Position.x = VertexX;
		//model->meshes[0].vertices[vertexIndex].Position.y = VertexY;
		//model->meshes[0].vertices[vertexIndex].Position.z = VertexZ;
		model->origVertices[vertexIndex].setPos(glm::vec3(VertexX, VertexY, VertexZ));

		static ImGuiTableFlags flags = 
			ImGuiTableFlags_Borders | 
			ImGuiTableFlags_RowBg | 
			ImGuiTableFlags_SizingFixedFit;
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
	}
private:
	GLFWwindow* window;
	Model* model;
	OPENFILENAME ofn;
	LPWSTR szFile;
	HWND hwnd;
	HANDLE hf;

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
			model->replaceModel(path);
		}
	}
};