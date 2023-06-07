#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


#include "Shader.h"


class Grid {
public:
	Grid() {
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(gridVertices), gridVertices, GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(gridIndices), gridIndices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
	}

	void render(glm::mat4 projection, glm::mat4 view, glm::vec3 cameraPos, float radius) {
		gridShader.use();
		gridShader.setMat4("projection", projection);
		gridShader.setMat4("view", view);
		gridShader.setVec3("cameraPos", cameraPos);
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(cameraPos.x, 0, cameraPos.z));
		model = glm::scale(model, glm::vec3(1000.0f));
		gridShader.setMat4("model", model);
		gridShader.setFloat("radius", radius);
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	}

private:
	Shader gridShader = Shader("grid.vert", "grid.frag");
	float gridVertices[12] = {
			0.5f, 0.0f,  0.5f, // top right
			0.5f, 0.0f, -0.5f, // bottom right
			-0.5f, 0.0f, -0.5f, // bottom left
			-0.5f, 0.0f,  0.5f, // top left
	};
	unsigned int gridIndices[6] = {
		0, 1, 3,
		1, 2, 3
	};
	unsigned int VBO, VAO, EBO;
};