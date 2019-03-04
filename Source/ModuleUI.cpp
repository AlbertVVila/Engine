#include "ModuleUI.h"

#include "Application.h"

#include "ModuleProgram.h"
#include "ModuleResourceManager.h"
#include "ModuleTextures.h"

#include "ComponentCamera.h"

#include "GL/glew.h"
#include "Math/float4x4.h"

ModuleUI::ModuleUI()
{
}

ModuleUI::~ModuleUI()
{
}

bool ModuleUI::Init(JSON* json)
{
	shader = App->program->GetProgram(shaderFile);

	texture = App->textures->GetTexture("Lenna");

	float vertices[] = {
			-0.5f, -0.5f, 0.0f,  // bottom left
			-0.5f,  0.5f, 0.0f,   // top left 
			 0.5f, -0.5f, 0.0f,  // bottom right
			 0.5f,  0.5f, 0.0f,  // top right

			0.0f, 0.0f,
			0.0f, 1.0f,
			1.0f, 0.0f,
			1.0f, 1.0f
	};

	unsigned int indices[] = {  
		0,2,1,
		1,2,3
	};

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	//Vertex
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	//UV position
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)(sizeof(float) * 3 * 4));

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	return true;
}

update_status ModuleUI::Update(float dt)
{
	return UPDATE_CONTINUE;
}

update_status ModuleUI::PostUpdate()
{
	return UPDATE_CONTINUE;
}

bool ModuleUI::CleanUp()
{
	if (shader != nullptr)
	{
		App->resManager->DeleteProgram(shader->file);
		shader = nullptr;
	}

	return true;
}

void ModuleUI::Draw(const ComponentCamera &camera)
{
	if (shader == nullptr) return;


	glUseProgram(shader->id);
	glBindVertexArray(VAO);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture->id);
	glUniform1i(glGetUniformLocation(shader->id, "texture0"), 0);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);
	glUseProgram(0);


}

