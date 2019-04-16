#include "Application.h"

#include "ModuleRender.h"
#include "ModuleCamera.h"
#include "ModuleScene.h"
#include "ModuleProgram.h"
#include "ModuleEditor.h"
#include "ModuleWindow.h"
#include "ModuleDebugDraw.h"
#include "ModuleResourceManager.h"
#include "ModuleUI.h"
#include "ModuleFontLoader.h"
#include "ModuleParticles.h"
#include "ModuleTime.h"

#include "GameObject.h"
#include "ComponentCamera.h"
#include "ComponentRenderer.h"
#include "ComponentTransform.h"
#include "Skybox.h"
#include "Viewport.h"
#include "JSON.h"

#include "SDL.h"
#include "GL/glew.h"
#include "imgui.h"
#include "Math/MathFunc.h"
#include "Math/float4x4.h"
#include "Brofiler.h"
#include "debugdraw.h"

ModuleRender::ModuleRender()
{
}

// Destructor
ModuleRender::~ModuleRender()
{
	RELEASE(skybox);
	RELEASE(viewScene);
	RELEASE(viewGame);
}

// Called before render is available
bool ModuleRender::Init(JSON * config)
{
	LOG("Creating Renderer context");

	InitSDL();

	glewInit();
	InitOpenGL();

	SDL_GL_SetSwapInterval((int)vsync);
	skybox = new Skybox();
	viewScene = new Viewport("Scene");
	viewGame = new Viewport("Game");

	JSON_value* renderer = config->GetValue("renderer");
	if (renderer == nullptr) return true;

	msaa = renderer->GetInt("msaa");
	msaa_level = renderer->GetInt("msaa_level");
	picker_debug = renderer->GetInt("picker_debug");
	light_debug = renderer->GetInt("light_debug");
	grid_debug = renderer->GetInt("grid_debug");
	depthTest = renderer->GetInt("depthTest");
	wireframe = renderer->GetInt("wireframe");
	vsync = renderer->GetInt("vsync");
	useMainCameraFrustum = renderer->GetInt("frustumMainCamera");
	skybox->enabled = renderer->GetInt("skybox");
	current_scale = renderer->GetInt("current_scale");
	switch (current_scale)
	{
		case 1:
			item_current = 0;
			break;
		case 10:
			item_current = 1;
			break;
		case 100:
			item_current = 2;
			break;
	}

	return true;
}

bool ModuleRender::Start()
{
	GenBlockUniforms();
	return true;
}


update_status ModuleRender::PreUpdate()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	return UPDATE_CONTINUE;
}

// Called every draw update
update_status ModuleRender::Update(float dt)
{ 
	return UPDATE_CONTINUE;
}


update_status ModuleRender::PostUpdate()
{
	BROFILER_CATEGORY("Render PostUpdate", Profiler::Color::Black);
	ComputeShadows();

	viewScene->Draw(App->camera->editorcamera, true);
	viewGame->Draw(App->scene->maincamera);
	App->editor->RenderGUI();

	SDL_GL_SwapWindow(App->window->window);

	return UPDATE_CONTINUE;
}

void ModuleRender::SaveConfig(JSON * config)
{
	JSON_value* renderer = config->CreateValue();

	renderer->AddInt("msaa", msaa);
	renderer->AddInt("msaa_level", msaa_level);
	renderer->AddInt("picker_debug", picker_debug);
	renderer->AddInt("light_debug", light_debug);
	renderer->AddInt("quadtree_debug", light_debug);
	renderer->AddInt("grid_debug", grid_debug);
	renderer->AddInt("current_scale", current_scale);
	renderer->AddInt("depthTest", depthTest);
	renderer->AddInt("wireframe", wireframe);
	renderer->AddInt("vsync", vsync);
	renderer->AddInt("frustumMainCamera", useMainCameraFrustum);
	renderer->AddInt("skybox", skybox->enabled);

	config->AddValue("renderer", *renderer);
}

void ModuleRender::Draw(const ComponentCamera &cam, int width, int height, bool isEditor) const
{
	BROFILER_CATEGORY("Render_Draw()", Profiler::Color::AliceBlue);
	glViewport(0, 0, width, height);
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

	SetProjectionUniform(cam);
	SetViewUniform(cam);
	skybox->Draw(*cam.frustum);
	if (isEditor)
	{
		DrawGizmos(cam);
	}
	App->scene->Draw(*cam.frustum, isEditor);
	App->particles->Render(App->time->gameDeltaTime, &cam);
	App->ui->Draw(width, height);
	
}

bool ModuleRender::IsSceneViewFocused() const
{
	return viewScene->focus;
}

bool ModuleRender::IsSceneHovered() const
{
	return viewScene->hover;
}
// Called before quitting
bool ModuleRender::CleanUp()
{

	LOG("Destroying renderer");
	if (UBO != 0)
	{
		glDeleteBuffers(1, &UBO);
	}
	return true;
}

