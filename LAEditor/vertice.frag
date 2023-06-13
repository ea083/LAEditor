#version 330 core

out vec4 color;

flat in int index;

uniform int selectedIndex;

void main()
{
	color = vec4(1.0, 0.835, 0.545, 1.0);
	if (index == selectedIndex){
		color = vec4(1.0, 0.0, 0.0, 1.0);
	}
}