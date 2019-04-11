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
#include "SDL_opengl.h"

#include "Recast/Recast.h"
#include "Detour/DetourNavMesh.h"
#include "Detour/DetourNavMeshBuilder.h"
#include "Detour/DetourNavMeshQuery.h"
#include "Recast/DebugUtils/RecastDebugDraw.h"

ModuleNavigation::ModuleNavigation()
{
}


ModuleNavigation::~ModuleNavigation()
{
	RELEASE_ARRAY(verts);
	RELEASE_ARRAY(tris);
	RELEASE_ARRAY(normals);
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
		ImGui::DragFloat("Cell width", &cellWidth, cellIncreaseSpeed, minCellSize, maxCellSize);
		ImGui::DragFloat("Cell height", &cellHeight, cellIncreaseSpeed, minCellSize, maxCellSize);
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
	//duDebugDrawNavMeshWithClosedList(&dd, *navMesh, *navQuery, m_navMeshDrawFlags);

	if (!meshGenerated)	return;

	glEnable(GL_FOG);
	glDepthMask(GL_TRUE);

	const float texScale = 1.0f / (cellWidth * 10.0f);
	
	if (m_drawMode != DRAWMODE_NAVMESH_TRANS)
	{
		// Draw mesh
		duDebugDrawTriMeshSlope(&dd, verts, nverts,
			tris, normals, ntris,
			characterMaxSlopeScaling, texScale);
		//m_geom->drawOffMeshConnections(&dd);//dont know about this one
	}

	glDisable(GL_FOG);
	glDepthMask(GL_FALSE);

	// Draw bounds
	const float bmin[3] = { meshbox->minPoint.x, meshbox->minPoint.y, meshbox->minPoint.z };
	const float bmax[3] = { meshbox->maxPoint.x, meshbox->maxPoint.y, meshbox->maxPoint.z };
	//const float* bmin = { meshbox->minPoint.x, meshbox->minPoint.y, meshbox->minPoint.z };
	//const float* bmax = { meshbox->maxPoint.x, meshbox->maxPoint.y, meshbox->maxPoint.z };

	duDebugDrawBoxWire(&dd, bmin[0], bmin[1], bmin[2], bmax[0], bmax[1], bmax[2], duRGBA(255, 255, 255, 128), 1.0f);
	dd.begin(DU_DRAW_POINTS, 5.0f);
	dd.vertex(bmin[0], bmin[1], bmin[2], duRGBA(255, 255, 255, 128));
	dd.end();

	if (navMesh && navQuery &&
		(m_drawMode == DRAWMODE_NAVMESH ||
			m_drawMode == DRAWMODE_NAVMESH_TRANS ||
			m_drawMode == DRAWMODE_NAVMESH_BVTREE ||
			m_drawMode == DRAWMODE_NAVMESH_NODES ||
			m_drawMode == DRAWMODE_NAVMESH_INVIS))
	{
		if (m_drawMode != DRAWMODE_NAVMESH_INVIS)
			duDebugDrawNavMeshWithClosedList(&dd, *navMesh, *navQuery, m_navMeshDrawFlags);
		if (m_drawMode == DRAWMODE_NAVMESH_BVTREE)
			duDebugDrawNavMeshBVTree(&dd, *navMesh);
		if (m_drawMode == DRAWMODE_NAVMESH_NODES)
			duDebugDrawNavMeshNodes(&dd, *navQuery);
		duDebugDrawNavMeshPolysWithFlags(&dd, *navMesh, SAMPLE_POLYFLAGS_DISABLED, duRGBA(0, 0, 0, 128));
	}

	glDepthMask(GL_TRUE);

	if (chf && m_drawMode == DRAWMODE_COMPACT)
		duDebugDrawCompactHeightfieldSolid(&dd, *chf);

	if (chf && m_drawMode == DRAWMODE_COMPACT_DISTANCE)
		duDebugDrawCompactHeightfieldDistance(&dd, *chf);
	if (chf && m_drawMode == DRAWMODE_COMPACT_REGIONS)
		duDebugDrawCompactHeightfieldRegions(&dd, *chf);
	if (heightField && m_drawMode == DRAWMODE_VOXELS)
	{
		glEnable(GL_FOG);
		duDebugDrawHeightfieldSolid(&dd, *heightField);
		glDisable(GL_FOG);
	}
	if (heightField && m_drawMode == DRAWMODE_VOXELS_WALKABLE)
	{
		glEnable(GL_FOG);
		duDebugDrawHeightfieldWalkable(&dd, *heightField);
		glDisable(GL_FOG);
	}
	if (cset && m_drawMode == DRAWMODE_RAW_CONTOURS)
	{
		glDepthMask(GL_FALSE);
		duDebugDrawRawContours(&dd, *cset);
		glDepthMask(GL_TRUE);
	}
	if (cset && m_drawMode == DRAWMODE_BOTH_CONTOURS)
	{
		glDepthMask(GL_FALSE);
		duDebugDrawRawContours(&dd, *cset, 0.5f);
		duDebugDrawContours(&dd, *cset);
		glDepthMask(GL_TRUE);
	}
	if (cset && m_drawMode == DRAWMODE_CONTOURS)
	{
		glDepthMask(GL_FALSE);
		duDebugDrawContours(&dd, *cset);
		glDepthMask(GL_TRUE);
	}
	if (chf && cset && m_drawMode == DRAWMODE_REGION_CONNECTIONS)
	{
		duDebugDrawCompactHeightfieldRegions(&dd, *chf);

		glDepthMask(GL_FALSE);
		duDebugDrawRegionConnections(&dd, *cset);
		glDepthMask(GL_TRUE);
	}
	if (pmesh && m_drawMode == DRAWMODE_POLYMESH)
	{
		glDepthMask(GL_FALSE);
		duDebugDrawPolyMesh(&dd, *pmesh);
		glDepthMask(GL_TRUE);
	}
	if (dmesh && m_drawMode == DRAWMODE_POLYMESH_DETAIL)
	{
		glDepthMask(GL_FALSE);
		duDebugDrawPolyMeshDetail(&dd, *dmesh);
		glDepthMask(GL_TRUE);
	}

	drawConvexVolumes(&dd);

	/*if (tool)
		tool->handleRender();
	renderToolStates();*/
	
	glDepthMask(GL_TRUE);
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
	meshbox  = static_cast <const AABB*>(&App->scene->selected->bbox);
	
	const float bmin[3] = {meshbox->minPoint.x, meshbox->minPoint.y, meshbox->minPoint.z };
	const float bmax[3] = {meshbox->maxPoint.x, meshbox->maxPoint.y, meshbox->maxPoint.z};
	
	meshComponent = static_cast <const ComponentRenderer*>(App->scene->selected->GetComponent(ComponentType::Renderer));

	nverts = meshComponent->mesh->meshVertices.size();
	verts = new float[nverts*3];
	fillVertices();
	//Indices
	ntris = meshComponent->mesh->meshIndices.size()/3;
	tris = new int[ntris*3];
	
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
	meshGenerated = true;

	return;
	
}

