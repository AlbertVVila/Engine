#version 330 core
out vec4 Fragcolor;
in vec4 vertexColor;
uniform vec4 Vcolor;

void main()
{
 Fragcolor= Vcolor;

}
