#pragma once

#include "Utilities.h"
#include "Model.h"
#include "UIElement.h"
#include "UIArrow.h"
#include "UIGizmoCenter.h"

class Outliner {
public:
	Outliner() {}
	~Outliner() {}

	void init() {}

	Model* getModel(int index) {
		if (index < models.size() && index >= 0)
			return models[index];
		std::cout << "ERROR::OUTLINER::getModel - model index out of bounds" << std::endl;
		Model* nullModel = new Model;
		return nullModel;
	}

	UIElement* getUIElement(int index) {
		if (index < uiElements.size() && index >= 0)
			return uiElements[index];
		std::cout << "ERROR::OUTLINER::getUIElement - element index out of bounds" << std::endl;
		UIElement* nullElement = new UIElement;
		return nullElement;
	}

	int getNumModels() { return (int)models.size(); }
	int getNumUIElements() { return (int)uiElements.size(); }

	void addModel(Model* model) {
		models.push_back(model);
	}

	void addUIElement(UIArrow* uiElement) {
		uiElement->setIndex(getNumUIElements());
		uiElements.push_back(uiElement);
	}

	void addUIElement(UIGizmoCenter* uiElement) {
		uiElement->setIndex(getNumUIElements());
		uiElements.push_back(uiElement);
	}
private:
	std::vector<Model*> models;
	std::vector<UIElement*> uiElements;
};
