#include "Mesh.h"

#include "Globals.h"
#include <assert.h>
#include "GL/glew.h"
#include "JSON.h"
#include "Geometry/Triangle.h"
#include "Geometry/LineSegment.h"
#include "Math/MathConstants.h"
#include "Math/float4x4.h"
#include "Math/float2.h"
#include <stack>

#include "ComponentRenderer.h"
#include "GameObject.h"

Mesh::Mesh()
{
}


Mesh::~Mesh()
{
	if (VAO != 0)
	{
		glDeleteVertexArrays(1, &VAO);
	}
	if (VBO != 0)
	{
		glDeleteBuffers(1, &VBO);
	}
	if (EBO != 0)
	{
		glDeleteBuffers(1, &EBO);
	}	
	if (VAObox != 0)
	{
		glDeleteVertexArrays(1, &VAObox);
	}
	if (VBObox != 0)
	{
		glDeleteBuffers(1, &VBObox);
	}
	if (EBObox != 0)
	{
		glDeleteBuffers(1, &EBObox);
	}
}

void Mesh::SetMesh(const char* meshData, unsigned uid)
{
	assert(meshData != nullptr);
	if (meshData == nullptr) return;

	unsigned numIndices = *(int*)meshData;
	meshData += sizeof(int);
	
	unsigned numVertices = *(int*)meshData;
	meshData += sizeof(int);
	
	float* vertices = (float*)meshData;
	meshData += sizeof(float) * 3 * numVertices;
		
	bool hasNormals = *(bool*)meshData;
	meshData += sizeof(bool);
	
	float* normals = nullptr;
	if (hasNormals)
	{
		normals = (float*)meshData;
		int nNormals = sizeof(float) * 3 * numVertices;
		meshData += nNormals;
		meshNormals.resize(numVertices);
		memcpy(&meshNormals[0], normals, nNormals);
	}
	
	bool hasTexCoords = *(bool*)meshData;
	meshData += sizeof(bool);
	
	float* texCoords = nullptr;
	if (hasTexCoords)
	{
		texCoords = (float*)meshData;
		int nTCoords = sizeof(float) * 2 * numVertices;
		meshData += nTCoords;
		meshTexCoords.resize(numVertices * 2);
		memcpy(&meshTexCoords[0], texCoords, nTCoords);
	}
	
	int* indices = (int*)meshData;
	meshData += sizeof(int) * numIndices;
	
	bool hasTangents = *(bool*)meshData;
	meshData += sizeof(bool);

	if (hasTangents)
	{
		int nTangents = sizeof(float3) * numVertices;
		meshTangents.resize(numVertices);
		memcpy(&meshTangents[0], meshData, nTangents);
		meshData += nTangents;
	}

	unsigned numBones = *(unsigned*)meshData;
	meshData += sizeof(unsigned);

	char boneName[MAX_BONE_NAME_LENGTH];

	bindBones.resize(numBones);
	bindBoneVertexAttaches.resize(numVertices);
	bindWeightVertexAttaches.resize(numVertices);

	for (unsigned i = 0u; i < numBones; ++i)
	{
		memcpy(&boneName[0], meshData, MAX_BONE_NAME_LENGTH);
		meshData += MAX_BONE_NAME_LENGTH;
		bindBones[i].name = boneName;
		memcpy(&bindBones[i].transform[0][0], meshData, sizeof(math::float4x4));
		meshData += sizeof(math::float4x4);

		unsigned numWeights;
		memcpy(&numWeights, meshData, sizeof(unsigned));
		meshData += sizeof(unsigned);

		for (unsigned j = 0u; j < numWeights; ++j)
		{
			unsigned vertex;
			memcpy(&vertex, meshData, sizeof(unsigned));
			meshData += sizeof(unsigned);

			float weight;
			memcpy(&weight, meshData, sizeof(float));
			meshData += sizeof(float);

			assert(vertex < numVertices);

			unsigned freeSlot = 0u;

			while (freeSlot < MAX_WEIGHTS_PER_BONE && bindWeightVertexAttaches[vertex].weight[freeSlot++] > 0); // Find empty slot

			if (freeSlot <= MAX_WEIGHTS_PER_BONE)
			{
				freeSlot--;
				bindBoneVertexAttaches[vertex].boneID[freeSlot] = i;
				bindWeightVertexAttaches[vertex].weight[freeSlot] = weight;
			}
		}
	}

	unsigned boneWeightCorrected = 0u;
	
	for (unsigned c = 0u; c < bindWeightVertexAttaches.size(); ++c)
	{
		float totalWeight = 0.f;
		for (unsigned k = 0u; k < MAX_WEIGHTS_PER_BONE; ++k)
		{
			totalWeight += bindWeightVertexAttaches[c].weight[k];
		}

		if (totalWeight > 1.f)
		{
			++boneWeightCorrected;
			for (unsigned k = 0u; k < MAX_WEIGHTS_PER_BONE; ++k)
			{
				bindWeightVertexAttaches[c].weight[k] /= totalWeight;
			}
		}
	
	}

	if (boneWeightCorrected > 0u)
	{
		LOG("Corrected %d vertex weights", boneWeightCorrected);
	}
	UID = uid;
	
	meshVertices.resize(numVertices);
	meshIndices.resize(numIndices);
	memcpy(&meshVertices[0], vertices, numVertices * sizeof(math::float3));
	memcpy(&meshIndices[0], indices, numIndices * sizeof(unsigned));

	ComputeBBox();
	SetMeshBuffers();
	SetBboxBuffers();

}

