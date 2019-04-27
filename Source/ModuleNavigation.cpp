#include "Application.h"
#include "Globals.h"
#include "GameObject.h"
#include "MathGeoLib/include/Math/float3.h"

#include "DetourPoints.h"

#include "ModuleNavigation.h"
#include "ModuleScene.h"
#include "ModuleDebugDraw.h"

#include "Component.h"
#include "ComponentRenderer.h"
#include "ComponentTransform.h"

#include "ResourceMesh.h"

#include "Geometry/AABB.h"

#include "imgui.h"
#include "SDL_opengl.h"
#include "JSON.h"
//#include "debugdraw.h"


#include "Recast/Recast/Recast.h"
#include "Recast/Detour/DetourNavMesh.h"
#include "Recast/Detour/DetourNavMeshBuilder.h"
#include "Recast/Detour/DetourNavMeshQuery.h"
#include "Recast/DebugUtils/RecastDebugDraw.h"
#include "Recast/DebugUtils/DetourDebugDraw.h"
#include "Recast/DebugUtils/DebugDraw.h"
#include "debug_draw.hpp"


ModuleNavigation::ModuleNavigation()
{
	ctx = new rcContext();
	navQuery = dtAllocNavMeshQuery();
}


ModuleNavigation::~ModuleNavigation()
{
	RELEASE_ARRAY(verts);
	RELEASE_ARRAY(tris);
	RELEASE_ARRAY(normals);
}

bool ModuleNavigation::Init(JSON * config)
{
	JSON_value* nav = config->GetValue("navigation");
	if (nav == nullptr) return true;

	cellWidth = nav->GetFloat("Cellwidth");
	meshGenerated = nav->GetUint("Generated", false);

	return true;
}
void ModuleNavigation::SaveConfig(JSON * config)
{
	JSON_value* nav = config->CreateValue();

	nav->AddFloat("Cellwidth", cellWidth);
	nav->AddUint("Generated", meshGenerated);

	config->AddValue("navigation", *nav);
}

void ModuleNavigation::sceneLoaded(JSON * config)
{
	JSON_value* nav = config->GetValue("navigationScene");
	if (nav == nullptr) return;

	renderMesh = nav->GetUint("RenderNavMesh", false);
	const char* objectName = nav->GetString("NavigableObjectName");
	if (objectName != nullptr)
	{
		GameObject* objToRender = App->scene->FindGameObjectByName(objectName);
		if (objToRender != nullptr)
		{
			addNavigableMesh(objToRender);
			generateNavigability(renderMesh);
		}
	}
}
void ModuleNavigation::sceneSaved(JSON * config)
{
	JSON_value* navigation = config->CreateValue();
	navigation->AddString("NavigableObjectName", objectName);
	navigation->AddUint("RenderNavMesh", renderMesh);

	config->AddValue("navigationScene", *navigation);
}

void ModuleNavigation::cleanValuesPRE()
{
	nverts = 0; ntris = 0;
	RELEASE_ARRAY(verts);
	RELEASE_ARRAY(tris);
	RELEASE_ARRAY(normals);
	RELEASE_ARRAY(m_triareas);
	m_triareas = 0;
	rcFreeHeightField(heightField);
	heightField = 0;
	rcFreeCompactHeightfield(chf);
	chf = 0;
	rcFreeContourSet(cset);
	cset = 0;
	rcFreePolyMesh(pmesh);
	pmesh = 0;
	rcFreePolyMeshDetail(dmesh);
	dmesh = 0;
	dtFreeNavMesh(navMesh);
	navMesh = 0;
	//will need to free navquery, navmesh, crowd
}

void ModuleNavigation::cleanValuesPOST()
{
	meshboxes.clear();
	meshComponents.clear();
	transformComponents.clear();
}

