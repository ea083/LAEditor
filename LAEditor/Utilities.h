#pragma once

#include <vector>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <chrono>

namespace Utilities {

	enum axis {
		X,
		Y,
		Z
	};

	enum UIElementType {
		GizmoXAxisArrow,
		GizmoYAxisArrow,
		GizmoZAxisArrow,
		GizmoCenter
	};

	std::string getUIElementNameFromType(UIElementType type) {
		std::string returnString;
		switch (type) {
		case GizmoXAxisArrow:
			returnString = std::string("Gizmo X Axis Arrow");
			return returnString;
		case GizmoYAxisArrow:
			returnString = std::string("Gizmo Y Axis Arrow");
			return returnString;
		case GizmoZAxisArrow:
			returnString = std::string("Gizmo Z Axis Arrow");
			return returnString;
		case GizmoCenter:
			returnString = std::string("Gizmo Center");
			return returnString;
		default:
			returnString = std::string("");
			return returnString;
		}
	}

	std::string returnAStringasdfasfasdfasdf() {
		std::string aString = std::string("asdf");
		return aString;
	}

	void nameVariableDebugTable(int varVal, std::string varName) {
		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);
		ImGui::TextUnformatted(varName.c_str());
		ImGui::TableSetColumnIndex(1);
		ImGui::TextUnformatted(std::to_string(varVal).c_str());
	}

	void nameVariableDebugTable(long varVal, std::string varName) {
		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);
		ImGui::TextUnformatted(varName.c_str());
		ImGui::TableSetColumnIndex(1);
		ImGui::TextUnformatted(std::to_string(varVal).c_str());
	}

	void nameVariableDebugTable(float varVal, std::string varName) {
		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);
		ImGui::TextUnformatted(varName.c_str());
		ImGui::TableSetColumnIndex(1); 
		ImGui::TextUnformatted(std::to_string(varVal).c_str());
	}

	void nameVariableDebugTable(bool varVal, std::string varName) {
		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);
		ImGui::TextUnformatted(varName.c_str());
		ImGui::TableSetColumnIndex(1);
		ImGui::TextUnformatted(std::to_string(varVal).c_str());
	}

	long getCurrentTimeInMS() {
		auto currentTime = std::chrono::system_clock::now();
		auto currentTimeMs = std::chrono::time_point_cast<std::chrono::milliseconds>(currentTime);
		auto currentTimeMsValue = currentTimeMs.time_since_epoch().count();
		return currentTimeMsValue;
	}

	struct Vertex {
		glm::vec3 Position;
		glm::vec3 Normal;

		std::vector<Vertex*> connectedVerts;
		std::vector<unsigned int> connectedVertsIndices;
	};

	struct MVP {
		glm::mat4 model;
		glm::mat4 view;
		glm::mat4 projection;

		MVP() : model(glm::mat4(1.0f)), view(glm::mat4(1.0f)), projection(glm::mat4(1.0f)) {}
	};

	struct PixelData {
		float r;
		float g;
		float b;
	};

	double discriminant(glm::vec3 rayDirection, glm::vec3 rayOrigin, glm::vec3 sphereCenter, double sphereRadius) {
		glm::vec3 rayOriginToSphere = rayOrigin - sphereCenter;
		float a = glm::dot(rayDirection, rayDirection);
		float b = 2.0f * glm::dot(rayOriginToSphere, rayDirection);
		float c = glm::dot(rayOriginToSphere, rayOriginToSphere) - (sphereRadius * sphereRadius);

		return (b * b) - (4.0f * a * c);
	}

	bool raySphereIntersection(glm::vec3 rayDirection, glm::vec3 rayOrigin, glm::vec3 sphereOrigin, double sphereRadius) {
		glm::vec3 rayOriginToSphere = sphereOrigin - rayOrigin;
		float t = glm::dot(rayOriginToSphere, rayDirection);

		glm::vec3 p = rayDirection * t;
		p = rayOrigin + p;
		float y = glm::length(sphereOrigin - p);

		return y < sphereRadius;
	}

	float RGBToFloat(int value) {
		return static_cast<float>(value) / 255.0f;
	}

	unsigned int generateVAO(
		unsigned int VAO, 
		unsigned int VBO, 
		unsigned int EBO, 
		std::vector<Vertex> vertices, 
		std::vector<unsigned int> indices) {

		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Utilities::Vertex), &vertices[0], GL_DYNAMIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_DYNAMIC_DRAW);

		// vertex Positions
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Utilities::Vertex), (void*)0);
		// vertex normals
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Utilities::Vertex), (void*)offsetof(Utilities::Vertex, Normal));
		glBindVertexArray(0);

		return VAO;

	}

	void generateCone(int numDivisions,
		float radius,
		float height,
		std::vector<Vertex>& vertices,
		std::vector<unsigned int>& indices,
		glm::vec3 offset) {

		std::vector<Vertex> tempVertices;
		std::vector<unsigned int> tempIndices;
		Vertex tempVert;
		for (int i = 0; i < numDivisions; i++) {
			const float degrees = i * 360 / numDivisions;
			tempVert.Position = glm::vec3(
				radius * sin(glm::radians(degrees)),
				radius * cos(glm::radians(degrees)),
				0.0f
			);
			tempVertices.push_back(tempVert);
		}
		// tip of cone
		tempVert.Position = glm::vec3(0.0f, 0.0f,  1.0f * height);
		tempVertices.push_back(tempVert);
		int tipOfCone = tempVertices.size() - 1;

		// center of circle
		tempVert.Position = glm::vec3(0.0f);
		tempVertices.push_back(tempVert);
		int centerOfCircle = tempVertices.size() - 1;

		int lastCircleVert = tempVertices.size() - 3;

		for (int i = 0; i < tipOfCone; i++) {
			if (i < lastCircleVert) {
				tempIndices.push_back(i);
				tempIndices.push_back(i + 1);
				tempIndices.push_back(tipOfCone);


				tempIndices.push_back(i);
				tempIndices.push_back(i + 1);
				tempIndices.push_back(centerOfCircle);
			}
			else {
				tempIndices.push_back(i);
				tempIndices.push_back(0);
				tempIndices.push_back(tipOfCone);

				tempIndices.push_back(i);
				tempIndices.push_back(0);
				tempIndices.push_back(centerOfCircle);
			}
		}

		for (int i = 0; i < tempIndices.size(); i++) {
			indices.push_back(tempIndices[i] + vertices.size());
		}

		for (int i = 0; i < tempVertices.size(); i++) {
			tempVertices[i].Position += offset;
			vertices.push_back(tempVertices[i]);
		}

	}

	void generateCylinder(int numDivisions, 
		float radius, 
		float height, 
		std::vector<Vertex>& vertices, 
		std::vector<unsigned int>& indices,
		glm::vec3 offset) {

		std::vector<Vertex> tempVertices;
		std::vector<unsigned int> tempIndices;
		Vertex tempVert;
		for (int i = 0; i < numDivisions; i++) {
			const float degrees = i * 360 / numDivisions;
			// 0, 2, 4 ... are bot of cylinder
			tempVert.Position = glm::vec3(
				radius * sin(glm::radians(degrees)),
				radius * cos(glm::radians(degrees)),
				-height / 2.0f
			);
			tempVertices.push_back(tempVert);

			// 1, 3, 5 ... are top of cylinder
			tempVert.Position = glm::vec3(
				radius * sin(glm::radians(degrees)),
				radius * cos(glm::radians(degrees)),
				height / 2.0f
			);
			tempVertices.push_back(tempVert);
		}
		// center of top circle
		tempVert.Position = glm::vec3(0.0f, 0.0f, height / 2.0f);
		tempVertices.push_back(tempVert);
		int centerOfTopcircle = tempVertices.size() - 1;

		// center of bot circle
		tempVert.Position = glm::vec3(0.0f, 0.0f, -height / 2.0f);
		tempVertices.push_back(tempVert);
		int centerOfBotCircle = tempVertices.size() - 1;

		int lastCircleVert = tempVertices.size() - 3;

		for (int i = 0; i < tempVertices.size() - 2; i++) {
			if (i < lastCircleVert - 1) {
				tempIndices.push_back(i);
				tempIndices.push_back(i + 1);
				tempIndices.push_back(i + 2);


				tempIndices.push_back(i);
				tempIndices.push_back(i + 2);
				if(i % 2 == 0)
					tempIndices.push_back(centerOfBotCircle);
				else
					tempIndices.push_back(centerOfTopcircle);
			}
			else if (i == lastCircleVert - 1) {
				tempIndices.push_back(i);
				tempIndices.push_back(i+1);
				tempIndices.push_back(0);

				tempIndices.push_back(i);
				tempIndices.push_back(0);
				if (i % 2 == 0)
					tempIndices.push_back(centerOfBotCircle);
				else
					tempIndices.push_back(centerOfTopcircle);
			}
			else {
				tempIndices.push_back(i);
				tempIndices.push_back(0);
				tempIndices.push_back(1);

				tempIndices.push_back(i);
				tempIndices.push_back(1);
				if (i % 2 == 0)
					tempIndices.push_back(centerOfBotCircle);
				else
					tempIndices.push_back(centerOfTopcircle);
			}
		}

		for (int i = 0; i < tempIndices.size(); i++) {
			indices.push_back(tempIndices[i] + vertices.size());
		}

		for (int i = 0; i < tempVertices.size(); i++) {
			tempVertices[i].Position += offset;
			vertices.push_back(tempVertices[i]);
		}

	}

	void generateCube(float edgeLength, std::vector<Vertex> &vertices, std::vector<unsigned int> &indices, glm::vec3 offset) {
		generateCylinder(4, edgeLength/2, edgeLength*0.75, vertices, indices, offset);
	}
}