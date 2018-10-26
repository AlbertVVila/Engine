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

	void ProcessVertexShader();
	void ProcessFragmentShader();

	char* ReadShader(char * file_name);

	void ShaderLog(unsigned int shader, char* type) const;

	unsigned int vertexShader;
	unsigned int fragmentShader;
	unsigned int shaderProgram;
};

#endif /* __ModuleProgram_h__ */
