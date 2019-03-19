#version 330 core
in vec2 TexCoords;
out vec4 color;

uniform sampler2D texture;

void main()
{    
    /*vec4 FragColor = texture2D(texture, TexCoords);
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(fontTexture, TexCoords).r);*/
    color = vec4(1,1,1,1);
}  