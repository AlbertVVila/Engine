#ifndef __ModuleProgram_h__
#define __ModuleProgram_h__

#include "Module.h"
#include <string>

struct Shader
{
	unsigned value;
	std::string file;
	Shader(unsigned program, std::string file) : value(program), file(file) {}
};

class ModuleProgram : public Module
{
public:
	ModuleProgram();
	~ModuleProgram();

	bool Init();
	bool CleanUp();

	Shader* CreateProgram(const char *name);
	Shader* GetProgram(const char * name); //creates if it doesn't exist

private:
	void ShaderLog(unsigned int shader, char* type) const;
	unsigned CreateVertexShader(const char *name);
	unsigned CreateFragmentShader(const char *name);

public:

	Shader * defaultShader = nullptr;

};

#endif /* __ModuleProgram_h__ */
