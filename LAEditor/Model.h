#pragma once

#include <glad/glad.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
//#define STB_IMAGE_IMPLEMENTATION
//#include "stb_image.h"
#include <vector>
#include <iostream>
#include <iomanip>

#include "Utilities.h"
#include "GLUtilities.h"
#include "Shader.h"

//#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
//#include <CGAL/Delaunay_triangulation_3.h>
//
//typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
//typedef CGAL::Delaunay_triangulation_3<K> Delaunay;
//typedef K::Point_3 Point_3;

struct face {
	std::vector<unsigned int> vertIndices;
};

class Model {
public:
	Model() : EBO(0), VAO(0), VBO(0), edgeEBO(0), edgeVAO(0), edgeVBO(0), selectedVertEBO(0), selectedVertVAO(0), selectedVertVBO(0) {

	}
	Model(std::string const& path) : EBO(0), VAO(0), VBO(0), edgeEBO(0), edgeVAO(0), edgeVBO(0), selectedVertEBO(0), selectedVertVAO(0), selectedVertVBO(0) {
		loadModel(path);
	}

	void Draw(Shader& shader, Shader* vertShader = NULL, Shader* selectedVertShader = NULL, Shader* edgeShader = NULL, bool ignoreFaces = false) {
		glBindVertexArray(VAO);
		shader.use();
		shader.setBool("textured", false);
		shader.setBool("ignore", ignoreFaces);
		glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);

		if (vertShader != NULL) {
			glEnable(GL_PROGRAM_POINT_SIZE);
			vertShader->use();
			vertShader->setBool("ignore", false);
			glDrawElements(GL_POINTS, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
		}
		if (selectedVertShader != NULL) {
			glDisable(GL_DEPTH_TEST);
			glBindVertexArray(selectedVertVAO);
			glEnable(GL_PROGRAM_POINT_SIZE);
			selectedVertShader->use();
			selectedVertShader->setBool("ignore", false);
			glDrawElements(GL_POINTS, static_cast<unsigned int>(selectedVerticesIndices.size()), GL_UNSIGNED_INT, 0);
			glEnable(GL_DEPTH_TEST);

		}
		if (edgeShader != NULL) {
			glBindVertexArray(edgeVAO);
			edgeShader->use();
			glDrawElements(GL_LINES, static_cast<unsigned int>(edgeIndices.size()), GL_UNSIGNED_INT, 0);
		}

		glBindVertexArray(0);
	}
	void getInterceptedVertices(glm::vec3 Ray, glm::vec3 cameraPos) {
		selectedVerticesIndices.clear();
		for (int i = 0; i < vertices.size(); i++) {
			float dist = glm::distance(cameraPos, vertices[i].Position);
			if (Utilities::raySphereIntersection(Ray, cameraPos, vertices[i].Position, 0.025f * dist)) {
				selectedVerticesIndices.push_back(i);
			}
			/*if (Utilities::discriminant(Ray, cameraPos, vertices[i].Position, 0.1f * dist) >= 0) {
				std::cout << "Intercepted Vertex " << i << " at pos " <<
					vertices[i].Position.x << ", " <<
					vertices[i].Position.y << ", " <<
					vertices[i].Position.z << std::endl;
				selectedVerticesIndices.push_back(i);
			}*/
		}
		setSelectedVerticeVAO();

	}
	void setSelectedVertices(Utilities::PixelData pixelData) {
		selectedVerticesIndices.clear();
		int index = (int)(pixelData.r - 1);
		if (index < 0 || index > vertices.size()) {
			std::cout << "ERROR::MODEL2::Selected Vertice out of range!" << std::endl;
			return;
		}
		selectedVerticesIndices.push_back(index);
		for (int i = 0; i < vertices[index].connectedVertsIndices.size(); i++)
			selectedVerticesIndices.push_back(vertices[index].connectedVertsIndices[i]);

		setSelectedVerticeVAO();
	}
	int getNumSelectedVertices() {
		return (int)selectedVerticesIndices.size();
	}
	glm::vec3 getSelectedVerticesMedian() {
		glm::vec3 median = glm::vec3(0.0f);
		for (const unsigned int index : selectedVerticesIndices) {
			median += vertices[index].Position;
		}
		median /= selectedVerticesIndices.size();
		return median;
	}
	void translateSelectedVertices(float distance, glm::vec3 direction) {
		const glm::vec3 delta = distance * glm::normalize(direction);
		for (int i = 0; i < selectedVerticesIndices.size(); i++) {
			const int currIndex = selectedVerticesIndices[i];
			vertices[currIndex].Position += delta;
			//for (int j = 0; j < vertices[currIndex].connectedVerts.size(); j++)
			//	vertices[currIndex].connectedVerts[j]->Position += delta;

		}
		setVAO();
		setSelectedVerticeVAO();
	}

