#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtx/orthonormalize.hpp>
#include <glm/gtx/matrix_cross_product.hpp>
#include <string>
#include <vector>
#include "OBJloader.h"
#include "Shader.h"
#include <math.h>

#include "Transform.h"

/* 
** VERTEX CLASS 
*/
class Vertex 
{
public:
	Vertex()
	{
		this->coord = glm::vec3(0.0f, 0.0f, 0.0f);
	}
	Vertex(const glm::vec3& coord) 
	{
		this->coord = coord;
	}

	glm::vec3& Get() { return coord; }
protected:
private:
	glm::vec3 coord;
};

class Triangle
{
public:
	Triangle(Vertex a, Vertex b, Vertex c) : m_a(a), m_b(b), m_c(c) {}

	Vertex GetVert(int i) 
	{
		if (i == 0)
			return m_a;
		else if (i == 1)
			return m_b;
		else
			return m_c;
	}
private:
	Vertex m_a;
	Vertex m_b;
	Vertex m_c;
};

enum MeshBufferPositions
{
	POSITION_VB,
	TEXCOORD_VB,
	NORMAL_VB,
	INDEX_VB
};

enum BasicShapeType
{
	Cube,
	Sphere,
	Tetrahedron
};


class Mesh
{
public:
	Mesh();
	Mesh(BasicShapeType shape);
	virtual ~Mesh();

	virtual glm::mat4 getModel() { return m_transform->GetFullMat(); }
	Transform* GetTransform() { return m_transform; }
	Shader getShader() const { return m_shader; }
	GLuint getVertexArrayObject() const{ return m_vertexArrayObject; }
	unsigned int getNumIndices() const{ return m_numIndices; }
	GLenum getMode() const { return m_mode; }

	void setPos(glm::vec3 p) { m_transform->SetPos(p); }
	void setPos(int i, float p) { m_transform->SetPos(i, p); }

	// assign shader to mesh
	void setShader(const Shader &shader) {
		m_shader = shader;
		m_shader.Use();
	}

	// initialise transform matrices to identity
	void initTransform();
	// create mesh from vertices
	void initMesh(Vertex vertices[], unsigned int numVertices);
	// create mesh from vertices and normals
	void initMesh(Vertex vertices[], unsigned int numVertices, glm::vec3 normals[], unsigned int numNorms);
	// create mesh from model (typically loaded from a file)
	void InitMesh(const IndexedModel& model);
	// load .obj file
	void loadOBJ(const char * path,
		std::vector < glm::vec3 > & out_vertices,
		std::vector < glm::vec2 > & out_uvs,
		std::vector < glm::vec3 > & out_normals
	);
	virtual void SetUniforms();
protected:
	enum {
		POSITION_VB,
		NUM_BUFFERS
	};

	std::vector<Triangle> SubDivide(std::vector<Triangle> in, int divisions);
	std::vector<Triangle> CurrentDivide(std::vector<Triangle> in);
	void SubdivideTri(std::vector<Triangle> &tris, Triangle &t);

	GLuint m_vertexArrayObject;
	GLuint m_vertexArrayBuffers[NUM_BUFFERS];
	unsigned int m_numIndices;
	GLenum m_mode;

	Transform* m_transform;

	Shader m_shader;
};


