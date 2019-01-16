#include "Mesh.h"

#include "Globals.h"
#include <assert.h>
#include "GL/glew.h"
#include "JSON.h"
#include "Geometry/Triangle.h"
#include "Geometry/LineSegment.h"
#include "Math/MathConstants.h"
#include "Math/float4x4.h"

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
	if (indices != nullptr)
	{
		RELEASE_ARRAY(indices);
	}
	if (vertices != nullptr)
	{
		RELEASE_ARRAY(vertices);
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

void Mesh::SetMesh(const char * meshData, unsigned uid)
{
	assert(meshData != nullptr);
	if (meshData == nullptr) return;

	const char *data = meshData;
	
	unsigned int numIndices = *(int*)meshData;
	meshData += sizeof(int);
	
	unsigned int numVertices = *(int*)meshData;
	meshData += sizeof(int);
	
	float* vertices = (float*)meshData;
	meshData += sizeof(float) * 3 * numVertices;
		
	bool hasNormals = *(bool*)meshData;
	meshData += sizeof(bool);
	
	float* normals = nullptr;
	if (hasNormals)
	{
		normals = (float*)meshData;
		meshData += sizeof(float) * 3 * numVertices;
	}
	
	bool hasTexCoords = *(bool*)meshData;
	meshData += sizeof(bool);
	
	float* texCoords = nullptr;
	if (hasTexCoords)
	{
		texCoords = (float*)meshData;
		meshData += sizeof(float) * 2 * numVertices;
	}
	
	int* indices = (int*)meshData;
	meshData += sizeof(int) * numIndices;
	
	UID = uid;
	this->numIndices = numIndices;
	this->numVertices = numVertices;

	this->vertices = new float[numVertices*3];
	this->indices = new int[numIndices];
	memcpy(this->vertices, vertices, numVertices * sizeof(float) * 3);
	memcpy(this->indices, indices, numIndices * sizeof(int));
	ComputeBBox();
	SetMeshBuffers(hasNormals, hasTexCoords, normals, texCoords);
	SetBboxBuffers();

	RELEASE_ARRAY(data);
}

void Mesh::SetMeshBuffers(bool hasNormals, bool hasTexCoords, float* normals, float* texCoords)
{
	// VAO Creation
	if (VAO == 0)
	{
		glGenVertexArrays(1, &VAO);
	}
	glBindVertexArray(VAO);

	// Buffer Creation with vertex data
	if (VBO == 0)
	{
		glGenBuffers(1, &VBO);
	}
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	unsigned bufferSize = sizeof(GLfloat)*numVertices * 3;
	if (hasNormals) bufferSize += sizeof(GLfloat)*numVertices * 3;
	if (hasTexCoords) bufferSize += sizeof(GLfloat)*numVertices * 2;
	glBufferData(GL_ARRAY_BUFFER, bufferSize, NULL, GL_STATIC_DRAW);

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLfloat) * 3 * numVertices, vertices);

	if (hasNormals)
	{
		glBufferSubData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 3 * numVertices, sizeof(GLfloat) * 3 * numVertices, normals);
	}
	unsigned offsetTexCoords = 0;
	if (hasTexCoords)
	{
		offsetTexCoords = hasNormals ? sizeof(GLfloat) * 6 * numVertices : sizeof(GLfloat) * 3 * numVertices;
		glBufferSubData(GL_ARRAY_BUFFER, offsetTexCoords, sizeof(GLfloat) * 2 * numVertices, texCoords);
	}

	//Buffer creation with indices
	if (EBO == 0)
	{
		glGenBuffers(1, &EBO);
	}
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)*numIndices, indices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(
		0,                  // attribute 0
		3,                  // number of componentes (3 floats)
		GL_FLOAT,           // data type
		GL_FALSE,           // should be normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);
	if (hasNormals)
	{
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(
			1,                  // attribute 1
			3,                  // number of componentes (3 floats)
			GL_FLOAT,           // data type
			GL_FALSE,           // should be normalized?
			0,                  // stride
			(void*)(sizeof(float) * 3 * numVertices)       // array buffer offset
		);
	}
	if (hasTexCoords)
	{
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(
			2,                  // attribute 2
			2,                  // number of componentes (2 floats)
			GL_FLOAT,           // data type
			GL_FALSE,           // should be normalized?
			0,                  // stride
			(void*)(offsetTexCoords)       // array buffer offset
		);
	}

	// Disable VAO
	glBindVertexArray(0);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);


	// Disable VBO and EBO
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Mesh::Draw(unsigned shaderProgram) const
{
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, 0);
	
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

void Mesh::ComputeBBox()
{
	float3 min, max;
	min = max = float3(vertices);
	
	for (unsigned i=0; i<numVertices*3; i+=3)
	{
		min.x = MIN(min.x, vertices[i]); //x
		min.y = MIN(min.y, vertices[i+1]); //y
		min.z = MIN(min.z, vertices[i+2]); //z
	
		max.x = MAX(max.x, vertices[i]);
		max.y = MAX(max.y, vertices[i+1]);
		max.z = MAX(max.z, vertices[i+2]);
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
	for (unsigned i = 0; i < numIndices; i+=3) //foreach triangle
	{
		float3 v0(&vertices[3*indices[i]]);
		float3 v1(&vertices[3*indices[i+1]]);
		float3 v2(&vertices[3*indices[i+2]]);
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
