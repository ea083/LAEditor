#pragma once

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "UIElement.h"
#include <vector>
#include <iostream>


class UIGizmoCenter : public UIElement {
public:
	UIGizmoCenter() : EBO(0), VAO(0), VBO(0) {	}

	void init() override {
		setType(Utilities::GizmoCenter);
		Utilities::generateCube(0.5, vertices, indices, glm::vec3(0.0f));
		setVAO();
	}

	void draw(Shader* shader, glm::mat4 model) override {
		shader->use();
		shader->setVec3("color", glm::vec3(1.0f, 1.0f, 1.0f));
		shader->setMat4("model", model);
		shader->setBool("isHighlighted", isHighlighted);
		glDisable(GL_DEPTH_TEST);
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
		glEnable(GL_DEPTH_TEST);
		glBindVertexArray(0);
	}


private:
	unsigned int VAO, VBO, EBO;

	void setVAO() {
		VAO = Utilities::generateVAO(VAO, VBO, EBO, vertices, indices);
	}
};