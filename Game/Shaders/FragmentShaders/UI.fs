#version 330 core

#define MASK_DIVISIONS 20

in vec2 TexCoords;
out vec4 color;

uniform sampler2D fontTexture;
uniform vec4 textColor;
uniform float mask[MASK_DIVISIONS];

void main()
{    
    vec4 FragColor = texture2D(fontTexture, TexCoords);
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(fontTexture, TexCoords).r);
    color = textColor * sampled * FragColor;
	float maskPos = TexCoords.t * MASK_DIVISIONS;	
	color.a = min(FragColor.a, textColor.a) * mask[int(maskPos)];

}  