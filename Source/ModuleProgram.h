#ifndef __ModuleProgram_h__
#define __ModuleProgram_h__

#include "Module.h"
#include <string>
#include <map>
#define DEFAULTPROGRAM "Default"
#define PBR_VARIATIONS 3
#define SHADOW_VARIATIONS 2
#define POSTPRO_VARIATIONS 4

struct Shader
{
	std::map<unsigned, unsigned> id;
	unsigned variation = 0u;

	std::string file;
	Shader(unsigned program, std::string file) : file(file) { id[0] = program; }
	~Shader();
};

class ModuleProgram : public Module
{
public:

	enum class PBR_Variations
	{
		SKINNED					= 1 << 0,
		SHADOWS_ENABLED			= 1 << 1
	};

	enum class Shadows_Variations
	{
		SKINNED = 1 << 0
	};

	enum class Postprocess_Variations
	{
		BLOOM_X5				= 1 << 0,
		BLOOM_X10				= 1 << 1,
		BLOOM_X15				= 1 << 2,
	};

	const char* PBRDefines[PBR_VARIATIONS] = {
		"#define SKINNED\n",		//Skinned
		"#define SHADOWS_ENABLED\n"
	};

	const char* ShadowDefines[SHADOW_VARIATIONS] = {
		"#define SKINNED\n",		//Skinned
	};

	const char* PostProcessDefines[POSTPRO_VARIATIONS] = {
		"#define BLOOM_X5",
		"#define BLOOM_X10",
		"#define BLOOM_X15"
	};

	ModuleProgram();
	~ModuleProgram();

	bool Init(JSON * config) override;
	bool CleanUp() override;

	Shader* CreateProgram(const char* name);
	Shader* GetProgram(const char* name); //Creates new one if it doesn't exist

private:
	void ShaderLog(unsigned int shader, char* type) const;
	unsigned CreateVertexShader(const char* name);
	unsigned CreateFragmentShader(const char* name);
	Shader* CreateVariations(const char* name, const char** defines, unsigned variations);

public:
	Shader * defaultShader = nullptr; //Deallocated in resourcemanager
};
#endif /* __ModuleProgram_h__ */
