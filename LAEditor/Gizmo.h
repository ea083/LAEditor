#pragma once

#include "Utilities.h"
#include "UIArrow.h"
#include "UIGizmoCenter.h"
#include "Outliner.h"
#include <iostream>

#define NOAXISSELECTED 0
#define AXISSELECTED 1
#define CENTERAXISSELECTED 2

class Gizmo {
public:
	Gizmo() {}

	void init() {
		xArrow.init(Utilities::X, Utilities::GizmoXAxisArrow);
		yArrow.init(Utilities::Y, Utilities::GizmoYAxisArrow);
		zArrow.init(Utilities::Z, Utilities::GizmoZAxisArrow);
		gizmoCenter.init();
	}

	void draw(Shader *shader) {
		shader->use();
		shader->setInt("outlinerIndex", xArrow.getIndex());
		xArrow.draw(shader, model);
		shader->setInt("outlinerIndex", yArrow.getIndex());
		yArrow.draw(shader, model);
		shader->setInt("outlinerIndex", zArrow.getIndex());
		zArrow.draw(shader, model);
		shader->setInt("outlinerIndex", gizmoCenter.getIndex());
		gizmoCenter.draw(shader, model);
	}

	void addToOutliner(Outliner* outliner) {
		outliner->addUIElement(&xArrow);
		outliner->addUIElement(&yArrow);
		outliner->addUIElement(&zArrow);
		outliner->addUIElement(&gizmoCenter);
	}

	void translate(glm::vec3 deltaPosition) {
		model = glm::translate(model, deltaPosition);
	}

	void setSelectedAxis(Utilities::axis axis) {
		isAxisSelected = AXISSELECTED;
		selectedAxis = axis;
	}

	void setCenterAxisSelected() {
		isAxisSelected = CENTERAXISSELECTED;
	}

	void deselectAxis() {
		isAxisSelected = NOAXISSELECTED;
	}

private:
	UIArrow xArrow, yArrow, zArrow;
	UIGizmoCenter gizmoCenter;

	glm::mat4 model;
	Utilities::axis selectedAxis;
	int isAxisSelected = 0;
};