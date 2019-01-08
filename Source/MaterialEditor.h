#ifndef __MaterialEditor_h__
#define __MaterialEditor_h__
#define None "None Selected"

class Material;

class MaterialEditor
{
public:
	MaterialEditor();
	~MaterialEditor();

	void Draw();
	void TextureSelector(unsigned i, std::string &current_texture);
	void CleanUp();

public:
	bool open=false;
	bool isCreated = false;
	Material* material = nullptr;

	std::string current_shader = None;
	std::string current_diffuse = None;
	std::string current_specular = None;
	std::string current_occlusion = None;
	std::string current_emissive = None;

	std::vector<std::string> textureFiles;
	std::vector<std::string> shaders;
};

#endif __MaterialEditor_h__
