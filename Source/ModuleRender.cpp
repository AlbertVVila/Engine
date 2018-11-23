#include "Application.h"
#include "ModuleRender.h"
#include "ModuleCamera.h"
#include "ComponentCamera.h"
#include "ModuleScene.h"
#include "ModuleProgram.h"
#include "ModuleEditor.h"
#include "ModuleWindow.h"
#include "SDL.h"
#include "GL/glew.h"
#include "imgui.h"
#include "Math/MathFunc.h"

ModuleRender::ModuleRender()
{
}

// Destructor
ModuleRender::~ModuleRender()
{
}

// Called before render is available
bool ModuleRender::Init()
{
	LOG("Creating Renderer context");

	InitSDL();

	glewInit();
	InitOpenGL();

	SDL_GL_SetSwapInterval((int)vsync);

	return true;
}

bool ModuleRender::Start()
{
	InitFrustum();
	CreateFrameBuffer(); //TODO: move frustum and view to camera
	CreateBlockUniforms();
	return true;
}


update_status ModuleRender::PreUpdate()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	return UPDATE_CONTINUE;
}

// Called every draw update
update_status ModuleRender::Update()
{
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glClearColor(0.3f, 0.3f, 0.3f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (wireframe)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	ProjectionMatrix();
	ViewMatrix();

	App->scene->Draw();

	DrawGizmos();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return UPDATE_CONTINUE;
}

update_status ModuleRender::PostUpdate()
{
	App->editor->RenderGUI();
	SDL_GL_SwapWindow(App->window->window);

	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleRender::CleanUp()
{
	LOG("Destroying renderer");
	glDeleteFramebuffers(1, &FBO);
	glDeleteRenderbuffers(1, &RBO);
	glDeleteBuffers(1, &UBO);
	return true;
}

void ModuleRender::OnResize()
{
    glViewport(0, 0, App->window->width, App->window->height);
	frustum.horizontalFov = 2.f * atanf(tanf(frustum.verticalFov * 0.5f) * ((float)App->window->width / (float)App->window->height));
	CreateFrameBuffer(); //We recreate framebuffer with new window size
}


void ModuleRender::DrawGizmos() const
{

	glUseProgram(App->program->defaultProgram);
	math::float4x4 model = math::float4x4::identity;
	glUniformMatrix4fv(glGetUniformLocation(App->program->defaultProgram,
		"model"), 1, GL_TRUE, &model[0][0]);

	DrawLines();
	DrawAxis();

	glUseProgram(0);
}


void ModuleRender::DrawLines() const
{
	float white[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glUniform4fv(glGetUniformLocation(App->program->defaultProgram,
		"Vcolor"), 1, white);

	glLineWidth(1.0f);
	float d = 200.0f;
	glBegin(GL_LINES);

	for (float i = -d; i <= d; i += 1.0f)
	{
		glVertex3f(i, 0.0f, -d);
		glVertex3f(i, 0.0f, d);
		glVertex3f(-d, 0.0f, i);
		glVertex3f(d, 0.0f, i);
	}
	glEnd();
}

void ModuleRender::DrawAxis() const
{
	glLineWidth(2.0f);

	float red[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
	glUniform4fv(glGetUniformLocation(App->program->defaultProgram,
		"Vcolor"), 1, red);

	glBegin(GL_LINES);
	// red X

	glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(1.0f, 0.0f, 0.0f);
	glVertex3f(1.0f, 0.1f, 0.0f); glVertex3f(1.1f, -0.1f, 0.0f);
	glVertex3f(1.1f, 0.1f, 0.0f); glVertex3f(1.0f, -0.1f, 0.0f);
	glEnd();

	float green[4] = { 0.0f, 1.0f, 0.0f, 1.0f };
	glUniform4fv(glGetUniformLocation(App->program->defaultProgram,
		"Vcolor"), 1, green);

	glBegin(GL_LINES);
	// green Y
	glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(0.0f, 1.0f, 0.0f);
	glVertex3f(-0.05f, 1.25f, 0.0f); glVertex3f(0.0f, 1.15f, 0.0f);
	glVertex3f(0.05f, 1.25f, 0.0f); glVertex3f(0.0f, 1.15f, 0.0f);
	glVertex3f(0.0f, 1.15f, 0.0f); glVertex3f(0.0f, 1.05f, 0.0f);
	glEnd();


	float blue[4] = { 0.0f, 0.0f, 1.0f, 1.0f };
	glUniform4fv(glGetUniformLocation(App->program->defaultProgram,
		"Vcolor"), 1, blue);

	glBegin(GL_LINES);
	// blue Z
	glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(0.0f, 0.0f, 1.0f);
	glVertex3f(-0.05f, 0.1f, 1.05f); glVertex3f(0.05f, 0.1f, 1.05f);
	glVertex3f(0.05f, 0.1f, 1.05f); glVertex3f(-0.05f, -0.1f, 1.05f);
	glVertex3f(-0.05f, -0.1f, 1.05f); glVertex3f(0.05f, -0.1f, 1.05f);
	glEnd();

	glLineWidth(1.0f);
}

void ModuleRender::InitFrustum()
{
	frustum.type = FrustumType::PerspectiveFrustum;
	frustum.pos = float3::zero;
	frustum.front = -float3::unitZ;
	frustum.up = float3::unitY;
	frustum.nearPlaneDistance = 0.1f;
	frustum.farPlaneDistance = 1000.0f;
	frustum.verticalFov = math::pi / 2.0f;
	frustum.horizontalFov = 2.f * atanf(tanf(frustum.verticalFov * 0.5f) * ((float)App->window->width / (float)App->window->height));

}

void ModuleRender::InitSDL()
{
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

	context = SDL_GL_CreateContext(App->window->window);
	SDL_GetWindowSize(App->window->window, &App->window->width, &App->window->height);
}

void ModuleRender::InitOpenGL() const
{
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glEnable(GL_TEXTURE_2D);

	glClearDepth(1.0f);
	glClearColor(0.3f, 0.3f, 0.3f, 1.f);


	glViewport(0, 0, App->window->width, App->window->height);
}

void ModuleRender::CreateFrameBuffer()
{
	glDeleteFramebuffers(1, &FBO);
	glDeleteRenderbuffers(1, &RBO);

	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	glGenTextures(1, &renderTexture);
	glBindTexture(GL_TEXTURE_2D, renderTexture);

	glTexImage2D(
		GL_TEXTURE_2D, 0, GL_RGB, App->window->width, App->window->height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL
	);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glFramebufferTexture2D(
		GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderTexture, 0
	);

	glBindTexture(GL_TEXTURE_2D, 0);

	glGenRenderbuffers(1, &RBO);
	glBindRenderbuffer(GL_RENDERBUFFER, RBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, App->window->width, App->window->height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);

	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		LOG("Framebuffer ERROR");

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ModuleRender::CreateBlockUniforms()
{
	unsigned int uniformBlockIndexDefault = glGetUniformBlockIndex(App->program->defaultProgram, "Matrices");
	unsigned int uniformBlockIndexTexture = glGetUniformBlockIndex(App->program->textureProgram, "Matrices");

	glUniformBlockBinding(App->program->defaultProgram, uniformBlockIndexDefault, 0);
	glUniformBlockBinding(App->program->textureProgram, uniformBlockIndexTexture, 0);

	glGenBuffers(1, &UBO);
	glBindBuffer(GL_UNIFORM_BUFFER, UBO);
	glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(float4x4), NULL, GL_STATIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glBindBufferRange(GL_UNIFORM_BUFFER, 0, UBO, 0, 2 * sizeof(float4x4));
}

void ModuleRender::DrawGUI()
{
	if (ImGui::Checkbox("Depth Test", &depthTest))
	{
		if (App->renderer->depthTest)
		{
			glEnable(GL_DEPTH_TEST);
		}
		else
		{
			glDisable(GL_DEPTH_TEST);
		}
	}

	ImGui::Checkbox("Wireframe", &wireframe);
	//ImGui::Checkbox("Show Model Bounding Boxes", &boundingBox); //TODO:BoundingBOx

	float degFov = math::RadToDeg(frustum.verticalFov);
	if (ImGui::SliderFloat("FOV", &degFov, 40, 120))
	{
		frustum.verticalFov = math::DegToRad(degFov);
		frustum.horizontalFov = 2.f * atanf(tanf(frustum.verticalFov*0.5f)*App->window->width/ App->window->height);
	}
	ImGui::InputFloat("Znear", &frustum.nearPlaneDistance, 1, 10);
	ImGui::InputFloat("Zfar", &frustum.farPlaneDistance, 1, 10);
	if (ImGui::Checkbox("Vsync", &vsync))
	{
		SDL_GL_SetSwapInterval((int)vsync);
	}
}

void ModuleRender::ViewMatrix() const
{
	math::float4x4 view = LookAt(App->camera->editorcamera->cameraPos, App->camera->editorcamera->cameraPos
		+ App->camera->editorcamera->cameraFront, float3::unitY);
	view.Transpose();
	glBindBuffer(GL_UNIFORM_BUFFER, UBO);
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(float4x4), sizeof(float4x4), &view[0][0]);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void ModuleRender::ProjectionMatrix() const
{
	float4x4 projection = frustum.ProjectionMatrix();
	projection.Transpose();
	glBindBuffer(GL_UNIFORM_BUFFER, UBO);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(float4x4), &projection[0][0]);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

math::float4x4 ModuleRender::LookAt(math::float3 OBS, math::float3 VRP, math::float3 up) const
{
	math::float3 forward(VRP - OBS); forward.Normalize(); //deprecated with camerafront pos and up
	math::float3 side(forward.Cross(up)); side.Normalize();
	math::float3 u(side.Cross(forward));

	math::float4x4 matrix(math::float4x4::zero);
	matrix[0][0] = side.x; matrix[0][1] = side.y; matrix[0][2] = side.z;
	matrix[1][0] = u.x; matrix[1][1] = u.y; matrix[1][2] = u.z;
	matrix[2][0] = -forward.x; matrix[2][1] = -forward.y; matrix[2][2] = -forward.z;
	matrix[0][3] = -side.Dot(OBS); matrix[1][3] = -u.Dot(OBS); matrix[2][3] = forward.Dot(OBS);
	matrix[3][3] = 1;

	return matrix;
}
