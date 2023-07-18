#pragma once

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "UIElement.h"
#include <vector>
#include <iostream>


class UIArrow : public UIElement {
public:
	UIArrow() {	}

	void init() override {
		setType(Utilities::GizmoXAxisArrow);
		Utilities::generateCone(12, 0.5f, 1.0f, vertices, indices, glm::vec3(0.0f));
		Utilities::generateCylinder(12, 0.25, 1.0f, vertices, indices, glm::vec3(0.0f));
		axis = Utilities::X;
		setVAO();
	}

	void init(Utilities::axis axis, Utilities::UIElementType type) {
		setType(type);
		this->axis = axis;
		Utilities::generateCone(32, 0.25f, 0.5f, vertices, indices, glm::vec3(0.0f, 0.0f, 2.0f));
		Utilities::generateCylinder(32, 0.05, 2.0f, vertices, indices, glm::vec3(0.0f, 0.0f, 1.0f));
		setVAO();
	}

	void draw(Shader *shader, glm::mat4 model) override{
		shader->use();
		shader->setBool("isHighlighted", isHighlighted);
		//glm::mat4 model = glm::mat4(1.0f);
		switch (axis) {
		case Utilities::X:
			model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			shader->setMat4("model", model);
			shader->setVec3("color", glm::vec3(1.0f, 0.0f, 0.0f));
			break;
		case Utilities::Y:
			model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			shader->setMat4("model", model);
			shader->setVec3("color", glm::vec3(0.0f, 1.0f, 0.0f));
			break;
		case Utilities::Z:
			shader->setMat4("model", model);
			shader->setVec3("color", glm::vec3(0.0f, 0.0f, 1.0f));
			break;
		}
		glDisable(GL_DEPTH_TEST);
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
		glEnable(GL_DEPTH_TEST);
		glBindVertexArray(0);
	}

	void showTables() override {
		static ImGuiTableFlags flags =
			ImGuiTableFlags_Borders |
			ImGuiTableFlags_RowBg |
			ImGuiTableFlags_SizingFixedFit |
			ImGuiTableFlags_ScrollY |
			ImGuiTableFlags_ScrollX;
		ImVec2 outer_size = ImVec2(0.0f, ImGui::GetTextLineHeightWithSpacing() * 8);
		if (ImGui::TreeNode("Vertices")) {
			if (ImGui::BeginTable("Vertices", 5, flags, outer_size))
			{
				ImGui::TableSetupScrollFreeze(1, 1);
				ImGui::TableSetupColumn(" ");
				ImGui::TableSetupColumn("X");
				ImGui::TableSetupColumn("Y");
				ImGui::TableSetupColumn("Z");
				ImGui::TableSetupColumn("Connected Indices");
				ImGui::TableHeadersRow();

				for (int row = 0; row < getNumVertices(); row++)
				{
					ImGui::TableNextRow();
					for (int column = 0; column < 5; column++)
					{
						ImGui::TableSetColumnIndex(column);
						Utilities::Vertex currVertex = getVertex(row);
						char buf[64];
						if (column == 0)
							sprintf_s(buf, "%i ", row);
						else if (column == 1)
							if (currVertex.Position.x < 0)
								sprintf_s(buf, "%f", currVertex.Position.x);
							else
								sprintf_s(buf, " %f", currVertex.Position.x);
						else if (column == 2)
							if (currVertex.Position.y < 0)
								sprintf_s(buf, "%f", currVertex.Position.y);
							else
								sprintf_s(buf, " %f", currVertex.Position.y);
						else if (column == 3)
							if (currVertex.Position.z < 0)
								sprintf_s(buf, "%f", currVertex.Position.z);
							else
								sprintf_s(buf, " %f", currVertex.Position.z);
						else if (column == 4) {
							std::string indices = "";
							for (int i = 0; i < currVertex.connectedVertsIndices.size(); i++) {
								indices += std::to_string(currVertex.connectedVertsIndices[i]) + ", ";
							}
							sprintf_s(buf, "%s", indices.c_str());
						}
						ImGui::TextUnformatted(buf);
					}
				}
				ImGui::EndTable();
			}
			ImGui::TreePop();
		}
	}

	

private:

	unsigned int VAO, VBO, EBO;
	Utilities::axis axis;
	
	void generateCone(int numDivisions, float radius) {
		Utilities::Vertex tempVert;
		for (int i = 0; i < numDivisions; i++) {
			const float degrees = i * 360 / numDivisions;
			tempVert.Position = glm::vec3(
				radius * sin(glm::radians(degrees)),
				radius * cos(glm::radians(degrees)),
				0.0f
			);
			vertices.push_back(tempVert);
			addVertex(tempVert);
		}
		tempVert.Position = glm::vec3(0.0f, 0.0f, -1.0f);
		vertices.push_back(tempVert);
		addVertex(tempVert);

		int tipOfCone = vertices.size() - 1;
		int lastCircleVert = vertices.size() - 2;

		for (int i = 0; i < tipOfCone; i++) {
			if (i < lastCircleVert) {
				indices.push_back(i);
				indices.push_back(i + 1);
				indices.push_back(tipOfCone);
			}
			else {
				indices.push_back(i);
				indices.push_back(0);
				indices.push_back(tipOfCone);
			}
		}
	}

	void setVAO() {
		VAO = Utilities::generateVAO(VAO, VBO, EBO, vertices, indices);
	}
};