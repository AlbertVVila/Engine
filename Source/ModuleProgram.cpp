#include "ModuleProgram.h"
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
	//Currently we have 2 programs, one for textures and a default one for axis
	defaultProgram = CreateProgram("Default");
	textureProgram = CreateProgram("Texture");
	skyboxProgram = CreateProgram("Skybox");
	return true;
}


unsigned int ModuleProgram::CreateProgram(const char * name)
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
	return program;
}

unsigned int ModuleProgram::CreateVertexShader(const char *name)
{
	assert(name != NULL);
	std::string file(name);
	file += ".vs";
	const char * vertexShaderSource = ReadShader(file.c_str());
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);
	delete[] vertexShaderSource; //dealloacte memory allocated in readshader

	ShaderLog(vertexShader, "VERTEX");
	return vertexShader;
}

unsigned int ModuleProgram::CreateFragmentShader(const char *name)
{
	assert(name != NULL);
	std::string file(name);
	file += ".fs";
	const char * fragmentShaderSource = ReadShader(file.c_str());
	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	delete [] fragmentShaderSource; //dealloacte memory allocated in readshader

	ShaderLog(fragmentShader, "FRAGMENT");
	return fragmentShader;
}

const char* ModuleProgram::ReadShader(const char * file_name) const
{
	assert(file_name != NULL);
	char* data = nullptr;
	FILE* file = nullptr;
	errno_t err = fopen_s(&file, file_name, "rb");
	if (err != 0)
	{
		LOG("Error, the file %s was not opened\n", file_name);
	}
	if (file)
	{
		fseek(file, 0, SEEK_END);
		int size = ftell(file);
		rewind(file);
		data = new char[size+1];
		if (data == NULL)
		{ 
			LOG("Memory error with shader reading\n");
		}
		size_t result = fread(data, 1, size, file);
		data[size] = 0;
		if (result != size)
		{
			LOG("Shader reading error\n");
		}
		fclose(file);
	}
	return data;
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
	glDeleteProgram(textureProgram);
	glDeleteProgram(defaultProgram);
	return true;
}

