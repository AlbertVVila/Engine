#include "Mesh.h"

#include "Globals.h"
#include <assert.h>
#include "GL/glew.h"
#include "JSON.h"
#include "Geometry/Triangle.h"
#include "Geometry/LineSegment.h"

Mesh::Mesh()
{
}


Mesh::~Mesh()
{
}

void Mesh::SetMesh(const char * meshData, unsigned uid)
{
	assert(meshData != nullptr);
	DeleteBuffers();
	const char *data = meshData;
	
	unsigned int numIndices = *(int*)meshData;
	meshData += sizeof(int);
	
	unsigned int numVertices = *(int*)meshData;
	meshData += sizeof(int);
	
	float* vertices = (float*)meshData;
	meshData += sizeof(float) * 3 * numVertices;
		
	bool hasNormals = (bool*)meshData;
	meshData += sizeof(bool);
	
	float* normals = nullptr;
	if (hasNormals)
	{
		normals = (float*)meshData;
		meshData += sizeof(float) * 3 * numVertices;
	}
	
	bool hasTexCoords = (bool*)meshData;
	meshData += sizeof(bool);
	
	float* texCoords = nullptr;
	if (hasTexCoords)
	{
		texCoords = (float*)meshData;
		meshData += sizeof(float) * 2 * numVertices;
	}
	
	int* indices = (int*)meshData;
	meshData += sizeof(int) * numIndices;
	// VAO Creation
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	
	// Buffer Creation with vertex data
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	unsigned bufferSize = sizeof(GLfloat)*numVertices * 3;
	if (hasNormals) bufferSize += sizeof(GLfloat)*numVertices * 3;
	if (hasTexCoords) bufferSize+= sizeof(GLfloat)*numVertices * 2;
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
	glGenBuffers(1, &EBO);
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
	
	UID = uid;
	this->numIndices = numIndices;
	this->numVertices = numVertices;
	this->vertices = vertices; //TODO: !!should copy before delete
	this->indices = indices;
	ComputeBBox();
}

void Mesh::Draw(unsigned int shaderProgram) const
{
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, 0);
	
	// We disable VAO
	glBindVertexArray(0);
	// We disable Texture
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

void Mesh::DeleteBuffers()
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
}

bool Mesh::Intersects(const LineSegment &line, float* distance)
{
	bool intersects = false;
	*distance = 0.f;
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
