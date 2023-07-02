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

	void setisLMBPressed(bool isLMBPressed) { this->isLMBPressed = isLMBPressed;}
	void setxPos(int xPos) { this->xPos = xPos;}
	void setyPos(int yPos) { this->yPos = yPos;}

	bool getisLMBPressed() { return isLMBPressed; }
	int getxPos() { return xPos; }
	int getyPos() { return yPos; }
	bool getIn3DWindow() {
		return in3DWindow;
	}
	void setIn3DWindow(bool in3DWindow) {
		this->in3DWindow = in3DWindow;
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
	int xPos = 0, yPos = 0;

};