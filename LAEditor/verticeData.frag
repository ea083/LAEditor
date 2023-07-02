#version 330 core

// uniform uint objectIndex;
//uniform uint drawIndex;

uniform bool ignore;

flat in int index;

out vec4 color;

void main()
{
	color = vec4(float(index + 1 ), float(0), float(1.0), 1.0);
	if (ignore)
		color = vec4(0.0);
	//color = vec4(0.0, 2.0, 0.0, 1.0);
}