void ModuleNavigation::DrawGUI()
{
	//menu inspired in recast interface
	ImGui::Text("Rasterization");
	ImGui::DragFloat("Cell width", &cellWidth, cellIncreaseSpeed, minCellSize, maxCellSize);
	ImGui::DragFloat("Cell height", &cellHeight, cellIncreaseSpeed, minCellSize, maxCellSize);

	ImGui::Separator();

	ImGui::Text("Agent, no multiple agents implemented yet");//to edit
	ImGui::InputText("New Character", newCharacter, 64);
	ImGui::DragFloat("Character Radius", &characterMaxRadius, sliderIncreaseSpeed, minSliderValue, maxSliderValue);
	ImGui::DragFloat("Height", &characterMaxHeight, sliderIncreaseSpeed, minSliderValue, maxSliderValue);
	ImGui::DragFloat("Max slope", &characterMaxSlopeScaling, sliderIncreaseSpeed, minSliderValue, maxSlopeValue);
	ImGui::DragFloat("Max step height", &characterMaxStepHeightScaling, sliderIncreaseSpeed, minSliderValue, maxSlopeValue);
	ImGui::Button("Add Character", ImVec2(ImGui::GetWindowWidth(), 25));

	ImGui::Separator();

	ImGui::Text("Region");
	ImGui::DragInt("Min region size", &minRegionSize, 1, 8, 150);
	ImGui::DragInt("Merged region size", &minRegionSize, 1, 0, 150);

	ImGui::Separator();

	if (ImGui::Button("Partitioning", ImVec2(ImGui::GetWindowWidth(), 25)))
		ImGui::OpenPopup("partitioning_popup");
	ImGui::SameLine();
	if (ImGui::BeginPopup("partitioning_popup"))
	{
		ImGui::Text("Partitioning option");
		ImGui::Separator();
		if (ImGui::Selectable("Watershed"))
		{
			partitionType = 0;
		}
		if (ImGui::Selectable("Monotone"))
		{
			partitionType = 1;
		}
		if (ImGui::Selectable("Layers"))
		{
			partitionType = 2;
		}
		ImGui::EndPopup();
	}

	ImGui::Separator();

	ImGui::Text("Filtering");
	ImGui::Checkbox("Low hanging obstacles", &filterLowHangingObstacles);
	ImGui::Checkbox("Ledge spans", &filterLedgeSpans);
	ImGui::Checkbox("Walkable low height spans", &filterWalkableLowHeightSpans);

	ImGui::Separator();

	ImGui::Text("Polygonization");
	ImGui::DragFloat("Max edge length", &edgeMaxLength, 1, 0, 50);
	ImGui::DragFloat("Max edge error", &edgeMaxError, sliderIncreaseSpeed, 0.1f, 3.f);
	ImGui::DragInt("Verts per poly", &vertexPerPoly, 1, 3, 12);

	ImGui::Separator();

	ImGui::Text("Detail Mesh");

	ImGui::Separator();

	ImGui::Text("Generation");
	
	ImGui::DragFloat("Agent max radius", &maxRadius, sliderIncreaseSpeed, minSliderValue, maxSliderValue);
	ImGui::DragFloat("Agent max height", &maxHeight, sliderIncreaseSpeed, minSliderValue, maxSliderValue);
	ImGui::DragFloat("Max slope scaling", &maxSlopeScaling, sliderIncreaseSpeed, minSliderValue, maxSlopeValue);
	ImGui::DragFloat("Max step height", &maxStepHeightScaling, sliderIncreaseSpeed, minSliderValue, maxSlopeValue);

	if (ImGui::Button("Add mesh to navigation"))
	{
		addNavigableMesh();
	}

	if (meshComponents.size() > 0 && ImGui::Button("Generate navigability"))
	{
		generateNavigability(true);
	}

	if (meshGenerated && ImGui::Button("Toggle nav mesh rendering"))
	{
		renderMesh = !renderMesh;
	}
	
	if (ImGui::CollapsingHeader("Detour"))
	{
		ImGui::Checkbox("Select Start", &startPoint);
		if (ImGui::Button("Generate Paths")) {
			//returnPath(math::float3(verts[0], verts[1], verts[2]), math::float3(verts[nverts - 3], verts[nverts - 2], verts[nverts - 1]));
			
			math::float3 start(-1333.133f, 344.937f, 0.f);
			math::float3 end(-3578.346f, 344.937f, -1218.517f);

			pathGenerated = FindPath(start, end, path);
			//WOWPOS start, end;
			//WOWPOS start = { -8949.95f, -132.493f, 83.5312f };
			//WOWPOS end = { -9046.507f, -45.71962f, 88.33186f };
			//start.x = verts[0]; start.y = verts[1]; start.z = verts[2]; end.x = verts[nverts - 3]; end.y = verts[nverts - 2]; end.z = verts[nverts - 1];
			//WOWPOS* path = new WOWPOS[MAX_PATH];

			//dtStatus pathSize = FindStraightPath(start, end, path, 5);

			//generateNavigability();
			//DetourPoints();

		}
	}
}

void ModuleNavigation::addNavigableMesh()
{
	meshboxes.push_back(static_cast <const AABB*>(&App->scene->selected->bbox));
	meshComponents.push_back(static_cast <const ComponentRenderer*>(App->scene->selected->GetComponent(ComponentType::Renderer)));
	transformComponents.push_back(static_cast <const ComponentTransform*>(App->scene->selected->GetComponent(ComponentType::Transform)));

	std::string s = App->scene->selected->name + " added to navigation";
	LOG(s.c_str());
	objectName = App->scene->selected->name.c_str();
}

void ModuleNavigation::addNavigableMesh(const GameObject* obj)
{
	meshboxes.push_back(static_cast <const AABB*>(&obj->bbox));
	meshComponents.push_back(static_cast <const ComponentRenderer*>(obj->GetComponent(ComponentType::Renderer)));
	transformComponents.push_back(static_cast <const ComponentTransform*>(obj->GetComponent(ComponentType::Transform)));
	std::string s = obj->name + " added to navigation";
	LOG(s.c_str());
}

void ModuleNavigation::navigableObjectToggled(GameObject* obj, const bool newState)
{
	if (newState) navigationMeshes.push_back(obj);
	else removeNavMesh(obj->UUID);
}

void ModuleNavigation::renderNavMesh()
{
	if (!meshGenerated || !renderMesh)	return;
	//test process
	for (int i = 0; i < navMesh->getMaxTiles(); ++i)
	{
		const dtMeshTile* tile = navMesh->getTile(i);
		if (!tile->header) continue;

		//drawing process
		dtPolyRef base = navMesh->getPolyRefBase(tile);
		for (int i = 0; i < tile->header->polyCount; ++i)
		{
			const dtPoly* p = &tile->polys[i];
			if (p->getType() == DT_POLYTYPE_OFFMESH_CONNECTION)	// Skip off-mesh links.
				continue;

			const dtPolyDetail* pd = &tile->detailMeshes[i];

			for (int j = 0; j < pd->triCount; ++j)
			{
				const unsigned char* t = &tile->detailTris[(pd->triBase + j) * 4];
				for (int k = 0; k < 3; ++k)
				{
					if (t[k] < p->vertCount)
						dd::point(ddVec3(tile->verts[p->verts[t[k]] * 3],
							tile->verts[p->verts[t[k]] * 3 + 1],
							tile->verts[p->verts[t[k]] * 3 + 2]), ddVec3(0, 1, 0.8), 5.0f);

					else
						dd::point(ddVec3(tile->detailVerts[(pd->vertBase + t[k] - p->vertCount) * 3],
							tile->detailVerts[(pd->vertBase + t[k] - p->vertCount) * 3 + 1],
							tile->detailVerts[(pd->vertBase + t[k] - p->vertCount) * 3 + 2]), ddVec3(1, 0, 0.5), 10.0f);
				}
			}
		}

	}

	if (start != math::float3::inf)
	{
		dd::point(start, dd::colors::Red, 5.0f);
	}
	if (end != math::float3::inf)
	{
		dd::point(end, dd::colors::Red, 5.0f);
	}
	if (pathGenerated)
	{
		for (int i = 0; i< path.size(); ++i)
		{
			if (i + 1 < path.size())
			{
				dd::line(path[i], path[i + 1], dd::colors::Red);
			}
		}
	}

	

	
	//drawMeshTile();
	//glDepthMask(GL_TRUE);
}

