#version 330 core

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BackGroundColor;

in vec3 TexCoords;

uniform samplerCube skybox;

void main()
{    
    FragColor = texture(skybox, TexCoords);
	BackGroundColor = vec4(0,0,0,0);
}