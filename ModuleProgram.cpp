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
	ProcessVertexShader();
	ProcessFragmentShader();

	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	int  success;
	char infoLog[512];
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		LOG("ERROR::PROGRAM::CREATION_FAILED\n");
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	return true;
}

bool ModuleProgram::CleanUp()
{
	return true;
}



void ModuleProgram::ProcessVertexShader()
{
	char * vertexShaderSource = ReadShader("Default.vs");
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);

	ShaderLog(vertexShader, "VERTEX");
}

void ModuleProgram::ProcessFragmentShader()
{
	char * fragmentShaderSource = ReadShader("Default.fs");
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);

	ShaderLog(fragmentShader, "FRAGMENT");
}

char* ModuleProgram::ReadShader(char * file_name)
{
	char* data = nullptr;
	FILE* file = nullptr;
	errno_t err = fopen_s(&file, file_name, "rb");
	if (err != 0)
	{
		LOG("Error, the file %s was not opened opened\n", file_name);
	}
	if (file)
	{
		fseek(file, 0, SEEK_END);
		int size = ftell(file);
		rewind(file);
		data = (char*)malloc(size);
		if (data == NULL)
		{ 
			LOG("Memory error with shader reading\n");
		}
		size_t result = fread(data, 1, size, file);
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