void ModuleNavigation::removeNavMesh(unsigned ID)
{
	for (int i = 0; i < navigationMeshes.size(); ++i)
	{
		if (navigationMeshes[i]->UUID == ID)
		{
			navigationMeshes.erase(navigationMeshes.begin() + i);
			return;
		}
	}
}

void ModuleNavigation::cleanUpNavValues()
{

}

void ModuleNavigation::generateNavigability(bool render)
{
	//clean old info
	cleanValuesPRE();

	pointsUpdated = true;

	//declaring mesh box
	bmin = new float[3];
	bmin[0] = meshboxes[0]->minPoint.x; bmin[1] = meshboxes[0]->minPoint.y; bmin[2] = meshboxes[0]->minPoint.z;

	bmax = new float[3];
	bmax[0] = meshboxes[0]->maxPoint.x; bmax[1] = meshboxes[0]->maxPoint.y; bmax[2] = meshboxes[0]->maxPoint.z;

	for (int i = 1; i < meshboxes.size(); ++i)
	{
		if (meshboxes[i]->minPoint.x < bmin[0]) bmin[0] = meshboxes[i]->minPoint.x;
		if (meshboxes[i]->minPoint.y < bmin[1]) bmin[1] = meshboxes[i]->minPoint.y;
		if (meshboxes[i]->minPoint.x < bmin[2]) bmin[2] = meshboxes[i]->minPoint.z;

		if (meshboxes[i]->maxPoint.x > bmax[0]) bmax[0] = meshboxes[i]->maxPoint.x;
		if (meshboxes[i]->maxPoint.y > bmax[1]) bmax[1] = meshboxes[i]->maxPoint.y;
		if (meshboxes[i]->maxPoint.z > bmax[2]) bmax[2] = meshboxes[i]->maxPoint.z;
	}
	
	//meshComponent = static_cast <const ComponentRenderer*>(App->scene->selected->GetComponent(ComponentType::Renderer));

	fillVertices();
	//Indices
	fillIndices();

	//calculate normals
	fillNormals();

	//step 1. Initialize build config.
	memset(&cfg, 0, sizeof(cfg));

	cfg.cs = cellWidth;
	cfg.ch = cellHeight;
	cfg.walkableSlopeAngle = maxSlopeValue;
	cfg.walkableHeight = (int)ceilf(characterMaxHeight / cfg.ch);
	cfg.walkableClimb = (int)floorf(characterMaxStepHeightScaling / cfg.ch);
	cfg.walkableRadius = (int)ceilf(characterMaxRadius / cfg.cs);
	cfg.maxEdgeLen = (int)(edgeMaxLength / cellWidth);
	cfg.maxSimplificationError = edgeMaxError;
	cfg.minRegionArea = (int)rcSqr(minRegionSize);		// Note: area = size*size
	cfg.mergeRegionArea = (int)rcSqr(mergedRegionSize);	// Note: area = size*size
	cfg.maxVertsPerPoly = (int)vertexPerPoly;
	cfg.detailSampleDist = sampleDistance < 0.9f ? 0 : cellWidth * sampleDistance;
	cfg.detailSampleMaxError = cellHeight * sampleMaxError;

	// Set the area where the navigation will be build.
	// Here the bounds of the input mesh are used, but the
	// area could be specified by an user defined box, etc.
	rcVcopy(cfg.bmin, bmin);
	rcVcopy(cfg.bmax, bmax);
	rcCalcGridSize(cfg.bmin, cfg.bmax, cfg.cs, &cfg.width, &cfg.height);

	// Reset build times gathering.
	//ctx->resetTimers();

	// Start the build process.	
	//ctx->startTimer(RC_TIMER_TOTAL);

	LOG("Building Navigation");

	//step 2. Rasterize input polygon soup.

	// Allocate voxel heightfield where we rasterize our input data to.
	heightField = rcAllocHeightfield();//this one does not get the spans correctly
	if (!heightField)
	{
		LOG("buildNavigation: Out of memory 'solid'.");
		return;
	}
	if (!rcCreateHeightfield(ctx, *heightField, cfg.width, cfg.height, cfg.bmin, cfg.bmax, cfg.cs, cfg.ch))
	{
		LOG("buildNavigation: Could not create solid heightfield.");
		return;
	}

	// Allocate array that can hold triangle area types.
	// If you have multiple meshes you need to process, allocate
	// and array which can hold the max number of triangles you need to process.
	m_triareas = new unsigned char[ntris];
	if (!m_triareas)
	{
		LOG("buildNavigation: Out of memory 'm_triareas' (%d).");
		return;
	}

	// Find triangles which are walkable based on their slope and rasterize them.
	// If your input data is multiple meshes, you can transform them here, calculate
	// the are type for each of the meshes and rasterize them.
	memset(m_triareas, 0, ntris * sizeof(unsigned char));
	rcMarkWalkableTriangles(ctx, cfg.walkableSlopeAngle, verts, nverts, tris, ntris, m_triareas);//we have more verts than tris, may not be right
	if (!rcRasterizeTriangles(ctx, verts, nverts, tris, m_triareas, ntris, *heightField, cfg.walkableClimb))
	{
		LOG("buildNavigation: Could not rasterize triangles.");
		return;
	}

	if (!m_keepInterResults)
	{
		delete[] m_triareas;
		m_triareas = 0;
	}

	// Step 3. Filter walkables surfaces.
	// Once all geoemtry is rasterized, we do initial pass of filtering to
	// remove unwanted overhangs caused by the conservative rasterization
	// as well as filter spans where the character cannot possibly stand.
	if (filterLowHangingObstacles)
		rcFilterLowHangingWalkableObstacles(ctx, cfg.walkableClimb, *heightField);
	if (filterLedgeSpans)
		rcFilterLedgeSpans(ctx, cfg.walkableHeight, cfg.walkableClimb, *heightField);//a little too complex
	if (filterWalkableLowHeightSpans)
		rcFilterWalkableLowHeightSpans(ctx, cfg.walkableHeight, *heightField);

	// Step 4. Partition walkable surface to simple regions.
	// Compact the heightfield so that it is faster to handle from now on.
	// This will result more cache coherent data as well as the neighbours
	// between walkable cells will be calculated.
	chf = rcAllocCompactHeightfield();
	if (!chf)
	{
		LOG("buildNavigation: Out of memory 'chf'.");
		return;
	}
	if (!rcBuildCompactHeightfield(ctx, cfg.walkableHeight, cfg.walkableClimb, *heightField, *chf))
	{
		LOG("buildNavigation: Could not build compact data.");
		return;
	}

	if (!m_keepInterResults)
	{
		rcFreeHeightField(heightField);
		heightField = 0;
	}

	// Erode the walkable area by agent radius.
	if (!rcErodeWalkableArea(ctx, cfg.walkableRadius, *chf))
	{
		LOG("buildNavigation: Could not erode.");
		return;
	}

	// (Optional) Mark areas.
	/*const ConvexVolume* vols = m_geom->getConvexVolumes();
	for (int i = 0; i < m_geom->getConvexVolumeCount(); ++i)
		rcMarkConvexPolyArea(ctx, vols[i].verts, vols[i].nverts, vols[i].hmin, vols[i].hmax, (unsigned char)vols[i].area, *chf);*/
	
	// Partition the heightfield so that we can use simple algorithm later to triangulate the walkable areas.
	// There are 3 martitioning methods, each with some pros and cons:
	// 1) Watershed partitioning
	//   - the classic Recast partitioning
	//   - creates the nicest tessellation
	//   - usually slowest
	//   - partitions the heightfield into nice regions without holes or overlaps
	//   - the are some corner cases where this method creates produces holes and overlaps
	//      - holes may appear when a small obstacles is close to large open area (triangulation can handle this)
	//      - overlaps may occur if you have narrow spiral corridors (i.e stairs), this make triangulation to fail
	//   * generally the best choice if you precompute the nacmesh, use this if you have large open areas
	// 2) Monotone partioning
	//   - fastest
	//   - partitions the heightfield into regions without holes and overlaps (guaranteed)
	//   - creates long thin polygons, which sometimes causes paths with detours
	//   * use this if you want fast navmesh generation
	// 3) Layer partitoining
	//   - quite fast
	//   - partitions the heighfield into non-overlapping regions
	//   - relies on the triangulation code to cope with holes (thus slower than monotone partitioning)
	//   - produces better triangles than monotone partitioning
	//   - does not have the corner cases of watershed partitioning
	//   - can be slow and create a bit ugly tessellation (still better than monotone)
	//     if you have large open areas with small obstacles (not a problem if you use tiles)
	//   * good choice to use for tiled navmesh with medium and small sized tiles
	
	if (partitionType == SAMPLE_PARTITION_WATERSHED)
	{
		// Prepare for region partitioning, by calculating distance field along the walkable surface.
		if (!rcBuildDistanceField(ctx, *chf))//tocheck
		{
			LOG("buildNavigation: Could not build distance field.");
			return;
		}
		
		// Partition the walkable surface into simple regions without holes.
		if (!rcBuildRegions(ctx, *chf, 0, cfg.minRegionArea, cfg.mergeRegionArea))//tocheck
		{
			LOG("buildNavigation: Could not build watershed regions.");
			return;
		}
	}
	else if (partitionType == SAMPLE_PARTITION_MONOTONE)
	{
		// Partition the walkable surface into simple regions without holes.
		// Monotone partitioning does not need distancefield.
		if (!rcBuildRegionsMonotone(ctx, *chf, 0, cfg.minRegionArea, cfg.mergeRegionArea))
		{
			LOG("buildNavigation: Could not build monotone regions.");
			return;
		}
	}
	else // SAMPLE_PARTITION_LAYERS
	{
		// Partition the walkable surface into simple regions without holes.
		if (!rcBuildLayerRegions(ctx, *chf, 0, cfg.minRegionArea))
		{
			LOG("buildNavigation: Could not build layer regions.");
			return;
		}
	}
	
	//
	// Step 5. Trace and simplify region contours.
	//
	
	// Create contours.
	cset = rcAllocContourSet();
	if (!cset)
	{
		LOG("buildNavigation: Out of memory 'cset'.");
		return;
	}
	if (!rcBuildContours(ctx, *chf, cfg.maxSimplificationError, cfg.maxEdgeLen, *cset))
	{
		LOG("buildNavigation: Could not create contours.");
		return;
	}
	
	//
	// Step 6. Build polygons mesh from contours.
	//
	
	// Build polygon navmesh from the contours.
	pmesh = rcAllocPolyMesh();

	if (!pmesh)
	{
		LOG("buildNavigation: Out of memory 'pmesh'.");
		return;
	}
	if (!rcBuildPolyMesh(ctx, *cset, cfg.maxVertsPerPoly, *pmesh))//gotta adapt this one to fill pmesh with the values
	{
		LOG("buildNavigation: Could not triangulate contours.");
		return;
	}
	
	//
	// Step 7. Create detail mesh which allows to access approximate height on each polygon.
	//
	
	dmesh = rcAllocPolyMeshDetail();
	if (!dmesh)
	{
		LOG("buildNavigation: Out of memory 'pmdtl'.");
		return;
	}

	if (!rcBuildPolyMeshDetail(ctx, *pmesh, *chf, cfg.detailSampleDist, cfg.detailSampleMaxError, *dmesh))
	{
		LOG("buildNavigation: Could not build detail mesh.");
		return;
	}

	if (!m_keepInterResults)
	{
		rcFreeCompactHeightfield(chf);
		chf = 0;
		rcFreeContourSet(cset);
		cset = 0;
	}

	// At this point the navigation mesh data is ready, you can access it from m_pmesh.
	// See duDebugDrawPolyMesh or dtCreateNavMeshData as examples how to access the data.
	
	//
	// (Optional) Step 8. Create Detour data from Recast poly mesh.
	//
	
	// The GUI may allow more max points per polygon than Detour can handle.
	// Only build the detour navmesh if we do not exceed the limit.
	if (cfg.maxVertsPerPoly <= DT_VERTS_PER_POLYGON)
	{
		unsigned char* navData = 0;
		int navDataSize = 0;

		// Update poly flags from areas.
		for (int i = 0; i < pmesh->npolys; ++i)
		{
			if (pmesh->areas[i] == RC_WALKABLE_AREA)
				pmesh->areas[i] = SAMPLE_POLYAREA_GROUND;
				
			if (pmesh->areas[i] == SAMPLE_POLYAREA_GROUND ||
				pmesh->areas[i] == SAMPLE_POLYAREA_GRASS ||
				pmesh->areas[i] == SAMPLE_POLYAREA_ROAD)
			{
				pmesh->flags[i] = SAMPLE_POLYFLAGS_WALK;
			}
			else if (pmesh->areas[i] == SAMPLE_POLYAREA_WATER)
			{
				pmesh->flags[i] = SAMPLE_POLYFLAGS_SWIM;
			}
			else if (pmesh->areas[i] == SAMPLE_POLYAREA_DOOR)
			{
				pmesh->flags[i] = SAMPLE_POLYFLAGS_WALK | SAMPLE_POLYFLAGS_DOOR;
			}
		}


		dtNavMeshCreateParams params;
		memset(&params, 0, sizeof(params));
		params.verts = pmesh->verts;
		params.vertCount = pmesh->nverts;
		params.polys = pmesh->polys;
		params.polyAreas = pmesh->areas;
		params.polyFlags = pmesh->flags;
		params.polyCount = pmesh->npolys;
		params.nvp = pmesh->nvp;
		params.detailMeshes = dmesh->meshes;
		params.detailVerts = dmesh->verts;
		params.detailVertsCount = dmesh->nverts;
		params.detailTris = dmesh->tris;
		params.detailTriCount = dmesh->ntris;
		//params.offMeshConVerts = m_geom->getOffMeshConnectionVerts();
		//params.offMeshConRad = m_geom->getOffMeshConnectionRads();
		//params.offMeshConDir = m_geom->getOffMeshConnectionDirs();
		//params.offMeshConAreas = m_geom->getOffMeshConnectionAreas();
		//params.offMeshConFlags = m_geom->getOffMeshConnectionFlags();
		//params.offMeshConUserID = m_geom->getOffMeshConnectionId();
		//params.offMeshConCount = m_geom->getOffMeshConnectionCount();
		params.offMeshConCount = 0;
		params.walkableHeight = characterMaxHeight;
		params.walkableRadius = characterMaxRadius;
		params.walkableClimb = characterMaxStepHeightScaling;
		rcVcopy(params.bmin, pmesh->bmin);
		rcVcopy(params.bmax, pmesh->bmax);
		params.cs = cfg.cs;
		params.ch = cfg.ch;
		params.buildBvTree = true;
		
		if (!dtCreateNavMeshData(&params, &navData, &navDataSize))
		{
			LOG("Could not build Detour navmesh");
			return;
		}
		
		navMesh = dtAllocNavMesh();
		if (!navMesh)
		{
			dtFree(navData);
			LOG("Could not create Detour navmesh");
			return;
		}
		
		dtStatus status;
		
		status = navMesh->init(navData, navDataSize, DT_TILE_FREE_DATA);
		if (dtStatusFailed(status))
		{
			dtFree(navData);
			LOG("Could not init Detour navmesh");
			return;
		}

		status = navQuery->init(navMesh, 2048);
		if (dtStatusFailed(status))
		{
			LOG("Could not init Detour navmesh query");
			return;
		}
	}
	
	//ctx->stopTimer(RC_TIMER_TOTAL);

	// Show performance stats.
	//duLogBuildTimes(*m_ctx, m_ctx->getAccumulatedTime(RC_TIMER_TOTAL));
	//LOG(">> Polymesh: %d vertices  %d polygons", m_pmesh->nverts, m_pmesh->npolys);
	
	//m_totalBuildTimeMs = m_ctx->getAccumulatedTime(RC_TIMER_TOTAL)/1000.0f;
	
	/*if (m_tool)
		m_tool->init(this);
	initToolStates(this);*/
	meshGenerated = true;
	renderMesh = render;
	LOG("Navigation mesh generated");
	cleanValuesPOST();

	return;
	
}

