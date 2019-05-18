open System.Windows.Forms

#version 330 core
in vec2 TexCoords;
out vec4 color;

uniform sampler2D fontTexture;
uniform vec4 textColor;
uniform int mask;

void main()
{    
    vec4 FragColor = texture2D(fontTexture, TexCoords);
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(fontTexture, TexCoords).r);
    color = textColor * sampled * FragColor;
	color.a = min(FragColor.a, textColor.a);

    // Creating the array with life level
    int lifeLvl[];
    for (unsigned int i; i <= 0; i++)
    {
        if(i < mask)
            lifeLvl[i] = 0;
        if(if >= mask)
            lifeLvl[i] = 1;
    }

    // Passing the life level array to UVO.t
    for (unsigned int i; i <= 0; i++)
    {
        UV0.t = lifeLvl[i];
    }
}  