void ModuleNavigation::fillVertices()
{
	for (int i = 0; i < nverts; ++i)
	{
		verts[i* 3] = meshComponent->mesh->meshVertices[i].x;
		verts[i* 3 + 1] = meshComponent->mesh->meshVertices[i].y;
		verts[i* 3 + 2] = meshComponent->mesh->meshVertices[i].z;
	}
}

void ModuleNavigation::fillIndices()
{
	for (int i = 0; i < ntris; ++i)
	{
		tris[i * 3] = meshComponent->mesh->meshIndices[i];
		tris[i * 3 + 1] = meshComponent->mesh->meshIndices[i + 1];
		tris[i * 3 + 2] = meshComponent->mesh->meshIndices[i + 2];
	}
}

void ModuleNavigation::fillNormals()
{
	normals = new float[ntris*3];
	for (int i = 0; i < ntris; ++i)
	{
		const float* v0 = &verts[tris[i] * 3];
		const float* v1 = &verts[tris[i + 1] * 3];
		const float* v2 = &verts[tris[i + 2] * 3];
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

//debug draw implementations
unsigned int SampleDebugDraw::areaToCol(unsigned int area)
{
	switch (area)
	{
		// Ground (0) : light blue
	case SAMPLE_POLYAREA_GROUND: return duRGBA(0, 192, 255, 255);
		// Water : blue
	case SAMPLE_POLYAREA_WATER: return duRGBA(0, 0, 255, 255);
		// Road : brown
	case SAMPLE_POLYAREA_ROAD: return duRGBA(50, 20, 12, 255);
		// Door : cyan
	case SAMPLE_POLYAREA_DOOR: return duRGBA(0, 255, 255, 255);
		// Grass : green
	case SAMPLE_POLYAREA_GRASS: return duRGBA(0, 255, 0, 255);
		// Jump : yellow
	case SAMPLE_POLYAREA_JUMP: return duRGBA(255, 255, 0, 255);
		// Unexpected : red
	default: return duRGBA(255, 0, 0, 255);
	}
}

void DebugDrawGL::depthMask(bool state)
{
	glDepthMask(state ? GL_TRUE : GL_FALSE);
}

void DebugDrawGL::texture(bool state)
{
	if (state)
	{
		glEnable(GL_TEXTURE_2D);
		//g_tex.bind();
	}
	else
	{
		glDisable(GL_TEXTURE_2D);
	}
}

void DebugDrawGL::begin(duDebugDrawPrimitives prim, float size)
{
	switch (prim)
	{
	case DU_DRAW_POINTS:
		glPointSize(size);
		glBegin(GL_POINTS);
		break;
	case DU_DRAW_LINES:
		glLineWidth(size);
		glBegin(GL_LINES);
		break;
	case DU_DRAW_TRIS:
		glBegin(GL_TRIANGLES);
		break;
	case DU_DRAW_QUADS:
		glBegin(GL_QUADS);
		break;
	};
}

void DebugDrawGL::vertex(const float* pos, unsigned int color)
{
	glColor4ubv((GLubyte*)&color);
	glVertex3fv(pos);
}

void DebugDrawGL::vertex(const float x, const float y, const float z, unsigned int color)
{
	glColor4ubv((GLubyte*)&color);
	glVertex3f(x, y, z);
}

void DebugDrawGL::vertex(const float* pos, unsigned int color, const float* uv)
{
	glColor4ubv((GLubyte*)&color);
	glTexCoord2fv(uv);
	glVertex3fv(pos);
}

void DebugDrawGL::vertex(const float x, const float y, const float z, unsigned int color, const float u, const float v)
{
	glColor4ubv((GLubyte*)&color);
	glTexCoord2f(u, v);
	glVertex3f(x, y, z);
}

void DebugDrawGL::end()
{
	glEnd();
	glLineWidth(1.0f);
	glPointSize(1.0f);
}

void drawConvexVolumes(struct duDebugDraw* dd, bool hilight = false)
{
	dd->depthMask(false);

	dd->begin(DU_DRAW_TRIS);

	for (int i = 0; i < m_volumeCount; ++i)
	{
		const ConvexVolume* vol = &m_volumes[i];
		unsigned int col = duTransCol(dd->areaToCol(vol->area), 32);
		for (int j = 0, k = vol->nverts - 1; j < vol->nverts; k = j++)
		{
			const float* va = &vol->verts[k * 3];
			const float* vb = &vol->verts[j * 3];

			dd->vertex(vol->verts[0], vol->hmax, vol->verts[2], col);
			dd->vertex(vb[0], vol->hmax, vb[2], col);
			dd->vertex(va[0], vol->hmax, va[2], col);

			dd->vertex(va[0], vol->hmin, va[2], duDarkenCol(col));
			dd->vertex(va[0], vol->hmax, va[2], col);
			dd->vertex(vb[0], vol->hmax, vb[2], col);

			dd->vertex(va[0], vol->hmin, va[2], duDarkenCol(col));
			dd->vertex(vb[0], vol->hmax, vb[2], col);
			dd->vertex(vb[0], vol->hmin, vb[2], duDarkenCol(col));
		}
	}

	dd->end();

	dd->begin(DU_DRAW_LINES, 2.0f);
	for (int i = 0; i < m_volumeCount; ++i)
	{
		const ConvexVolume* vol = &m_volumes[i];
		unsigned int col = duTransCol(dd->areaToCol(vol->area), 220);
		for (int j = 0, k = vol->nverts - 1; j < vol->nverts; k = j++)
		{
			const float* va = &vol->verts[k * 3];
			const float* vb = &vol->verts[j * 3];
			dd->vertex(va[0], vol->hmin, va[2], duDarkenCol(col));
			dd->vertex(vb[0], vol->hmin, vb[2], duDarkenCol(col));
			dd->vertex(va[0], vol->hmax, va[2], col);
			dd->vertex(vb[0], vol->hmax, vb[2], col);
			dd->vertex(va[0], vol->hmin, va[2], duDarkenCol(col));
			dd->vertex(va[0], vol->hmax, va[2], col);
		}
	}
	dd->end();

	dd->begin(DU_DRAW_POINTS, 3.0f);
	for (int i = 0; i < m_volumeCount; ++i)
	{
		const ConvexVolume* vol = &m_volumes[i];
		unsigned int col = duDarkenCol(duTransCol(dd->areaToCol(vol->area), 220));
		for (int j = 0; j < vol->nverts; ++j)
		{
			dd->vertex(vol->verts[j * 3 + 0], vol->verts[j * 3 + 1] + 0.1f, vol->verts[j * 3 + 2], col);
			dd->vertex(vol->verts[j * 3 + 0], vol->hmin, vol->verts[j * 3 + 2], col);
			dd->vertex(vol->verts[j * 3 + 0], vol->hmax, vol->verts[j * 3 + 2], col);
		}
	}
	dd->end();


	dd->depthMask(true);
}