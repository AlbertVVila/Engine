#include "ModuleProgram.h"
#include "ModuleFileSystem.h"
#include "ModuleResourceManager.h"

#include "Application.h"
#include "GL/glew.h"
#include <string>
#include <assert.h>

ModuleProgram::ModuleProgram()
{
}


ModuleProgram::~ModuleProgram()
{
}

bool ModuleProgram::Init() 
{
	const char* default = "Default";
	defaultShader = CreateProgram(default);

	return true;
}



Shader* ModuleProgram::CreateProgram(const char * name) //TODO: Use shader struct or class for abstraction (see LearnOpengl)
{
	assert(name != NULL);
	unsigned int vertexShader = CreateVertexShader(name);
	unsigned int fragmentShader = CreateFragmentShader(name);

	unsigned int program = glCreateProgram();
	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);
	glLinkProgram(program);

	int  success;
	char infoLog[512];
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(program, 512, NULL, infoLog);
		LOG("ERROR::PROGRAM::CREATION_FAILED\n");
		LOG("ERROR: %s\n", infoLog);
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	Shader* shader = new Shader(program, name);
	App->resManager->AddProgram(shader);
	return shader;
}

Shader* ModuleProgram::GetProgram(const char * name)
{
	Shader* shader = App->resManager->GetProgram(name);
	if (shader != nullptr) return shader;

	return CreateProgram(name);
}

unsigned ModuleProgram::CreateVertexShader(const char *name)
{
	assert(name != NULL);
	std::string file(name);
	file += ".vs";
	char * vertexShaderSource = nullptr;
	App->fsystem->Load((VERTEXSHADERS + file).c_str(), &vertexShaderSource);

	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);
	delete[] vertexShaderSource; //dealloacte memory allocated in readshader

	ShaderLog(vertexShader, "VERTEX");
	return vertexShader;
}

unsigned ModuleProgram::CreateFragmentShader(const char *name)
{
	assert(name != NULL);
	std::string file(name);
	file += ".fs";
	char * fragmentShaderSource = nullptr;
	App->fsystem->Load((FRAGSHADERS+file).c_str(), &fragmentShaderSource);

	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	delete [] fragmentShaderSource; //dealloacte memory allocated in readshader

	ShaderLog(fragmentShader, "FRAGMENT");
	return fragmentShader;
}

void ModuleProgram::ShaderLog(unsigned int shader, char * type) const
{
	assert(type != NULL);
	int  success;
	char infoLog[512];
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

	if (!success)
	{
		glGetShaderInfoLog(shader, 512, NULL, infoLog);
		LOG("ERROR::SHADER::%s::COMPILATION_FAILED\n", type);
		LOG("ERROR: %s\n", infoLog);
	}
}

bool ModuleProgram::CleanUp()
{
	glDeleteProgram(defaultShader->value);
	RELEASE(defaultShader);
	return true;
}

