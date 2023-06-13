#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Utilities {

	struct MVP {
		glm::mat4 model;
		glm::mat4 view;
		glm::mat4 projection;

		MVP() : model(glm::mat4(1.0f)), view(glm::mat4(1.0f)), projection(glm::mat4(1.0f)) {}
	};


	double discriminant(glm::vec3 rayDirection, glm::vec3 rayOrigin, glm::vec3 sphereCenter, double sphereRadius) {
		glm::vec3 rayOriginToSphere = rayOrigin - sphereCenter;
		float a = glm::dot(rayDirection, rayDirection);
		float b = 2.0f * glm::dot(rayOriginToSphere, rayDirection);
		float c = glm::dot(rayOriginToSphere, rayOriginToSphere) - (sphereRadius * sphereRadius);

		return (b * b) - (4.0f * a * c);
	}
}