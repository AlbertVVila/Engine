layout (location = 0) out vec4 color;

in vec3 TexCoords;

uniform samplerCube skybox;

void main()
{    
    color = texture(skybox, TexCoords);
	color.a = 1;	
#ifndef IS_EDITOR
	color = vec4(pow(color.r, 2.2), pow(color.g, 2.2), pow(color.b, 2.2), color.a); //Gamma correction	
#endif
}