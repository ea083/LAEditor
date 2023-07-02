#pragma once

#include "Utilities.h"
#include "UIArrow.h"
#include "Outliner.h"
#include <iostream>

class Gizmo {
public:
	Gizmo() {}

	void init() {
		xArrow.init(Utilities::X, "XAxisArrow");
	}

	void draw(Shader *shader) {
		xArrow.draw(shader);
	}

	void addToOutliner(Outliner* outliner) {
		outliner->addUIElement(&xArrow);
	}
private:
	UIArrow xArrow;
};