#version 460 core

out vec4 FragColor;

uniform vec4 outColor;

void main()
{
	FragColor = outColor;
}