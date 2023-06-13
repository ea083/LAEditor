#version 330 core

layout (location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 viewPos;

uniform float offset;

uniform int selectedIndex;

flat out int index;

void main()
{
	gl_Position = projection * view * model * vec4(aPos + 0.01 * (viewPos - aPos), 1.0);
	gl_PointSize = 5;
	index = gl_VertexID;
}