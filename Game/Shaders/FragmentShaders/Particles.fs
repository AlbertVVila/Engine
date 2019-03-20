#version 330 core
in vec2 TexCoords;
out vec4 color;

uniform sampler2D texture;

in vec2 tCoords;

void main()
{    
    /*vec4 FragColor = texture2D(texture, TexCoords);
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(fontTexture, TexCoords).r);*/
    color = texture2D(texture, tCoords);
}  