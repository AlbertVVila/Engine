#include "Application.h"
#include "Globals.h"
#include "GameObject.h"
#include "MathGeoLib/include/Math/float3.h"

#include "ModuleNavigation.h"
#include "ModuleScene.h"
#include "ModuleDebugDraw.h"

#include "Component.h"
#include "ComponentRenderer.h"

#include "ResourceMesh.h"

#include "Geometry/AABB.h"

#include "imgui.h"
#include "SDL_opengl.h"
//#include "debugdraw.h"


#include "Recast/Recast.h"
#include "Detour/DetourNavMesh.h"
#include "Detour/DetourNavMeshBuilder.h"
#include "Detour/DetourNavMeshQuery.h"
#include "Recast/DebugUtils/RecastDebugDraw.h"
#include "DebugUtils/DetourDebugDraw.h"
#include "DebugUtils/DebugDraw.h"
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

void ModuleNavigation::cleanValues()
{
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
	if (ImGui::CollapsingHeader("Detour"))
	{
		ImGui::Text("Start Point:");
		ImGui::InputFloat3("SP", pStart, 3);
		
		ImGui::Text("End Point:");
		ImGui::InputFloat3("EP", pEnd, 3);
		
		if (ImGui::Button("Generate Paths"))
			if (!pStart) return;
			else if (!pStart) return;
			else
				//std::vector<math::float3> lstPoints = returnPath(pStart, pEnd);
	}
}

void ModuleNavigation::navigableObjectToggled(GameObject* obj, const bool newState)
{
	if (newState) navigationMeshes.push_back(obj);
	else removeNavMesh(obj->UUID);
}

