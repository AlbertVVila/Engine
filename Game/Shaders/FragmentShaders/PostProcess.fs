#version 330 core

out vec4 color;

uniform sampler2D gColor;
uniform sampler2D gHighlight;

in vec2 UV0;

void main()
{
	color = vec4(1,0,0,1);
}