void ModuleRender::OnResize()
{
    glViewport(0, 0, App->window->width, App->window->height);
	App->camera->editorcamera->SetAspect((float)App->window->width / (float)App->window->height);
}

void ModuleRender::DrawGizmos(const ComponentCamera &camera) const
{
	BROFILER_CATEGORY("Render_DrawGizmos()", Profiler::Color::AliceBlue);
	unsigned shader = App->program->defaultShader->id;
	glUseProgram(shader);

	if (picker_debug)
	{
		for (const auto & line : App->scene->debuglines)
		{
			dd::line(line.a, line.b, math::float3(0.0f, 0.0f, 1.0f));
		}
	}

	math::float4x4 model = math::float4x4::identity;
	glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_TRUE, &model[0][0]);

	if (grid_debug)
	{
		dd::xzSquareGrid(-500.0f * current_scale, 500.0f * current_scale, 0.0f, 1.0f * current_scale, math::float3(0.65f, 0.65f, 0.65f));
	}
	
	dd::axisTriad(math::float4x4::identity, 0.5f * current_scale, 5.0f * current_scale, 0, true);

	if (App->scene->maincamera != nullptr && App->renderer->useMainCameraFrustum)
	{
		dd::frustum((App->scene->maincamera->frustum->ProjectionMatrix() * App->scene->maincamera->frustum->ViewMatrix()).Inverted(), dd::colors::Red);
	}

	App->debug->Draw(camera, viewScene->current_width, viewScene->current_height);
	glUseProgram(0);
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
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_MULTISAMPLE);

	glClearDepth(1.0f);
	glClearColor(0.3f, 0.3f, 0.3f, 1.f);

	glViewport(0, 0, App->window->width, App->window->height);
}

