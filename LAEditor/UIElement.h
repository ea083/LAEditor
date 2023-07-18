#pragma once
#include "Utilities.h"
#include "Shader.h"

class UIElement {
public:
	virtual void init() {

	}
	virtual void draw(Shader* shader, glm::mat4 model) {
		 
	}

	virtual void showTables() {
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

	virtual void setIsHighlighted(bool isHighlighted) {
		this->isHighlighted = isHighlighted;
	}

	Utilities::Vertex getVertex(int index) {
		if (index < vertices.size() && index >= 0)
			return vertices[index];
		std::cout << "ERROR::UIELEMENT::getVertex - Vertex index out of bounds" << std::endl;
		Utilities::Vertex nullVertex;
		return nullVertex;
	}

	void addVertex(Utilities::Vertex newVertex) {
		vertices.push_back(newVertex);
	}

	int getNumVertices() {
		return vertices.size();
	}

	Utilities::UIElementType getType() {
		return type;
	}

	void setType(Utilities::UIElementType newType) {
		type = newType;
	}

	void setIndex(int index) {
		outlinerIndex = index;
	}

	int getIndex() {
		return outlinerIndex;
	}

private:
	Utilities::UIElementType type;
	int outlinerIndex;

protected:
	bool isHighlighted = false;
	std::vector<Utilities::Vertex> vertices;
	std::vector<unsigned int> indices;
};