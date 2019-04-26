#version 330 core
in vec2 TexCoords;
out vec4 color;

uniform sampler2D texture;
uniform int xTiles;
uniform int yTiles;

uniform int f1Xpos;
uniform int f1Ypos;
uniform int f2Xpos;
uniform int f2Ypos;
uniform float mixAmount;
uniform vec3 particleColor;

in vec2 tCoords;


void main()
{   
	float cellS = tCoords.s / float(xTiles);
	float cellT = tCoords.t / float(yTiles);
	float invX = 1 / float(xTiles);
	float invY = 1 / float(yTiles);
	vec2 tC1 = vec2(cellS + (invX * f1Xpos), cellT + (invY * f1Ypos));
	vec2 tC2 = vec2(cellS + (invX * f2Xpos), cellT + (invY * f2Ypos));
	color = mix(texture2D(texture, tC1), texture2D(texture, tC2) , mixAmount) * vec4(particleColor, 1.0f);
}  