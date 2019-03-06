#include "ResourceShader.h"

#include "Globals.h"
#include "Application.h"
#include "ModuleFileSystem.h"

#include "JSON.h"
#include "GL/glew.h"
#include <assert.h>

ResourceShader::ResourceShader(unsigned uid) : Resource(uid, TYPE::SHADER)
{
}


ResourceShader::~ResourceShader()
{
}

bool ResourceShader::LoadInMemory()
{
	unsigned vertexShader = CreateVertexShader();
	unsigned fragmentShader = CreateFragmentShader();

	unsigned program = glCreateProgram();
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
	id = program;

	return success;
}

void ResourceShader::DeleteFromMemory()
{
	if (id != 0)
	{
		glDeleteProgram(id);
	}
}

unsigned ResourceShader::CreateVertexShader()
{
	std::string file(exportedFileName);
	file += ".vs";
	char * vertexShaderSource = nullptr;
	App->fsystem->Load((VERTEXSHADERS + file).c_str(), &vertexShaderSource);

	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);
	RELEASE_ARRAY(vertexShaderSource);

	ShaderLog(vertexShader, "VERTEX");
	return vertexShader;
}

unsigned ResourceShader::CreateFragmentShader()
{
	std::string file(exportedFileName);
	file += ".fs";
	char * fragmentShaderSource = nullptr;
	App->fsystem->Load((FRAGSHADERS + file).c_str(), &fragmentShaderSource);

	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	RELEASE_ARRAY(fragmentShaderSource);

	ShaderLog(fragmentShader, "FRAGMENT");
	return fragmentShader;
}

void ResourceShader::ShaderLog(unsigned int shader, char * type) const
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

void ResourceShader::Save(JSON_value &config) const
{
	Resource::Save(config);

	config.AddUint("ID", id);
}

void ResourceShader::Load(const JSON_value &config)
{
	Resource::Load(config);

	id = config.GetUint("ID");
}