void ModuleNavigation::fillVertices()
{
	for (int i = 0; i < meshComponents.size(); ++i)
	{
		nverts += meshComponents[i]->mesh->meshVertices.size();
	}
	verts = new float[nverts * 3];
	int currentGlobalVert = 0;
	for (int j = 0; j < meshComponents.size(); ++j)
	{
		for (int i = 0; i < meshComponents[j]->mesh->meshVertices.size(); ++i)
		{
			float4 tempVertex = float4(	meshComponents[j]->mesh->meshVertices[i].x, 
										meshComponents[j]->mesh->meshVertices[i].y,
										meshComponents[j]->mesh->meshVertices[i].z, 1.f );
			tempVertex = transformComponents[j]->global * tempVertex;


			//apply the transformation of the game object to the vertex

			//store the vertex
			verts[currentGlobalVert * 3] = tempVertex.x/tempVertex.w;
			verts[currentGlobalVert * 3 + 1] = tempVertex.y / tempVertex.w;
			verts[currentGlobalVert * 3 + 2] = tempVertex.z / tempVertex.w;
			++currentGlobalVert;
		}
	}
}

void ModuleNavigation::fillIndices()
{
	for (int i = 0; i < meshComponents.size(); ++i)
	{
		ntris += meshComponents[i]->mesh->meshIndices.size() / 3;
	}
	tris = new int[ntris * 3];//tris maps vertex and triangles
	int currentGlobalTri = 0;
	for (int j = 0; j < meshComponents.size(); ++j)
	{
		for (int i = 0; i < meshComponents[j]->mesh->meshIndices.size(); i += 3)
		{
			//changed y and z order
			tris[currentGlobalTri] = meshComponents[j]->mesh->meshIndices[i];
			tris[currentGlobalTri + 1] = meshComponents[j]->mesh->meshIndices[i + 1];
			tris[currentGlobalTri + 2] = meshComponents[j]->mesh->meshIndices[i + 2];
			currentGlobalTri += 3;
		}
	}
}

