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

	unsigned CreateProgram(const char *name);
	unsigned GetProgram(const char * name);
	unsigned CreateVertexShader(const char *name);
	unsigned CreateFragmentShader(const char *name);

	//const char* ReadShader(const char *filename) const;

	void ShaderLog(unsigned int shader, char* type) const;

	//TODO: Use container for programs
	unsigned defaultProgram = 0;
	//unsigned textureProgram = 0;
	//unsigned skyboxProgram = 0;
	//unsigned flatProgram = 0;
	//unsigned gouraudProgram = 0;
	//unsigned phongProgram = 0;

};

#endif /* __ModuleProgram_h__ */
