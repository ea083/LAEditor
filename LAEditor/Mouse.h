#pragma once

#include "Utilities.h"
#include <iostream>

class Mouse {
public:
	Mouse() {}
	Mouse(float lastX, float lastY) {
		init(lastX, lastY);
	}
	~Mouse() {}
	void init(float lastX, float lastY) {
		this->lastX = lastX;
		this->lastY = lastY;
	}

	void degbug() {
		static ImGuiTableFlags flags =
			ImGuiTableFlags_Borders |
			ImGuiTableFlags_RowBg |
			ImGuiTableFlags_SizingFixedFit |
			ImGuiTableFlags_ScrollY |
			ImGuiTableFlags_ScrollX;
		ImVec2 outer_size = ImVec2(0.0f, ImGui::GetTextLineHeightWithSpacing() * 10);

		if (ImGui::TreeNode("Mouse")) {
			if (ImGui::BeginTable("Variables", 2, flags, outer_size)) {
				ImGui::TableSetupScrollFreeze(1, 1);
				ImGui::TableSetupColumn("Name");
				ImGui::TableSetupColumn("Value");
				ImGui::TableHeadersRow();

				Utilities::nameVariableDebugTable(lastX, "lastX");
				Utilities::nameVariableDebugTable(lastY, "lastY");
				Utilities::nameVariableDebugTable(keepCursorInFrame, "keepCursorInFrame");
				Utilities::nameVariableDebugTable(firstMouse, "firstMouse");
				Utilities::nameVariableDebugTable(is3DViewerFocused, "is3DViewerFocused");
				Utilities::nameVariableDebugTable(in3DWindow, "in3DWindow");
				Utilities::nameVariableDebugTable(isLMBPressed, "isLMBPressed");
				Utilities::nameVariableDebugTable(isLMBPressedLastFrame, "isLMBPressedLastFrame");
				Utilities::nameVariableDebugTable(xPos, "xPos");
				Utilities::nameVariableDebugTable(yPos, "yPos");
				Utilities::nameVariableDebugTable(xoffset, "xoffset");
				Utilities::nameVariableDebugTable(yoffset, "yoffset");
				Utilities::nameVariableDebugTable(movingTime, "movingTime");
				ImGui::EndTable();
			}
			ImGui::TreePop();
		}
	}

	void setOffset(float xoffset, float yoffset) {
		this->xoffset = xoffset;
		this->yoffset = yoffset;
	}

	glm::vec2 getOffset() {
		return glm::vec2(xoffset, yoffset);
	}

	void setLastX(float lastX) {
		this->lastX = lastX;
	}
	void setLastY(float lastY) {
		this->lastY = lastY;
	}
	float getLastX() { return lastX; }
	float getLastY() { return lastY; }

	void setKeepCursorInFrame(bool keepCursorInFrame) { this->keepCursorInFrame = keepCursorInFrame; }
	void setfirstMouse(bool firstMouse) { this->firstMouse = firstMouse; }
	void setis3DViewerFocused(bool is3DViewerFocused) { this->is3DViewerFocused = is3DViewerFocused; }

	bool getkeepCursorInFrame() { return keepCursorInFrame; }
	bool getfirstMouse() { return firstMouse; }
	bool getis3DViewerFocused() { return is3DViewerFocused; }

	void setxPos(int xPos) { this->xPos = xPos;}
	void setyPos(int yPos) { this->yPos = yPos;}

	void setisLMBPressed(bool isLMBPressed) { 
		isLMBPressedLastFrame = isLMBPressed;
		this->isLMBPressed = isLMBPressed;
	}
	bool getisLMBPressed() { return isLMBPressed; }
	bool getIsLMBPressedLastFrame() { return isLMBPressedLastFrame; }

	int getxPos() { return xPos; }
	int getyPos() { return yPos; }
	bool getIn3DWindow() {
		return in3DWindow;
	}
	void setIn3DWindow(bool in3DWindow) {
		this->in3DWindow = in3DWindow;
	}

	void setMovingTime() {
		movingTime = Utilities::getCurrentTimeInMS();
	}

	long getMovingTime() {
		return movingTime;
	}

	glm::vec3 Raycast(Window* window, Utilities::MVP mats) {
		float ratio = window->getViewerSize().y / window->getViewerSize().x;
		// NDC
		glm::vec2 NDCRay = glm::vec3(
			(2.0f * xPos) / window->getViewerSize().x - 1.0f,
			(1.0f - (2.0f * yPos) / window->getViewerSize().y) + 0.17f,
			1.0f
		);
		// Homogeneouse Clip Coordinates
		glm::vec4 HCCRay = glm::vec4(NDCRay.x, NDCRay.y, -1.0f, 1.0f);
		// 4D Eye (Camera) Coordinates
		glm::vec4 CameraRay = glm::inverse(mats.projection) * HCCRay;
		CameraRay = glm::vec4(CameraRay.x, CameraRay.y, -1.0f, 0.0f);
		// 4D World Coordinates
		glm::vec3 WorldCoordinatesRay = glm::inverse(mats.view) * CameraRay;
		WorldCoordinatesRay = glm::normalize(WorldCoordinatesRay);

		return WorldCoordinatesRay;
	}
	
private:
	float lastX = 400.0f;
	float lastY = 300.0f;
	bool keepCursorInFrame = false;
	bool firstMouse = true;
	bool is3DViewerFocused = true;
	bool in3DWindow = true;

	bool isLMBPressed = false;
	bool isLMBPressedLastFrame = isLMBPressed;
	int xPos = 0, yPos = 0;

	float xoffset = 0, yoffset = 0;

	
	long movingTime = Utilities::getCurrentTimeInMS();
};