#pragma once
#include "Utilities.h"
#include "Shader.h"

class UIElement {
public:
	virtual void init() {

	}
	virtual void draw(Shader* shader) {
		 
	}

	virtual void showTables() {

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

	std::string getName() {
		return name;
	}

	void setName(std::string newName) {
		name = newName;
	}


private:
	std::string name;

protected:
	std::vector<Utilities::Vertex> vertices;
	std::vector<unsigned int> indices;
};