#ifndef __ModuleProgram_h__
#define __ModuleProgram_h__

#include "Module.h"
#include "MathGeoLib.h"

class ModuleProgram : public Module
{
public:
	ModuleProgram();
	~ModuleProgram();

	bool Init();
	bool CleanUp();

	void CompileShader(unsigned int shader);

	void ProcessShader();

	unsigned int vertexShader;
	unsigned int fragmentShader;
	unsigned int shaderProgram;
};

#endif /* __ModuleProgram_h__ */
