#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
//#include "Model.h"
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
	
private:
	float lastX = 400.0f;
	float lastY = 300.0f;
	bool keepCursorInFrame = false;
	bool firstMouse = true;
	bool is3DViewerFocused = true;

	bool isLMBPressed = false;
	int xPos = 0, yPos = 0;

};