void Mesh::ProcessVertexTangent(const float vIndex1, const float vIndex2, const float vIndex3)
{
	math::float3 tangent;

	math::float2 UV = math::float2(meshTexCoords[vIndex1 * 2], meshTexCoords[vIndex1 * 2 + 1]);
	math::float2 UV1 = math::float2(meshTexCoords[vIndex2 * 2], meshTexCoords[vIndex2 * 2 + 1]);
	math::float2 UV2 = math::float2(meshTexCoords[vIndex3 * 2], meshTexCoords[vIndex3 * 2 + 1]);

	math::float2 deltaUV1 = UV1 - UV;
	math::float2 deltaUV2 = UV2 - UV;

	math::float3 edge1 = meshVertices[vIndex2] - meshVertices[vIndex1];
	math::float3 edge2 = meshVertices[vIndex3] - meshVertices[vIndex1];

	float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

	tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
	tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
	tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
	tangent = tangent.Normalized();

	meshTangents[vIndex1] = meshTangents[vIndex1] + tangent;
}

void Mesh::CalculateTangents()
{
	assert(meshIndices.size() % 3 == 0); //not triangulated
	if (meshTexCoords.size() == 0)
	{
		LOG("Unnable to set tangents - Not found in mesh & the mesh doesn't have texture coordinates.");
		return;
	}
	meshTangents.resize(meshVertices.size(), math::float3::zero);

	for (unsigned i = 0u; i < meshIndices.size(); i += 3) //for all the triangles of the mesh
	{
		//calculate tangents for each vertex of the triangle
		//V1
		unsigned vIndex1 = meshIndices[i];
		unsigned vIndex2 = meshIndices[i + 1];
		unsigned vIndex3 = meshIndices[i + 2];
		ProcessVertexTangent(vIndex1, vIndex2, vIndex3);
		//V2
		vIndex1 = meshIndices[i + 1];
		vIndex2 = meshIndices[i];
		vIndex3 = meshIndices[i + 2];
		ProcessVertexTangent(vIndex1, vIndex2, vIndex3);
		//V3
		vIndex1 = meshIndices[i + 2];
		vIndex2 = meshIndices[i + 1];
		vIndex3 = meshIndices[i];
		ProcessVertexTangent(vIndex1, vIndex2, vIndex3);

	}

	for (unsigned i = 0u; i < meshVertices.size(); ++i)
	{
		meshTangents[i] = meshTangents[i].Normalized();
	}
}