	void setSelectedVerticesMedianPos(glm::vec3 new_pos) {
		const glm::vec3 old_pos = getSelectedVerticesMedian();
		const glm::vec3 delta = new_pos - old_pos;
		for (int i = 0; i < selectedVerticesIndices.size(); i++) {
			const int currIndex = selectedVerticesIndices[i];
			vertices[currIndex].Position += delta;
		}
		setVAO();
		setSelectedVerticeVAO();
	}

	Utilities::Vertex getVertex(int index) {
		if (index < vertices.size() && index >= 0)
			return vertices[index];
		std::cout << "ERROR::MODEL2::getVertex - Vertex index out of bounds" << std::endl;
		Utilities::Vertex nullVertex;
		return nullVertex;
	}
	face getFace(int index) {
		if (index < faces.size() && index >= 0)
			return faces[index];
		std::cout << "ERROR::MODEL2::getFace - Face index out of bounds" << std::endl;
		face nullFace;
		return nullFace;
	}
	unsigned int getIndice(int index) {
		if (index < indices.size() && index >= 0)
			return indices[index];
		std::cout << "ERROR::MODEL2::getIndice - Indice index out of bounds" << std::endl;
		return NULL;
	}
	unsigned int getEdgeIndice(int index) {
		if (index < edgeIndices.size() && index >= 0)
			return edgeIndices[index];
		std::cout << "ERROR::MODEL2::getEdgeIndice - Indice index out of bounds" << std::endl;
		return NULL;
	}

	int getNumVertices() {
		return (int)vertices.size();
	}
	int getNumFaces() {
		return (int)faces.size();
	}
	int getNumIndices() {
		return (int)indices.size();
	}
	int getNumEdgeIndices() {
		return (int)edgeIndices.size();
	}
	std::string getName() { return name; }

	void debug() {
		if (ImGui::TreeNode("Model")) {
			if (ImGui::TreeNode("Vertices")) {
				if (Utilities::startDebugTable("Vertices", "Index", "Value")) {
					for (int i = 0; i < vertices.size(); i++) {
						Utilities::nameVariableDebugTable(vertices[i].Position, i + " position");
						Utilities::nameVariableDebugTable(vertices[i].Normal, i + " normal");
					}
					ImGui::EndTable();
				}
				ImGui::TreePop();
			}
			if (ImGui::TreeNode("Selected Vertices Indices")) {
				if (Utilities::startDebugTable("Vertices", "Index", "Value")) {
					for (int i = 0; i < selectedVerticesIndices.size(); i++) {
						Utilities::nameVariableDebugTable(selectedVerticesIndices[i], std::to_string(i));
					}
					ImGui::EndTable();
				}
				ImGui::TreePop();
			}
			ImGui::TreePop();
		}
	}

private:
	std::string directory;
	std::string name;
	std::vector<Utilities::Vertex> vertices;
	std::vector<face> faces;
	std::vector<unsigned int> indices;
	std::vector<unsigned int> edgeIndices;
	std::vector<unsigned int> selectedVerticesIndices;