void ModuleNavigation::fillNormals()
{
	//revisar
	normals = new float[ntris*3];
	for (int i = 0; i < ntris*3; i+=3)
	{
		const float* v0 = &verts[tris[i]*3];
		const float* v1 = &verts[tris[i + 1]*3];
		const float* v2 = &verts[tris[i + 2]*3];
		float e0[3], e1[3];
		for (int j = 0; j < 3; ++j)
		{
			e0[j] = v1[j] - v0[j];
			e1[j] = v2[j] - v0[j];
		}
		float* n = &normals[i];
		n[0] = e0[1] * e1[2] - e0[2] * e1[1];
		n[1] = e0[2] * e1[0] - e0[0] * e1[2];
		n[2] = e0[0] * e1[1] - e0[1] * e1[0];
		float d = sqrtf(n[0] * n[0] + n[1] * n[1] + n[2] * n[2]);
		if (d > 0)
		{
			d = 1.0f / d;
			n[0] *= d;
			n[1] *= d;
			n[2] *= d;
		}
	}
}

void ModuleNavigation::fillDrawPoints()
{
	points = new dd::DrawVertex[nverts];
	for (int i = 0; i < nverts; ++i)
	{
		points[i].point.x = verts[i*3];
		points[i].point.y = verts[i*3 + 1];
		points[i].point.z = verts[i*3 + 2];
		points[i].point.r = 0;
		points[i].point.g = 0;
		points[i].point.b = 1;
	}
}

