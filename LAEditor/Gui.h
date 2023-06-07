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
            model->test();
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