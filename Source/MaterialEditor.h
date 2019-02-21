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
	void ShaderSelector(std::string &current_shader);
	void TextureSelector(unsigned i, std::string &current_texture, int id);
	void SetCurrentTextures();	// Sets current textures strings with the corresponding texture file string
	void NewMaterial();
	bool Exists(const char* material);
	void CleanUp();

public:
	bool open = false;

	char newName[64] = "New Material";
	bool newMaterial = false;

	Material* material = nullptr;
	Material* previous = nullptr;

	std::string current_shader = None;
	std::string current_diffuse = None;
	std::string current_specular = None;
	std::string current_occlusion = None;
	std::string current_emissive = None;
	std::string current_normal = None;

	std::vector<std::string> textureFiles;
	std::vector<std::string> shaders;
};

#endif __MaterialEditor_h__