class myPoint : public dd::RenderInterface
{
	~myPoint() 
	{}
};

//Detour stuff http://irrlicht.sourceforge.net/forum/viewtopic.php?f=9&t=49482
std::vector<math::float3>  ModuleNavigation::returnPath(math::float3 pStart, math::float3 pEnd)
{
	std::vector<math::float3> lstPoints;
	
	if (navQuery)
	{
		if (navMesh == 0)
		{
			return  lstPoints;
		}
	/*if (pointsUpdated)
	{
		fillDrawPoints();
		pointsUpdated = false;
	}
	myPoint* renderIface = new myPoint();
	renderIface->drawPointList(points, nverts, false);*/
	/*for (int i = 0; i < nverts; ++i)
	{
		dd::point(ddVec3(verts[i * 3], verts[i * 3+1], verts[i * 3+2]), ddVec3(0,0,1), 10.0f);
	}
	*/
	
	//dd::xzSquareGrid(-500.0f * 10, 500.0f * 10, 0.0f, 1.0f * 10, math::float3(0.65f, 0.65f, 0.65f));
	//dtPolyRef base = navMesh->getPolyRefBase(tile);

		dtQueryFilter m_filter;
		dtPolyRef m_startRef;
		dtPolyRef m_endRef;

		const int MAX_POLYS = 256;
		dtPolyRef m_polys[MAX_POLYS];
		dtPolyRef returnedPath[MAX_POLYS];
		float m_straightPath[MAX_POLYS * 3];
		int numStraightPaths;
		float  m_spos[3] = { pStart.x, pStart.y, pStart.z };
		float  m_epos[3] = { pEnd.x, pEnd.y, pEnd.z };
		float m_polyPickExt[3];
		m_polyPickExt[0] = 2;
		m_polyPickExt[1] = 4;
		m_polyPickExt[2] = 2;


		navQuery->findNearestPoly(m_spos, m_polyPickExt, &m_filter, &m_startRef, 0);

		if (m_startRef == 0)
		{
			return lstPoints;

		}
		navQuery->findNearestPoly(m_epos, m_polyPickExt, &m_filter, &m_endRef, 0);

		if (m_endRef == 0)
		{
			return lstPoints;

		}
		dtStatus findStatus = DT_FAILURE;
		int pathCount;

		findStatus = navQuery->findPath(m_startRef, m_endRef, m_spos, m_epos, &m_filter, returnedPath, &pathCount, MAX_POLYS);



		if (pathCount > 0)
		{
			findStatus = navQuery->findStraightPath(m_spos, m_epos, returnedPath,
				pathCount, m_straightPath, 0, 0, &numStraightPaths, MAX_POLYS);

			for (int i = 0; i < numStraightPaths; ++i)
			{
				float3 cpos(m_straightPath[i * 3], m_straightPath[i * 3 + 1] + 0.25,
					m_straightPath[i * 3 + 2]);

				lstPoints.push_back(cpos);
				//path->AddNode(node);
			}


		}

	}
	return lstPoints;
}
/* TODO add where the mesh is calculated!!!

	/*scene::IAnimatedMesh *terrain_model = smgr->addHillPlaneMesh("groundPlane", // Name of the scenenode
		tileSize, // Tile size
		tileCount, // Tile count
		0, // Material
		20.0f, // Hill height
		core::dimension2d<f32>(0.0f, 1.0f), // countHills
		core::dimension2d<f32>(1.0f, 1.0f)); ;// textureRepeatCount

	terrain_node = smgr->addAnimatedMeshSceneNode(terrain_model);

	scene::IMeshBuffer *terbuffer = terrain_node->getMesh()->getMeshBuffer(0);


	if (terbuffer)
	{
		recast = new RecastUtilM();
		if (recast->handleBuild(terbuffer))
		{
			scene::SMesh* smesh = new scene::SMesh();
			if (!recast->setupIrrSMeshFromRecastDetailMesh(smesh))
			{
				printf("recast->setupIrrSMeshFromRecastDetailMesh(smesh): FAILED!\n");
			}
			else
			{
				scene::ISceneNode *naviNode = smgr->addOctTreeSceneNode(smesh);
				naviNode->setName("Terrain");
				naviNode->setDebugDataVisible(scene::EDS_MESH_WIRE_OVERLAY);
			}
			smesh->drop();
		}
	}
	

//On your event input positions
//std::vector<math::float3> lstPoints = ModuleNavigation->returnPath(vector3df_Start, vector3df_End);

	dd->depthMask(true);
}

//Detour stuff http://irrlicht.sourceforge.net/forum/viewtopic.php?f=9&t=49482
/*std::vector<math::float3>  ModuleNavigation::returnPath(math::float3 pStart, math::float3 pEnd)
{
	std::vector<math::float3> lstPoints;
	
	if (navQuery)
	{
		if (navMesh == 0)
		{
			return  lstPoints;
		}

		dtQueryFilter m_filter;
		dtPolyRef m_startRef;
		dtPolyRef m_endRef;

		const int MAX_POLYS = 256;
		dtPolyRef m_polys[MAX_POLYS];
		dtPolyRef returnedPath[MAX_POLYS];
		float m_straightPath[MAX_POLYS * 3];
		int numStraightPaths;
		float  m_spos[3] = { pStart.x, pStart.y, pStart.z };
		float  m_epos[3] = { pEnd.x, pEnd.y, pEnd.z };
		float m_polyPickExt[3];
		m_polyPickExt[0] = 2;
		m_polyPickExt[1] = 4;
		m_polyPickExt[2] = 2;


		navQuery->findNearestPoly(m_spos, m_polyPickExt, &m_filter, &m_startRef, 0);

		if (m_startRef == 0)
		{
			return lstPoints;

		}
		navQuery->findNearestPoly(m_epos, m_polyPickExt, &m_filter, &m_endRef, 0);

		if (m_endRef == 0)
		{
			return lstPoints;

		}
		dtStatus findStatus = DT_FAILURE;
		int pathCount;

		findStatus = navQuery->findPath(m_startRef, m_endRef, m_spos, m_epos, &m_filter, returnedPath, &pathCount, MAX_POLYS);



		if (pathCount > 0)
		{
			findStatus = navQuery->findStraightPath(m_spos, m_epos, returnedPath,
				pathCount, m_straightPath, 0, 0, &numStraightPaths, MAX_POLYS);

			for (int i = 0; i < numStraightPaths; ++i)
			{
				float3 cpos(m_straightPath[i * 3], m_straightPath[i * 3 + 1] + 0.25,
					m_straightPath[i * 3 + 2]);

				lstPoints.push_back(cpos);
				//path->AddNode(node);
			}


		}

	}
	return lstPoints;
}*/
/* TODO add where the mesh is calculated!!!

	/*scene::IAnimatedMesh *terrain_model = smgr->addHillPlaneMesh("groundPlane", // Name of the scenenode
		tileSize, // Tile size
		tileCount, // Tile count
		0, // Material
		20.0f, // Hill height
		core::dimension2d<f32>(0.0f, 1.0f), // countHills
		core::dimension2d<f32>(1.0f, 1.0f)); ;// textureRepeatCount

	terrain_node = smgr->addAnimatedMeshSceneNode(terrain_model);

	scene::IMeshBuffer *terbuffer = terrain_node->getMesh()->getMeshBuffer(0);


	if (terbuffer)
	{
		recast = new RecastUtilM();
		if (recast->handleBuild(terbuffer))
		{
			scene::SMesh* smesh = new scene::SMesh();
			if (!recast->setupIrrSMeshFromRecastDetailMesh(smesh))
			{
				printf("recast->setupIrrSMeshFromRecastDetailMesh(smesh): FAILED!\n");
			}
			else
			{
				scene::ISceneNode *naviNode = smgr->addOctTreeSceneNode(smesh);
				naviNode->setName("Terrain");
				naviNode->setDebugDataVisible(scene::EDS_MESH_WIRE_OVERLAY);
			}
			smesh->drop();
		}
	}
	*/

