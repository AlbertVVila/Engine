#version 330 core
out vec4 FragColor;

in vec2 uv0;

uniform sampler2D texture0;

void main()
{    
    FragColor = texture2D(texture0, uv0);
} 