#pragma once

#include "Utilities.h"
#include "Model2.h"
#include "UIElement.h"
#include "UIArrow.h"

class Outliner {
public:
	Outliner() {}

	void init() {

	}

	Model2::Model2* getModel(int index) {
		if (index < models.size() && index >= 0)
			return models[index];
		std::cout << "ERROR::OUTLINER::getModel - model index out of bounds" << std::endl;
		Model2::Model2* nullModel = new Model2::Model2;
		return nullModel;
	}

	UIElement* getUIElement(int index) {
		if (index < uiElements.size() && index >= 0)
			return uiElements[index];
		std::cout << "ERROR::OUTLINER::getUIElement - element index out of bounds" << std::endl;
		UIElement* nullElement = new UIElement;
		return nullElement;
	}

	int getNumModels() { return models.size(); }
	int getNumUIElements() { return uiElements.size(); }

	void addModel(Model2::Model2* model) {
		models.push_back(model);
	}

	void addUIElement(UIArrow* uiElement) {
		uiElements.push_back(uiElement);
	}

private:
	std::vector<Model2::Model2*> models;
	std::vector<UIElement*> uiElements;
};