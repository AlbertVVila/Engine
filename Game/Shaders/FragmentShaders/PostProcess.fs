#version 330 core

layout (location = 0) out vec4 color;
layout (location = 1) out vec4 hlight;

uniform sampler2D gColor;
uniform sampler2D gHighlight;

uniform bool horizontal = true;
uniform float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

in vec2 UV0;

void main()
{

	vec2 offset = 1.0 / textureSize(gHighlight, 0);
	vec4 N = texture2D(gHighlight, vec2(UV0.x , UV0.y + offset.y));
	vec4 NE = texture2D(gHighlight, vec2(UV0.x + offset.x, UV0.y + offset.y));
	vec4 NO = texture2D(gHighlight, vec2(UV0.x - offset.x, UV0.y + offset.y));
	vec4 S = texture2D(gHighlight, vec2(UV0.x , UV0.y - offset.y));
	vec4 SE = texture2D(gHighlight, vec2(UV0.x + offset.x , UV0.y - offset));
	vec4 SO = texture2D(gHighlight, vec2(UV0.x - offset.x , UV0.y - offset.y));
	vec4 E = texture2D(gHighlight, vec2(UV0.x + offset.x, UV0.y));
	vec4 O = texture2D(gHighlight, vec2(UV0.x - offset.x, UV0.y));
	vec4 C = texture2D(gHighlight, UV0);
	
	hlight = C;

	color = E - C;
	color = color + O - C;
	color = color + N - C;
	color = color + NE - C;
	color = color + NO - C;
	color = color + S - C;
	color = color + SE - C;
	color = color + SO - C;
	color.a = 1;
	
	color.r = max(color.r, 0.0f);
	color.g = max(color.g, 0.0f);
	color.b = max(color.b, 0.0f);
	color.a = max(color.a, 0.0f);
	
	float lC = length(color.rgb);
	color = (1 - lC) * texture2D(gColor, UV0) + lC * color;
	
	/* tex_offset = 1.0 / textureSize(gColor, 0); // gets size of single texel
    vec3 result = texture(gColor, UV0).rgb * weight[0]; // current fragment's contribution
    if(horizontal)
    {
        for(int i = 1; i < 5; ++i)
        {
            result += texture(gColor, UV0 + vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
            result += texture(gColor, UV0 - vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
        }
    }
    else
    {
        for(int i = 1; i < 5; ++i)
        {
            result += texture(gColor, UV0 + vec2(0.0, tex_offset.y * i)).rgb * weight[i];
            result += texture(gColor, UV0 - vec2(0.0, tex_offset.y * i)).rgb * weight[i];
        }
    }
    color = vec4(result, 1.0);
	*/
}
