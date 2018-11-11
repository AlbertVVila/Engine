#ifndef __ModuleProgram_h__
#define __ModuleProgram_h__

#include "Module.h"

class ModuleProgram : public Module
{
public:
	ModuleProgram();
	~ModuleProgram();

	bool Init();
	bool CleanUp();

	unsigned int CreateProgram(const char *name);
	unsigned int CreateVertexShader(const char *name);
	unsigned int CreateFragmentShader(const char *name);

	char* ReadShader(const char *filename) const;

	void ShaderLog(unsigned int shader, char* type) const;

	//TODO: Use container for programs
	unsigned int defaultProgram = 0;
	unsigned int textureProgram= 0;

};

#endif /* __ModuleProgram_h__ */