void Mesh::SetMeshBuffers()
{
	if (meshTangents.size() == 0) //if the mesh don't have tangents -> calculate them
		CalculateTangents();
	
	unsigned offsetTexCoords = meshVertices.size() * sizeof(math::float3);
	unsigned offsetNormals = offsetTexCoords + (meshTexCoords.size() * sizeof(float) * 2);
	unsigned offsetTangents = offsetNormals + (meshNormals.size() * sizeof(math::float3));
	unsigned offsetBones = offsetTangents + (bindBoneVertexAttaches.size() * sizeof(int) * 4);
	unsigned offsetWeights = offsetBones + (bindBoneVertexAttaches.size() * sizeof(float) * 4);
	
	unsigned totalSize = offsetWeights + (meshTangents.size() * sizeof(float) * 4);

	if (VAO == 0)
	{
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);
	}
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, totalSize, NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, offsetTexCoords, &meshVertices[0]);
	if (meshTexCoords.size() > 0)
		glBufferSubData(GL_ARRAY_BUFFER, offsetTexCoords, sizeof(float) * meshTexCoords.size(), &meshTexCoords[0]);

	if (meshNormals.size() > 0)
		glBufferSubData(GL_ARRAY_BUFFER, offsetNormals, sizeof(math::float3) * meshNormals.size(), &meshNormals[0]);

	if (meshTangents.size() > 0)
		glBufferSubData(GL_ARRAY_BUFFER, offsetTangents, sizeof(math::float3) * meshTangents.size(), &meshTangents[0]);

	if (bindBoneVertexAttaches.size() > 0)
	{
		glBufferSubData(GL_ARRAY_BUFFER, offsetBones, sizeof(int) * 4 * bindBoneVertexAttaches.size(), &bindBoneVertexAttaches[0]);
		glBufferSubData(GL_ARRAY_BUFFER, offsetWeights, sizeof(float) * 4 * bindWeightVertexAttaches.size(), &bindWeightVertexAttaches[0]);
	}

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned) * meshIndices.size(), &meshIndices[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)offsetTexCoords);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)offsetNormals);
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, (void*)offsetTangents);


	if (bindBones.size() != 0)
	{
		glEnableVertexAttribArray(4);
		glVertexAttribIPointer(4, 4, GL_UNSIGNED_INT, 0, (void*)offsetBones);
		glEnableVertexAttribArray(5);
		glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 0, (void*)offsetWeights);
	}
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	if (VAO == 0 || VBO == 0 || EBO == 0)
	{
		LOG("Error sending mesh to GPU");
	}
}

void Mesh::Draw(unsigned shaderProgram) const
{
	if (bindBones.size() > 0)
	{
		std::vector<math::float4x4> palette(bindBones.size()); //TODO: Declare on .h
		unsigned i = 0u;
		for (BindBone bb : bindBones)
		{
			palette[i++] = bb.go->GetGlobalTransform() * bb.transform;
		}
		
		glUniformMatrix4fv(glGetUniformLocation(shaderProgram,
			"palette"), bindBones.size(), GL_TRUE, palette[0].ptr());
	}
	glBindVertexArray(VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glDrawElements(GL_TRIANGLES, meshIndices.size(), GL_UNSIGNED_INT, 0);
	
	// We disable VAO
	glBindVertexArray(0);
}

void Mesh::SetBboxBuffers()
{
	// VAO Creation
	if (VAObox == 0)
	{
		glGenVertexArrays(1, &VAObox);
	}
	glBindVertexArray(VAObox);

	// Buffer Creation with vertex data
	if (VBObox == 0)
	{
		glGenBuffers(1, &VBObox);
	}

	GLfloat boxvertices[] = {
		-0.5, -0.5, -0.5, 1.0,
		0.5, -0.5, -0.5, 1.0,
		0.5,  0.5, -0.5, 1.0,
		-0.5,  0.5, -0.5, 1.0,
		-0.5, -0.5,  0.5, 1.0,
		0.5, -0.5,  0.5, 1.0,
		0.5,  0.5,  0.5, 1.0,
		-0.5,  0.5,  0.5, 1.0,
	};
	glBindBuffer(GL_ARRAY_BUFFER, VBObox);
	glBufferData(GL_ARRAY_BUFFER, sizeof(boxvertices), boxvertices, GL_STATIC_DRAW);

	GLushort elements[] = {
		0, 1, 2, 3,
		4, 5, 6, 7,
		0, 4, 1, 5, 2, 6, 3, 7
	};
	if (EBObox == 0)
	{
		glGenBuffers(1, &EBObox);
	}
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBObox);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(
		0,  // attribute
		4,                  // number of elements per vertex, here (x,y,z,w)
		GL_FLOAT,           // the type of each element
		GL_FALSE,           // take our values as-is
		0,                  // no extra data between each position
		0                   // offset of first element
	);

	// Disable VAO
	glBindVertexArray(0);
	glDisableVertexAttribArray(0);

	// Disable VBO and EBO
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}


