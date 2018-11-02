#include "Application.h"
#include "ModuleProgram.h"
#include "ModuleRenderExercise.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "ModuleModelLoader.h"
#include "ModuleRender.h"
#include "ModuleTextures.h"
#include "ModuleTime.h"
#include "GL/glew.h"
#include "IL/ilut.h"
#include "SDL.h"
#include "ModuleEditor.h"

ModuleRenderExercise::ModuleRenderExercise()
{
}

ModuleRenderExercise::~ModuleRenderExercise()
{
}

bool ModuleRenderExercise::Init()
{
    return true;
}

update_status ModuleRenderExercise::Update()
{
	//ProcessInput();

	
	return UPDATE_CONTINUE;
}

void ModuleRenderExercise::CreateBuffers()
{
	//float vertex_buffer_data[] = { 
	//	-1.0f, -1.0f, 0.0f,
	//	1.0f, -1.0f, 0.0f,
	//	-1.0f, 1.0f, 0.0f,
	//	1.0f, -1.0f, 0.0f,
	//	1.0f, 1.0f, 0.0f,
	//  - 1.0f, 1.0f, 0.0f,
	//	
	//	0.0f, 0.0f,
	//	1.0f, 0.0f,
	//	0.0f, 1.0f,
	//	1.0f, 0.0f,
	//	1.0f, 1.0f,
	//	0.0f, 1.0f
	//
	//};

	//// Creació del Vertex Array Object (VAO) que usarem per pintar
	//glGenVertexArrays(1, &vao);
	//glBindVertexArray(vao);

	//// Creació del buffer amb les dades dels vèrtexs
	//glGenBuffers(1, &vbo);
	//glBindBuffer(GL_ARRAY_BUFFER, vbo);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_buffer_data), vertex_buffer_data, GL_STATIC_DRAW);
	////glBindBuffer(GL_ARRAY_BUFFER, 0);

	////Activem atribut que farem servir per vèrtex( el 0 en aquest cas)
	//glEnableVertexAttribArray(0);
	//glVertexAttribPointer(
	//	0,                  // attribute 0
	//	3,                  // number of componentes (3 floats)
	//	GL_FLOAT,           // data type
	//	GL_FALSE,           // should be normalized?
	//	0,                  // stride
	//	(void*)0            // array buffer offset
	//);
	//glEnableVertexAttribArray(1);
	//glVertexAttribPointer(
	//	1,                  // attribute 0
	//	2,                  // number of componentes (3 floats)
	//	GL_FLOAT,           // data type
	//	GL_FALSE,           // should be normalized?
	//	0,                  // stride
	//	(void*)(sizeof(float) * 3 * 6)           // array buffer offset
	//);

	//// Desactivem el VAO
	//glBindVertexArray(0);

	//glDisableVertexAttribArray(0);
	//glDisableVertexAttribArray(1);

	//// Desactivem el VBO
	//glBindBuffer(GL_ARRAY_BUFFER, 0);
}



bool ModuleRenderExercise::CleanUp()
{
	return true;
}




//void ModuleRenderExercise::ShowRenderExerciseDialog()
//{
//	ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar;
//
//	ImVec2 main_viewport_pos = ImGui::GetMainViewport()->Pos;
//	ImGui::SetNextWindowPos(ImVec2(main_viewport_pos.x + 250, main_viewport_pos.y + 20), ImGuiCond_FirstUseEver);
//	ImGui::SetNextWindowSize(ImVec2(550, 680), ImGuiCond_FirstUseEver);
//	if (!ImGui::Begin("Texture Editor", false, window_flags))
//	{
//		ImGui::End();
//		return;
//	}
//
//	ImGui::Text("IMAGES:");
//	static int e = 0;
//	ImGui::RadioButton("Lenna", &e, 0); ImGui::SameLine();
//	ImGui::RadioButton("Black Hole", &e, 1); ImGui::SameLine();
//	ImGui::RadioButton("Not Blitzcrank", &e, 2);
//
//	char* newImage;
//	switch (e)
//	{
//	case 0:
//		newImage = "Lenna.png";
//		break;
//	case 1:
//		newImage = "black_hole.jpg";
//		break;
//	case 2:
//		newImage = "not_blitzcrank.jpg";
//		break;
//	default:
//		break;
//	}
//	if (newImage != image)
//	{
//		texture0 = App->textures->Load(newImage);
//		image = newImage;
//	}
//	ImGui::Separator();
//	ImGui::Text("CURRENT IMAGE INFO:"); 
//
//	ImGui::Text("Width: %dpx",App->textures->width); 
//	ImGui::Text("Height: %dpx", App->textures->height);
//	ImGui::Text("Pixel depth: %d", App->textures->pixelDepth);
//	ImGui::Text("Format: %d", App->textures->format);
//
//	if (ImGui::TreeNode("Image Options"))
//	{
//		ImGui::Checkbox("Mipmap", &App->textures->mipmap);
//		ImGui::Checkbox("MAG filter", &App->textures->magfilter);
//		ImGui::Checkbox("MIN filter", &App->textures->minfilter);
//		ImGui::Checkbox("Anisotropic Filtering", &App->textures->anisotropic_filter);
//
//		ImGui::Text("WRAP MODE:");
//		ImGui::RadioButton("Clamp", &App->textures->wrap_mode, 0); ImGui::SameLine();
//		ImGui::RadioButton("Clamp to Border", &App->textures->wrap_mode, 1);
//		ImGui::RadioButton("Repeat", &App->textures->wrap_mode, 2); ImGui::SameLine();
//		ImGui::RadioButton("Mirrored Repeat", &App->textures->wrap_mode, 3);
//		ImGui::TreePop();
//	}
//	ImGui::End();
//}