//On your event input positions
//std::vector<math::float3> lstPoints = ModuleNavigation->returnPath(vector3df_Start, vector3df_End);

// TODO: New Detour version

bool ModuleNavigation::FindPath(math::float3 start, math::float3 end, std::vector<math::float3>& path) const
{
	path.clear();
	dtPolyRef startPoly, endPoly;
	dtQueryFilter filter;
	filter.setIncludeFlags(SAMPLE_POLYFLAGS_ALL ^ SAMPLE_POLYFLAGS_DISABLED);
	filter.setExcludeFlags(0);

	float polyPickExt[3] = { 0,0,0 };

	static const int MAX_POLYS = 256;
	dtPolyRef polyPath[MAX_POLYS];
	int pathCount = 0;

	navQuery->findNearestPoly((float*)&start, polyPickExt, &filter, &startPoly, 0); // find start Poly
	navQuery->findNearestPoly((float*)&end, polyPickExt, &filter, &endPoly, 0); // find end Poly

	if (!startPoly)
	{
		LOG("Could not find any nearby poly to the start");
		return false;
	}
	if (!endPoly)
	{
		LOG("Could not find any nearby poly to the end");
		return false;
	}

	navQuery->findPath(startPoly, endPoly, (float*)&start, (float*)&end, &filter, polyPath, &pathCount, MAX_POLYS);
	if (pathCount)
	{
		float straightPath[MAX_POLYS * 3]; //deprecated for path variable
		//unsigned char straightPathFlags[MAX_POLYS];
		//dtPolyRef m_straightPathPolys[MAX_POLYS];
		int nbPoints = 0;
		int straightPathOptions = 0;
		//TODO: partial path + end poly clamping
		navQuery->findStraightPath((float*)&start, (float*)&end, polyPath, pathCount,
			straightPath, NULL, NULL, &nbPoints, MAX_POLYS, straightPathOptions);

		if (nbPoints != 0)
		{
			path.reserve(nbPoints);
			for (size_t i = 0; i < nbPoints; i++)
			{
				path.push_back(float3(straightPath[i*3], straightPath[i * 3 + 1], straightPath[i * 3 + 2]));
			}
			return true;
		}
	}
	return false;
}

