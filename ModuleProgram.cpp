#include "ModuleProgram.h"
#include "GL/glew.h"
#include <string>


ModuleProgram::ModuleProgram()
{
}


ModuleProgram::~ModuleProgram()
{
}

bool ModuleProgram::Init()
{
	//Currently we have this 2 progams, one for textures and the other one for axis 
	defaultProgram = CreateProgram("Default");
	textureProgram = CreateProgram("Texture");
	return true;
}


unsigned int ModuleProgram::CreateProgram(const char * name)
{
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
	std::string file(name);
	file += ".vs";
	char * vertexShaderSource = ReadShader(file.c_str());
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);

	ShaderLog(vertexShader, "VERTEX");
	return vertexShader;
}

unsigned int ModuleProgram::CreateFragmentShader(const char *name)
{
	std::string file(name);
	file += ".fs";
	char * fragmentShaderSource = ReadShader(file.c_str());
	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);

	ShaderLog(fragmentShader, "FRAGMENT");
	return fragmentShader;
}

char* ModuleProgram::ReadShader(const char * file_name) const
{
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
		data = (char*)malloc(size+1);
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
	//Delete program
	return true;
}

