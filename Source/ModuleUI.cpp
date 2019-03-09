#include "ModuleUI.h"

#include "Application.h"
#include "ModuleProgram.h"
#include "ModuleResourceManager.h"
#include "ModuleTextures.h"

#include "ComponentCamera.h"
#include "ComponentImage.h"

#include "GL/glew.h"

ModuleUI::ModuleUI()
{
}

ModuleUI::~ModuleUI()
{
}



bool ModuleUI::Init(JSON* json)
{
	shader = App->program->GetProgram(shaderFile);

	float quadVertices[] =
	{
			-0.5f, -0.5f, 0.0f, 0.0f, // bottom left
			-0.5f,  0.5f, 0.0f, 1.0f, // top left 
			 0.5f, -0.5f, 1.0f, 0.0f, // bottom right
			 0.5f,  0.5f, 1.0f, 1.0f  // top right
			
	};

	unsigned int quadIndices[] =
	{
		0,2,1,
		1,2,3
	};

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quadIndices), quadIndices, GL_STATIC_DRAW);

	//Vertex + UV position
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);

	glEnableVertexAttribArray(0);

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

	for (std::list<ComponentImage*>::iterator it = images.begin(); it != images.end(); ++it)
	{
		if ((*it)->texture != nullptr && (*it)->texture != 0 && (*it)->enabled)
		{
			RenderImage(*(*it));
		}
	}
}


void ModuleUI::RenderImage(const ComponentImage& componentImage)
{
	glUseProgram(shader->id);

	glUniform4f(glGetUniformLocation(shader->id, "textColor"), componentImage.color.x, componentImage.color.y, componentImage.color.z, componentImage.color.w);

	glBindVertexArray(VAO);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, componentImage.texture->id);
	glUniform1i(glGetUniformLocation(shader->id, "texture0"), 0);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);
	glUseProgram(0);
}
