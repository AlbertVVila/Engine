#ifndef __ModuleFontLoader_H__
#define __ModuleFontLoader_H__

#include "Module.h"
#include "GL/glew.h"
#include "Math/float2.h"
#include "Math/float3.h"
#include <map>
#include <vector>

struct Shader;
class ModuleFontLoader :
	public Module
{
public:

	//font structures
	struct Character {
		GLuint		TextureID;  // ID handle of the glyph texture
		float2		Size;       // Size of glyph
		float2		Bearing;    // Offset from baseline to left/top of glyph
		GLuint		Advance;    // Offset to advance to next glyph
	};

	//functions
	ModuleFontLoader();
	~ModuleFontLoader();
	bool Init(JSON* json) override;
	void Draw();
	bool CleanUp() override;
	void LoadFonts(const char* newFont);

	//variables
	//std::map<const char*, std::map<GLchar, Character>> fonts;
	std::map<const char*, std::vector<Character>> fonts;
	const char* defaultFont = "Fonts/DroidSans.ttf";

private:
	//functions
	void RenderText(Shader &s, std::string text, GLfloat x, GLfloat y, GLfloat scale, float3 color, const char* font);
	void drawText();

	//variables
	Shader* shaderFonts = nullptr;
	const char* shaderFontsFile = "Fonts";
	unsigned VAOText = 0;
	unsigned VBOText = 0;
};

#endif