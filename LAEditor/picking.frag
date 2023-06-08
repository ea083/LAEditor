#version 330

uniform unsigned int objectIndex;
uniform unsigned int drawIndex;

out uvec3 FragColor;

void main() {
	FragColor = uvec3(objectIndex, drawIndex, gl_PrimitiveID);
}