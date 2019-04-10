#include "Application.h"
#include "Globals.h"
#include "GameObject.h"

#include "ModuleNavigation.h"
#include "ModuleScene.h"

#include "Component.h"
#include "ComponentRenderer.h"

#include "ResourceMesh.h"

#include "Geometry/AABB.h"

#include "imgui.h"

#include "Recast/Recast.h"
#include "Detour/DetourNavMesh.h"
#include "Detour/DetourNavMeshBuilder.h"
#include "Detour/DetourNavMeshQuery.h"

ModuleNavigation::ModuleNavigation()
{
}


ModuleNavigation::~ModuleNavigation()
{
	RELEASE_ARRAY(verts);
	RELEASE_ARRAY(tris);
}

void ModuleNavigation::DrawGUI()
{
	if (ImGui::CollapsingHeader("Agents"))
	{
		ImGui::InputText("New Character", newCharacter, 64);
		ImGui::DragFloat("Character Radius", &characterMaxRadius, sliderIncreaseSpeed, minSliderValue, maxSliderValue);
		ImGui::DragFloat("Height", &characterMaxHeight, sliderIncreaseSpeed, minSliderValue, maxSliderValue);
		ImGui::DragFloat("Max slope", &characterMaxSlopeScaling, sliderIncreaseSpeed, minSliderValue, maxSlopeValue);
		ImGui::DragFloat("Max step height", &characterMaxStepHeightScaling, sliderIncreaseSpeed, minSliderValue, maxSlopeValue);
		ImGui::Button("Add Character", ImVec2(ImGui::GetWindowWidth(), 25));
	}
	if (ImGui::CollapsingHeader("Areas"))
	{

	}
	if (ImGui::CollapsingHeader("Bake"))
	{
		ImGui::DragFloat("Agent max radius", &maxRadius, sliderIncreaseSpeed, minSliderValue, maxSliderValue);
		ImGui::DragFloat("Agent max height", &maxHeight, sliderIncreaseSpeed, minSliderValue, maxSliderValue);
		ImGui::DragFloat("Max slope scaling", &maxSlopeScaling, sliderIncreaseSpeed, minSliderValue, maxSlopeValue);
		ImGui::DragFloat("Max step height", &maxStepHeightScaling, sliderIncreaseSpeed, minSliderValue, maxSlopeValue);

		if (ImGui::Button("Generate navigability"))
		{
			generateNavigability();
		}
	}
	if (ImGui::CollapsingHeader("Object"))
	{
		
	}
}

void ModuleNavigation::navigableObjectToggled(GameObject* obj, const bool newState)
{
	if (newState) navigationMeshes.push_back(obj);
	else removeNavMesh(obj->UUID);
}

void ModuleNavigation::renderNavMesh()
{
	/*
	from sample_solomesh.cpp in recast its in the line 268, the lib of the function is detourdebugdraw.h, gonna need some structures.
	if (m_drawMode != DRAWMODE_NAVMESH_INVIS)
			duDebugDrawNavMeshWithClosedList(&m_dd, *m_navMesh, *m_navQuery, m_navMeshDrawFlags);
	*/
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

void ModuleNavigation::generateNavigability()
{
	//clean old info
	cleanUpNavValues();

	//const shit
	const AABB* const meshbox  = static_cast <const AABB* const>(&App->scene->selected->bbox);
	
	const float bmin[3] = {meshbox->minPoint.x, meshbox->minPoint.y, meshbox->minPoint.z };
	const float bmax[3] = {meshbox->maxPoint.x, meshbox->maxPoint.y, meshbox->maxPoint.z};
	
	meshComponent = static_cast <const ComponentRenderer*>(App->scene->selected->GetComponent(ComponentType::Renderer));

	//reallocate arrays, should ask
	//meshComponent->mesh->meshVertices.resize(meshComponent->mesh->meshVertices.capacity + 1);
	//meshComponent->mesh->meshVertices.resize(meshComponent->mesh->meshVertices.capacity + 1);

	const int nverts = meshComponent->mesh->meshVertices.size();
	verts = new float[nverts*3];
	fillVertices(verts, nverts);
	//Indices
	const int ntris = meshComponent->mesh->meshIndices.size()/3;
	tris = new int[ntris*3];
	
	fillIndices(tris, ntris);

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
	heightField = rcAllocHeightfield();
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
	rcMarkWalkableTriangles(ctx, cfg.walkableSlopeAngle, verts, nverts, tris, ntris, m_triareas);
	/*if (!rcRasterizeTriangles(ctx, verts, nverts, tris, m_triareas, ntris, *heightField, cfg.walkableClimb))
	{
		LOG("buildNavigation: Could not rasterize triangles.");
		return;
	}*/

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
		rcFilterLedgeSpans(ctx, cfg.walkableHeight, cfg.walkableClimb, *heightField);
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
		rcMarkConvexPolyArea(m_ctx, vols[i].verts, vols[i].nverts, vols[i].hmin, vols[i].hmax, (unsigned char)vols[i].area, *m_chf);*/
	
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
		if (!rcBuildDistanceField(ctx, *chf))
		{
			LOG("buildNavigation: Could not build distance field.");
			return;
		}
		
		// Partition the walkable surface into simple regions without holes.
		if (!rcBuildRegions(ctx, *chf, 0, cfg.minRegionArea, cfg.mergeRegionArea))
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
	if (!rcBuildPolyMesh(ctx, *cset, cfg.maxVertsPerPoly, *pmesh))
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
			LOG("Could not build Detour navmesh.");
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

	return;
	
}

void ModuleNavigation::fillVertices(float* verts, const int nverts)
{
	for (int i = 0; i < nverts; ++i)
	{
		verts[i* 3] = meshComponent->mesh->meshVertices[i].x;
		verts[i* 3 + 1] = meshComponent->mesh->meshVertices[i].y;
		verts[i* 3 + 2] = meshComponent->mesh->meshVertices[i].z;
	}
}

void ModuleNavigation::fillIndices(int* tris, const int ntris)
{
	for (int i = 0; i < ntris; ++i)
	{
		tris[i * 3] = meshComponent->mesh->meshIndices[i];
		tris[i * 3 + 1] = meshComponent->mesh->meshIndices[i+1];
		tris[i * 3 + 2] = meshComponent->mesh->meshIndices[i+2];
	}
}