	unsigned int VAO, VBO, EBO;
	unsigned int selectedVertVAO, selectedVertVBO, selectedVertEBO;
	unsigned int edgeVAO, edgeVBO, edgeEBO;

	void loadModel(std::string path) {
		Assimp::Importer import;
		const aiScene* scene = import.ReadFile(path, /*aiProcess_Triangulate |*/ aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace);

		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
			std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
			return;
		}
		if (path.find('/') == std::string::npos) {
			name = path;
			directory = "";
		}
		else {
			directory = path.substr(0, path.find_last_of('/'));
			name = path.substr(path.find_last_of('/'), (path.size() - 1) - path.find_last_of('/'));
		}
		storeModel(scene);
		//displayData(scene);
	}
	void storeModel(const aiScene* scene) {
		for (unsigned int i = 0; i < scene->mNumMeshes; i++) {
			for (unsigned int j = 0; j < scene->mMeshes[i]->mNumVertices; j++) {
				Utilities::Vertex tempVertex;
				tempVertex.Position.x = scene->mMeshes[i]->mVertices[j].x;
				tempVertex.Position.y = scene->mMeshes[i]->mVertices[j].y;
				tempVertex.Position.z = scene->mMeshes[i]->mVertices[j].z;

				tempVertex.Normal.x = scene->mMeshes[i]->mNormals[j].x;
				tempVertex.Normal.y = scene->mMeshes[i]->mNormals[j].y;
				tempVertex.Normal.z = scene->mMeshes[i]->mNormals[j].z;
				vertices.push_back(tempVertex);
			}

			for (unsigned int j = 0; j < scene->mMeshes[i]->mNumFaces; j++) {
				face tempFace;
				for (unsigned int k = 0; k < scene->mMeshes[i]->mFaces[j].mNumIndices; k++) {
					tempFace.vertIndices.push_back(scene->mMeshes[i]->mFaces[j].mIndices[k]);
				}
				faces.push_back(tempFace);
			}
		}

		for (unsigned int i = 0; i < faces.size(); i++) {
			for (unsigned int j = 0; j < faces[i].vertIndices.size(); j++) {
				if (j < faces[i].vertIndices.size() - 1) {
					edgeIndices.push_back(faces[i].vertIndices[j]);
					edgeIndices.push_back(faces[i].vertIndices[j + 1]);
				}
				else {
					edgeIndices.push_back(faces[i].vertIndices[j]);
					edgeIndices.push_back(faces[i].vertIndices[0]);
				}
			}
		}


		setConnectedVertices();
		triangulateFaces();

		setVAO();
		setSelectedVerticeVAO();
	}

	void setVAO() {
		// vertice VAO
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

		// edge VAO
		glGenVertexArrays(1, &edgeVAO);
		glGenBuffers(1, &edgeVBO);
		glGenBuffers(1, &edgeEBO);

		glBindVertexArray(edgeVAO);
		glBindBuffer(GL_ARRAY_BUFFER, edgeVBO);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Utilities::Vertex), &vertices[0], GL_DYNAMIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, edgeEBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, edgeIndices.size() * sizeof(unsigned int), &edgeIndices[0], GL_DYNAMIC_DRAW);

		// vertex Positions
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Utilities::Vertex), (void*)0);
		// vertex normals
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Utilities::Vertex), (void*)offsetof(Utilities::Vertex, Normal));

		glBindVertexArray(0);
	}
	void setSelectedVerticeVAO() {
		if (selectedVerticesIndices.size() > 0) {
			glGenVertexArrays(1, &selectedVertVAO);
			glGenBuffers(1, &selectedVertVBO);
			glGenBuffers(1, &selectedVertEBO);

			glBindVertexArray(selectedVertVAO);
			glBindBuffer(GL_ARRAY_BUFFER, selectedVertVBO);
			glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Utilities::Vertex), &vertices[0], GL_DYNAMIC_DRAW);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, selectedVertEBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, selectedVerticesIndices.size() * sizeof(unsigned int), &selectedVerticesIndices[0], GL_DYNAMIC_DRAW);

			// vertex Positions
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Utilities::Vertex), (void*)0);
			// vertex normals
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Utilities::Vertex), (void*)offsetof(Utilities::Vertex, Normal));
		}
		glBindVertexArray(0);
	}

	void triangulateFaces() {
		indices.clear();
		for (face currFace : faces) {
			std::vector<unsigned int> triangulatedFaces = traingulateFace(currFace);
			for (int i = 0; i < triangulatedFaces.size(); i++) {
				indices.push_back(triangulatedFaces[i]);
			}
		}
	}
	std::vector<unsigned int> traingulateFace(face currFace) {
		std::vector<unsigned int> triangulatedFaces;
		if (currFace.vertIndices.size() < 3)
			return triangulatedFaces;
		if (currFace.vertIndices.size() == 3) {
			triangulatedFaces.push_back(currFace.vertIndices[0]);
			triangulatedFaces.push_back(currFace.vertIndices[1]);
			triangulatedFaces.push_back(currFace.vertIndices[2]);
			return triangulatedFaces;
		}

		std::vector<unsigned int> remainingVertices = currFace.vertIndices;
		while (remainingVertices.size() > 3) {
			triangulatedFaces.push_back(remainingVertices[0]);
			triangulatedFaces.push_back(remainingVertices[1]);
			triangulatedFaces.push_back(remainingVertices[2]);
			remainingVertices.erase(remainingVertices.begin() + 1);
		}

		triangulatedFaces.push_back(remainingVertices[0]);
		triangulatedFaces.push_back(remainingVertices[1]);
		triangulatedFaces.push_back(remainingVertices[2]);


		if (triangulatedFaces.size() % 3 != 0)
			std::cout << "Triangulated Faces not in 3s!!!" << std::endl;
		return triangulatedFaces;

	}
	void setConnectedVertices() {
		for (int i = 0; i < vertices.size(); i++) {
			for (int j = 0; j < vertices.size(); j++) {
				if (i == j)
					continue;
				if (vertices[i].Position == vertices[j].Position) {
					vertices[i].connectedVerts.push_back(&vertices[j]);
					vertices[i].connectedVertsIndices.push_back(j);
				}
			}
		}
	}
	void displayData(const aiScene* scene) {
		for (unsigned int i = 0; i < scene->mNumMeshes; i++) {
			std::cout << "Mesh Name: " << scene->mMeshes[i]->mName.C_Str() << std::endl;
			std::cout << "Num Vertices: " << scene->mMeshes[i]->mNumVertices << std::endl;
			std::cout << "vertices: " << std::endl;
			for (unsigned int j = 0; j < scene->mMeshes[i]->mNumVertices; j++) {
				std::cout << std::fixed << std::setprecision(3) <<
					"Index: " << j <<
					"\tX: " << scene->mMeshes[i]->mVertices[j].x <<
					"\t Y: " << scene->mMeshes[i]->mVertices[j].y <<
					"\t Z: " << scene->mMeshes[i]->mVertices[j].z << std::endl;
			}
			std::cout << "Num Faces: " << scene->mMeshes[i]->mNumFaces << std::endl;
			std::cout << "Faces: " << std::endl;
			for (unsigned int j = 0; j < scene->mMeshes[i]->mNumFaces; j++) {
				std::cout << "\tNum Indices: " << scene->mMeshes[i]->mFaces[j].mNumIndices << std::endl;
				for (unsigned int k = 0; k < scene->mMeshes[i]->mFaces[j].mNumIndices; k++) {
					std::cout << "\t\t" << scene->mMeshes[i]->mFaces[j].mIndices[k] << std::endl;
				}
			}
		}
	}
};