void ModuleNavigation::RecalcPath(math::float3 point)
{
	if (startPoint)
	{
		start = point;
	}
	else
	{
		end = point;
	}
	if (start != end && start != math::float3::inf && end != math::float3::inf)
	{
		pathGenerated = FindPath(start, end, path);
	}
}

//int ModuleNavigation::FindStraightPath(WOWPOS start, WOWPOS end, WOWPOS *path, int size)
//{

	//
//	float m_spos[3];
//
//	m_spos[0] = -1.0f * start.y;
//	m_spos[1] = start.z;
//	m_spos[2] = -1.0f * start.x;
//
//	//
//	float m_epos[3];
//
//	m_epos[0] = -1.0f * end.y;
//	m_epos[1] = end.z;
//	m_epos[2] = -1.0f * end.x;
//
//	//
//	dtQueryFilter m_filter;
//
//	//m_filter.includeFlags = 0xffff;
//	//m_filter.excludeFlags = 0;
//
//	//
//	float m_polyPickExt[3];
//
//	m_polyPickExt[0] = 2;
//	m_polyPickExt[1] = 4;
//	m_polyPickExt[2] = 2;
//
//	//
//	dtPolyRef m_startRef;
//	dtPolyRef m_endRef;
//
//	//
//	/*m_startRef = navQuery->findNearestPoly(m_spos, m_polyPickExt, &m_filter, 0);
//	m_endRef = navQuery->findNearestPoly(m_epos, m_polyPickExt, &m_filter, 0);*/
//	navQuery->findNearestPoly(m_spos, m_polyPickExt, &m_filter, &m_startRef, 0);
//	navQuery->findNearestPoly(m_spos, m_polyPickExt, &m_filter, &m_startRef, 0);
//
//	//
//	if (!m_startRef || !m_endRef)
//	{
//		LOG("Could not find any nearby poly to the start");
//		return ERROR_NEARESTPOLY;
//	}
//	if (!m_endRef)
//	{
//		LOG("Could not find any nearby poly to the end");
//		return ERROR_NEARESTPOLY;
//	}
//
//	//
//	static const int MAX_POLYS = 256;
//
//	dtPolyRef m_polys[MAX_POLYS];
//
//	int m_npolys;
//	float m_straightPath[MAX_POLYS * 3];
//	unsigned char m_straightPathFlags[MAX_POLYS];
//
//	dtPolyRef m_straightPathPolys[MAX_POLYS];
//
//	int m_nstraightPath;
//
//	//
//	int pos = 0;
//
//	//
//	//m_npolys = navQuery->findPath(m_startRef, m_endRef, m_spos, m_epos, &m_filter, m_polys, MAX_POLYS);
//	navQuery->findPath(m_startRef, m_endRef, m_spos, m_epos, &m_filter, m_polys, &m_npolys, MAX_POLYS);
//	m_nstraightPath = 0;
//
//	if (m_npolys)
//	{
//		//m_nstraightPath = navQuery->findStraightPath(m_spos, m_epos, m_polys, m_npolys, m_straightPath, m_straightPathFlags, m_straightPathPolys, MAX_POLYS);
//		navQuery->findStraightPath(m_spos, m_epos, m_polys, m_npolys, m_straightPath, m_straightPathFlags,
//			m_straightPathPolys, &m_nstraightPath, MAX_POLYS, 0);//0 are the options
//
//		for (int i = 0; i < m_nstraightPath * 3; )
//		{
//			path[pos].y = -1.0f * m_straightPath[i++];
//			path[pos].z = m_straightPath[i++];
//			path[pos].x = -1.0f * m_straightPath[i++];
//
//			pos++;
//		}
//
//		// append the end point
//		path[pos].x = end.x;
//		path[pos].y = end.y;
//		path[pos].z = end.z;
//
//		pos++;
//	}
//
//	return pos;
//}
