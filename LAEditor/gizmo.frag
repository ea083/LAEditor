#version 330 core

out vec4 FragColor;

uniform vec3 color;
uniform bool isHighlighted;

void main()
{
	//FragColor = vec4(1.0, 0.0, 0.0, 1.0);
	FragColor = vec4(color, 1.0);
	if(isHighlighted)
		FragColor = vec4(1.0f, 1.0f, 0.5f  , 1.0);
		
}