void Mesh::DrawBbox(unsigned shader, const AABB &globalBBOX) const
{
	if (globalBBOX.Volume() <= 0) return;
	glUseProgram(shader);

	float4x4 boxtransform = float4x4::FromTRS(globalBBOX.CenterPoint(), Quat::identity, globalBBOX.Size());
	glUniformMatrix4fv(glGetUniformLocation(shader,
		"model"), 1, GL_TRUE, &(boxtransform)[0][0]);

	float green[4] = { 0.0f, 1.0f, 0.0f, 1.0f };
	glUniform4fv(glGetUniformLocation(shader,
		"Vcolor"), 1, green);
	

	glLineWidth(4.f);
	glBindVertexArray(VAObox);
	glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_SHORT, 0);
	glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_SHORT, (GLvoid*)(4 * sizeof(GLushort)));
	glDrawElements(GL_LINES, 8, GL_UNSIGNED_SHORT, (GLvoid*)(8 * sizeof(GLushort)));
	glLineWidth(1.f);

	// We disable VAO
	glBindVertexArray(0);

	glUseProgram(0);
}

void Mesh::LinkBones(const ComponentRenderer* renderer)
{
	if (bindBones.size() == 0)
	{
		return;
	}
	unsigned linkedCount = 0u;

	for (unsigned i = 0u; i < bindBones.size(); ++i)
	{
		GameObject* node = renderer->gameobject;
		while (node != nullptr && !node->isBoneRoot)
		{
			node = node->parent;
		}
		
		if (node == nullptr)
		{
			return;
		}

		bool found = false;

		std::stack<GameObject*> S;
		S.push(node);

		while (!S.empty() && !found)
		{
			node = S.top();
			S.pop();
			if (node->name == bindBones[i].name)
			{
				found = true;
				bindBones[i].go = node;
				++linkedCount;
			}
			else
			{
				for (GameObject* go : node->children)
				{
					S.push(go);
				}
			}
		}
	}

	LOG("Linked %d bones from %s", linkedCount, renderer->gameobject->name.c_str());
}

void Mesh::ComputeBBox()
{
	math::float3 min, max;
	min = max = math::float3(meshVertices[0]);
	
	for (unsigned i=0u; i<meshVertices.size(); ++i)
	{
		min.x = MIN(min.x, meshVertices[i].x); //x
		min.y = MIN(min.y, meshVertices[i].y); //y
		min.z = MIN(min.z, meshVertices[i].z); //z
	
		max.x = MAX(max.x, meshVertices[i].x);
		max.y = MAX(max.y, meshVertices[i].y);
		max.z = MAX(max.z, meshVertices[i].z);
	}
	boundingBox.minPoint = min;
	boundingBox.maxPoint = max;
}

AABB Mesh::GetBoundingBox() const
{
	return boundingBox;
}

bool Mesh::Intersects(const LineSegment &line, float* distance)
{
	bool intersects = false;
	*distance = FLOAT_INF;
	for (unsigned i = 0; i < meshIndices.size(); i+=3) //foreach triangle
	{
		math::float3 v0(meshVertices[meshIndices[i]]);
		math::float3 v1(meshVertices[meshIndices[i+1]]);
		math::float3 v2(meshVertices[meshIndices[i+2]]);
		Triangle triangle(v0, v1, v2);
		float dist = -1.f;
		if (line.Intersects(triangle, &dist, nullptr))
		{
			intersects = true;
			*distance = MIN(*distance, dist);
		}
	}
	return intersects;
}
