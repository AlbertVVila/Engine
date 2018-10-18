#include "ModuleProgram.h"
#include "GL/glew.h"



ModuleProgram::ModuleProgram()
{
}


ModuleProgram::~ModuleProgram()
{
}

bool ModuleProgram::Init()
{
	ProcessShader();
	//CompileVertexShader(vertexShader);
	//CompileFragmentShader(vertexShader);

	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	return true;
}

bool ModuleProgram::CleanUp()
{
	return true;
}

void ModuleProgram::CompileShader(unsigned int shader)
{
	//shader = glCreateShader(GL_VERTEX_SHADER);
	//glShaderSource(shader, 1, &shadersource, NULL);
	//glCompileShader(shader);
}

void ModuleProgram::ProcessShader()
{

}
