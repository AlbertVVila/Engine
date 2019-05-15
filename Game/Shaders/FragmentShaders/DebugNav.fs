#version 330 core
out vec4 Fragcolor;

in vec4 fcolor;

void main()
{
	Fragcolor= vec4(fcolor);//vertex_color;
}