void ModuleNavigation::renderNavMesh()
{
	if (!meshGenerated)	return;
	//get a const instance of navmesh
	/*const dtNavMesh* tmpNavMesh = navMesh;
	for (int i = 0; i < navMesh->getMaxTiles(); ++i)
	{
		const dtMeshTile* tile = tmpNavMesh->getTile(i);
		if (!tile->header) continue;
		drawMeshTile();
	}*/
	drawMeshTile();
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

void ModuleNavigation::generateNavigability()
{
	//clean old info
	cleanValues();

	pointsUpdated = true;

	//declaring mesh box
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
		const float* v0 = &verts[tris[i*3] * 3];
		const float* v1 = &verts[tris[i*3 + 1] * 3];
		const float* v2 = &verts[tris[i*3 + 2] * 3];
		float e0[3], e1[3];
		for (int j = 0; j < 3; ++j)
		{
			e0[j] = v1[j] - v0[j];
			e1[j] = v2[j] - v0[j];
		}
		float* n = &normals[i*3];
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
/*
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
*/
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

void ModuleNavigation::drawMeshTile()
{
	
	/*if (pointsUpdated)
	{
		fillDrawPoints();
		pointsUpdated = false;
	}
	myPoint* renderIface = new myPoint();
	renderIface->drawPointList(points, nverts, false);*/
	for (int i = 0; i < nverts; ++i)
	{
		dd::point(ddVec3(verts[i * 3], verts[i * 3+1], verts[i * 3+2]), ddVec3(0,0,1), 10.0f);
	}
	
	
	/*dd::xzSquareGrid(-500.0f * 10, 500.0f * 10, 0.0f, 1.0f * 10, math::float3(0.65f, 0.65f, 0.65f));
	dtPolyRef base = mesh.getPolyRefBase(tile);

	int tileNum = mesh.decodePolyIdTile(base);
	const unsigned int tileColor = duIntToCol(tileNum, 128);

	dd->depthMask(false);

	dd->begin(DU_DRAW_TRIS);
	for (int i = 0; i < tile->header->polyCount; ++i)
	{
		const dtPoly* p = &tile->polys[i];
		if (p->getType() == DT_POLYTYPE_OFFMESH_CONNECTION)	// Skip off-mesh links.
			continue;

		const dtPolyDetail* pd = &tile->detailMeshes[i];

		unsigned int col;
		if (query && query->isInClosedList(base | (dtPolyRef)i))
			col = duRGBA(255, 196, 0, 64);
		else
		{
			if (flags & DU_DRAWNAVMESH_COLOR_TILES)
				col = tileColor;
			else
				col = duTransCol(dd->areaToCol(p->getArea()), 64);
		}

		for (int j = 0; j < pd->triCount; ++j)
		{
			const unsigned char* t = &tile->detailTris[(pd->triBase + j) * 4];
			for (int k = 0; k < 3; ++k)
			{
				if (t[k] < p->vertCount)
					dd->vertex(&tile->verts[p->verts[t[k]] * 3], col);
				else
					dd->vertex(&tile->detailVerts[(pd->vertBase + t[k] - p->vertCount) * 3], col);
			}
		}
	}
	dd->end();

	// Draw inter poly boundaries
	//drawPolyBoundaries(dd, tile, duRGBA(0, 48, 64, 32), 1.5f, true);

	// Draw outer poly boundaries
	//drawPolyBoundaries(dd, tile, duRGBA(0, 48, 64, 220), 2.5f, false);

	if (flags & DU_DRAWNAVMESH_OFFMESHCONS)
	{
		dd->begin(DU_DRAW_LINES, 2.0f);
		for (int i = 0; i < tile->header->polyCount; ++i)
		{
			const dtPoly* p = &tile->polys[i];
			if (p->getType() != DT_POLYTYPE_OFFMESH_CONNECTION)	// Skip regular polys.
				continue;

			unsigned int col, col2;
			if (query && query->isInClosedList(base | (dtPolyRef)i))
				col = duRGBA(255, 196, 0, 220);
			else
				col = duDarkenCol(duTransCol(dd->areaToCol(p->getArea()), 220));

			const dtOffMeshConnection* con = &tile->offMeshCons[i - tile->header->offMeshBase];
			const float* va = &tile->verts[p->verts[0] * 3];
			const float* vb = &tile->verts[p->verts[1] * 3];

			// Check to see if start and end end-points have links.
			bool startSet = false;
			bool endSet = false;
			for (unsigned int k = p->firstLink; k != DT_NULL_LINK; k = tile->links[k].next)
			{
				if (tile->links[k].edge == 0)
					startSet = true;
				if (tile->links[k].edge == 1)
					endSet = true;
			}

			// End points and their on-mesh locations.
			dd->vertex(va[0], va[1], va[2], col);
			dd->vertex(con->pos[0], con->pos[1], con->pos[2], col);
			col2 = startSet ? col : duRGBA(220, 32, 16, 196);
			duAppendCircle(dd, con->pos[0], con->pos[1] + 0.1f, con->pos[2], con->rad, col2);

			dd->vertex(vb[0], vb[1], vb[2], col);
			dd->vertex(con->pos[3], con->pos[4], con->pos[5], col);
			col2 = endSet ? col : duRGBA(220, 32, 16, 196);
			duAppendCircle(dd, con->pos[3], con->pos[4] + 0.1f, con->pos[5], con->rad, col2);

			// End point vertices.
			dd->vertex(con->pos[0], con->pos[1], con->pos[2], duRGBA(0, 48, 64, 196));
			dd->vertex(con->pos[0], con->pos[1] + 0.2f, con->pos[2], duRGBA(0, 48, 64, 196));

			dd->vertex(con->pos[3], con->pos[4], con->pos[5], duRGBA(0, 48, 64, 196));
			dd->vertex(con->pos[3], con->pos[4] + 0.2f, con->pos[5], duRGBA(0, 48, 64, 196));

			// Connection arc.
			duAppendArc(dd, con->pos[0], con->pos[1], con->pos[2], con->pos[3], con->pos[4], con->pos[5], 0.25f,
				(con->flags & 1) ? 0.6f : 0, 0.6f, col);
		}
		dd->end();
	}

	const unsigned int vcol = duRGBA(0, 0, 0, 196);
	dd->begin(DU_DRAW_POINTS, 3.0f);
	for (int i = 0; i < tile->header->vertCount; ++i)
	{
		const float* v = &tile->verts[i * 3];
		dd->vertex(v[0], v[1], v[2], vcol);
	}
	dd->end();

	dd->depthMask(true);*/
}

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

		dtQueryFilter m_filter;
		dtPolyRef m_startRef;
		dtPolyRef m_endRef;

		const int MAX_POLYS = 256;
		dtPolyRef m_polys[MAX_POLYS];
		dtPolyRef returnedPath[MAX_POLYS];
		float m_straightPath[MAX_POLYS * 3];
		int numStraightPaths;
		float  m_spos[3] = { pStart.X, pStart.Y, pStart.Z };
		float  m_epos[3] = { pEnd.X, pEnd.Y, pEnd.Z };
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
	*/

//On your event input positions
//std::vector<math::float3> lstPoints = ModuleNavigation->returnPath(vector3df_Start, vector3df_End);