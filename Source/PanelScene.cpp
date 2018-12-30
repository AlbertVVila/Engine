#include "Application.h"

#include "ModuleInput.h"
#include "ModuleRender.h"
#include "ModuleCamera.h"
#include "ModuleScene.h"

#include "GameObject.h"
#include "ComponentCamera.h"

#include "PanelScene.h"

#include "imgui.h"
#include "ImGuizmo.h"
#include "Geometry/LineSegment.h"
#include "GL/glew.h"
PanelScene::PanelScene()
{
}


PanelScene::~PanelScene()
{
	if (FBO != 0)
	{
		glDeleteFramebuffers(1, &FBO);
	}
	if (RBO != 0)
	{
		glDeleteRenderbuffers(1, &RBO);
	}
	if (texture != 0)
	{
		glDeleteTextures(1, &texture);
	}
	if (MSAAFBO != 0)
	{
		glDeleteFramebuffers(1, &MSAAFBO);
	}
	if (MSAADEPTH != 0)
	{
		glDeleteRenderbuffers(1, &MSAADEPTH);
	}
	if (MSAACOLOR != 0)
	{
		glDeleteRenderbuffers(1, &MSAACOLOR);
	}
}

void PanelScene::Draw()
{
	ImGui::Begin("Scene", &enabled, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

	ImVec2 size = ImGui::GetWindowSize();

	App->camera->editorcamera->SetAspect(size.x / size.y);
	CreateFrameBuffer(size.x, size.y);
	current_width = size.x;
	current_height = size.y;

	glBindFramebuffer(GL_FRAMEBUFFER, MSAAFBO);
	App->renderer->Draw(*App->camera->editorcamera, current_width, current_height);
	if (App->renderer->msaa) //TODO apply draws and msaa to each camera in scene ->use viewport class o
	{
		glBindFramebuffer(GL_READ_FRAMEBUFFER, MSAAFBO);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, FBO);
		glBlitFramebuffer(0, 0, current_width, current_height,
			0, 0, current_width, current_height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
	}


	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	ImGui::SetCursorPos({ 0,0 });

	
	ImGui::Image((ImTextureID)texture, size, ImVec2(0, 1), ImVec2(1, 0));

	focus = ImGui::IsWindowFocused();

	DrawImGuizmo();
	Pick();

	ImGui::End();
}

void PanelScene::CreateFrameBuffer(int width, int height)
{
	//CleanUp(); //Delete old FBO,RBO and texture
	if (width != current_width || height != current_height)
	{
		if (FBO == 0)
		{
			glGenFramebuffers(1, &FBO);
		}
		glBindFramebuffer(GL_FRAMEBUFFER, FBO);

		if (texture == 0)
		{
			glGenTextures(1, &texture);
		}
		
		glBindTexture(GL_TEXTURE_2D, texture);

		glTexImage2D(
			GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL
		);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glFramebufferTexture2D(
			GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0
		);

		glBindTexture(GL_TEXTURE_2D, 0);

		if (RBO == 0)
		{
			glGenRenderbuffers(1, &RBO);
		}
		
		glBindRenderbuffer(GL_RENDERBUFFER, RBO);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);

		glBindRenderbuffer(GL_RENDERBUFFER, 0);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			LOG("Framebuffer ERROR");

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		if (App->renderer->msaa)
		{
			CreateMSAABuffers(width, height);
		}
	}
}

void PanelScene::CreateMSAABuffers(int width, int height)
{
	if (MSAAFBO == 0)
	{
		glGenFramebuffers(1, &MSAAFBO);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, MSAAFBO);

	if (MSAADEPTH == 0)
	{
		glGenRenderbuffers(1, &MSAADEPTH);
	}
	glBindRenderbuffer(GL_RENDERBUFFER, MSAADEPTH);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, App->renderer->msaa_level, GL_DEPTH24_STENCIL8, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, MSAADEPTH);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	if (MSAACOLOR == 0)
	{
		glGenRenderbuffers(1, &MSAACOLOR);
	}
	glBindRenderbuffer(GL_RENDERBUFFER, MSAACOLOR);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, App->renderer->msaa_level, GL_RGBA, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, MSAACOLOR);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glDrawBuffer(GL_COLOR_ATTACHMENT0);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		LOG("Framebuffer ERROR");

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void PanelScene::DrawImGuizmo()
{
	ImVec2 pos = ImGui::GetWindowPos();
	ImGuizmo::SetRect(pos.x, pos.y, current_width, current_height);
	ImGuizmo::SetDrawlist();

	static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::TRANSLATE);

	ImGui::SetCursorPos({ 20,30 });


	if (ImGui::Button("Translate"))
	{
		mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
	}
	ImGui::SameLine();
	if (ImGui::Button("Rotate"))
	{
		mCurrentGizmoOperation = ImGuizmo::ROTATE;
	}
	ImGui::SameLine();
	if (ImGui::Button("Scale"))
	{
		mCurrentGizmoOperation = ImGuizmo::SCALE;
	}

	if (App->scene->selected != nullptr) //TODO: WORLD translation + rotation, scale? -> LOCAL do it with local !watch rotation
	{

		float4x4 model = App->scene->selected->GetGlobalTransform();
		float4x4 view = App->camera->editorcamera->GetViewMatrix();
		float4x4 proj = App->camera->editorcamera->GetProjectionMatrix();
		ImGuizmo::SetOrthographic(false);
		model.Transpose();
		//ImGuizmo::DrawCube((float*)&view, (float*)&proj, (float*)&model);
		ImGuizmo::Manipulate((float*)&view, (float*)&proj, mCurrentGizmoOperation, ImGuizmo::LOCAL, (float*)&model, NULL, NULL, NULL, NULL);
		model.Transpose();

		App->scene->selected->SetGlobalTransform(model);
	}
}

void PanelScene::Pick()
{
	if (App->input->GetMouseButtonDown(1) == KEY_DOWN && ImGui::IsWindowFocused() && ImGui::IsMouseHoveringWindow())
	{
		ImVec2 pos = ImGui::GetWindowPos();
		float2 mouse((float*)&App->input->GetMousePosition());
		float normalized_x = ((mouse.x - pos.x) / current_width) * 2 - 1; //0 to 1 -> -1 to 1
		float normalized_y = (1 - (mouse.y - pos.y) / current_height) * 2 - 1; //0 to 1 -> -1 to 1
		App->scene->Pick(normalized_x, normalized_y);
	}
}