void ModuleRender::ComputeShadows()
{
	if (directionalLight)
	{
		math::Quat lookAtLight = math::Quat::LookAt(math::float3::unitZ, directionalLight->gameobject->transform->front, math::float3::unitY, directionalLight->gameobject->transform->up);
		math::AABB lightAABB;
		lightAABB.SetNegativeInfinity();
		for (GameObject* go : App->scene->dynamicFilteredGOs) //TODO: get volumetric gos even if outside the frustum
		{
			ComponentRenderer* cr = (ComponentRenderer*)go->GetComponent(ComponentType::Renderer);
			if (cr && cr->castShadows)
			{
				lightAABB.Enclose(go->bbox);
			}
		}

		for (GameObject* go : App->scene->staticFilteredGOs)
		{
			ComponentRenderer* cr = (ComponentRenderer*)go->GetComponent(ComponentType::Renderer);
			if (cr && cr->castShadows)
			{
				lightAABB.Enclose(go->bbox);
			}
		}


		math::float3 points[8];

		math::float4x4 lightMat = math::Quat::LookAt(math::float3::unitZ, directionalLight->gameobject->transform->front, math::float3::unitY, directionalLight->gameobject->transform->up).Inverted().ToFloat3x3();

		lightAABB.GetCornerPoints(points);

		math::float3 minP, maxP;
		minP = points[0];
		maxP = points[0];
		for (unsigned i = 0u; i < 8u; ++i)
		{
			points[i] = lightMat.TransformPos(points[i]);
			minP.x = MIN(points[i].x, minP.x);
			minP.y = MIN(points[i].y, minP.y);
			minP.z = MIN(points[i].z, minP.z);

			maxP.x = MAX(points[i].x, maxP.x);
			maxP.y = MAX(points[i].y, maxP.y);
			maxP.z = MAX(points[i].z, maxP.z);
		}

		float widthP = maxP.x - minP.x;
		float halfWidthP = widthP * .5f;
		float heightP = maxP.y - minP.y;
		float halfHeightP = heightP * .5f;
		float lengthP = (maxP.z - minP.z) * .5f;

		math::float3 lightPos = lightMat.Inverted().TransformPos(math::float3((maxP.x + minP.x) * .5f, (maxP.y + minP.y) * .5f, maxP.z));

		if (shadowDebug) // draw shadows volume
		{
			dd::sphere(lightPos, dd::colors::YellowGreen, current_scale);

			math::float3 lFront = directionalLight->gameobject->transform->front;
			math::float3 lRight = directionalLight->gameobject->transform->right;
			math::float3 lUp = directionalLight->gameobject->transform->up;

			dd::line(lightPos, lightPos - lFront * lengthP, dd::colors::YellowGreen);
			dd::line(lightPos, lightPos - lRight * halfWidthP, dd::colors::Red);
			dd::line(lightPos, lightPos + lRight * halfWidthP, dd::colors::Red);
			dd::line(lightPos, lightPos - lUp * halfHeightP, dd::colors::Green);
			dd::line(lightPos, lightPos + lUp * halfHeightP, dd::colors::Green);

			math::float3 nearCorners[4] = {
						lightPos + lUp * halfHeightP + lRight * halfWidthP,
						lightPos + lUp * halfHeightP - lRight * halfWidthP,
						lightPos - lUp * halfHeightP - lRight * halfWidthP,
						lightPos - lUp * halfHeightP + lRight * halfWidthP
			};

			math::float3 farCorners[4] = {
						lightPos - lFront * lengthP + lUp * halfHeightP + lRight * halfWidthP,
						lightPos - lFront * lengthP + lUp * halfHeightP - lRight * halfWidthP,
						lightPos - lFront * lengthP - lUp * halfHeightP - lRight * halfWidthP,
						lightPos - lFront * lengthP - lUp * halfHeightP + lRight * halfWidthP
			};

			for (unsigned i = 0u; i < 4u; ++i)
			{
				dd::line(nearCorners[i], farCorners[i], dd::colors::YellowGreen);
				if (i < 3u)
				{
					dd::line(nearCorners[i], nearCorners[i + 1], dd::colors::YellowGreen);
					dd::line(farCorners[i], farCorners[i + 1], dd::colors::YellowGreen);
				}
				else
				{
					dd::line(nearCorners[i], nearCorners[0], dd::colors::YellowGreen);
					dd::line(farCorners[i], farCorners[0], dd::colors::YellowGreen);
				}
			}
		}
	}
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
	if (ImGui::Checkbox("Vsync", &vsync))
	{
		SDL_GL_SetSwapInterval((int)vsync);
	}
	ImGui::Checkbox("Game Frustum", &useMainCameraFrustum);
	ImGui::Checkbox("Skybox", &skybox->enabled);
	ImGui::Checkbox("MSAA", &msaa);
	if (msaa)
	{
		int previous_msaa_level = msaa_level;
		ImGui::RadioButton("x2", &msaa_level, 2); ImGui::SameLine();
		ImGui::RadioButton("x4", &msaa_level, 4); ImGui::SameLine();
		ImGui::RadioButton("x8", &msaa_level, 8);
		if (previous_msaa_level != msaa_level)
		{
			msaa_lvl_changed = true;
		}
	}
	ImGui::Checkbox("Picker Debug", &picker_debug);
	ImGui::Checkbox("Light Debug", &light_debug);
	ImGui::Checkbox("Shadow volume Debug", &shadowDebug);
	ImGui::Checkbox("Dynamic AABBTree Debug", &aabbTreeDebug);
	ImGui::Checkbox("Static KDTree Debug", &kDTreeDebug);
	ImGui::Checkbox("Grid Debug", &grid_debug);

	const char* scales[] = {"1", "10", "100"};
	ImGui::Combo("Scale", &item_current, scales, 3);
	unsigned new_scale = atoi(scales[item_current]);
	if (new_scale != current_scale)
	{
		current_scale = new_scale;
		std::vector<Component*> cameras = App->scene->root->GetComponentsInChildren(ComponentType::Camera);
		for (auto &cam : cameras)
		{
			((ComponentCamera*)cam)->frustum->nearPlaneDistance = ZNEARDIST * current_scale;
			((ComponentCamera*)cam)->frustum->farPlaneDistance = ZFARDIST * current_scale;
		}
		App->camera->editorcamera->frustum->nearPlaneDistance = ZNEARDIST * current_scale;
		App->camera->editorcamera->frustum->farPlaneDistance = ZFARDIST * current_scale;
	}
}

void ModuleRender::GenBlockUniforms()
{
	glGenBuffers(1, &UBO); //Block uniform creation
	glBindBuffer(GL_UNIFORM_BUFFER, UBO);
	//TODOALF: this should be nullptr instead unll
	glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(float4x4), NULL, GL_STATIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glBindBufferRange(GL_UNIFORM_BUFFER, 0, UBO, 0, 2 * sizeof(float4x4));
}

void ModuleRender::AddBlockUniforms(const Shader &shader) const
{ 
	unsigned int uniformBlockIndex = glGetUniformBlockIndex(shader.id, "Matrices");
	glUniformBlockBinding(shader.id, uniformBlockIndex, 0);
}

void ModuleRender::SetViewUniform(const ComponentCamera &camera) const
{
	glBindBuffer(GL_UNIFORM_BUFFER, UBO);
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(float4x4), sizeof(float4x4), &camera.GetViewMatrix()[0][0]);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void ModuleRender::SetProjectionUniform(const ComponentCamera &camera) const
{
	glBindBuffer(GL_UNIFORM_BUFFER, UBO);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(float4x4), &camera.GetProjectionMatrix()[0][0]);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}
