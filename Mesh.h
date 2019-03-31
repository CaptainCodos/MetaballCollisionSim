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
class Vertex {
public:
	Vertex()
	{
		this->coord = glm::vec3(0.0f, 0.0f, 0.0f);
	}
	Vertex(const glm::vec3& coord) {
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

/* 
** MESH CLASS 
*/
class Mesh
{
public:
	/*
	** CONSTRUCTORS
	*/

	// default constructor creates a horizontal plane or dimensions 1 x 1 centered on the origin
	Mesh();
	Mesh(BasicShapeType shape);
	// create mesh from a .obj file
	Mesh(const std::string& fileName);
	virtual ~Mesh();


	/*
	** GET AND SET METHODS
	*/

	// getModel computes the model matrix any time it is required
	//glm::vec3 getPos() const { return getTranslate()[3]; }
	virtual glm::mat4 getModel() { return m_transform->GetFullMat(); }
	/*glm::mat4 getTranslate() const{ return m_translate; }
	glm::mat4 getRotate() const{ return m_rotate; }
	glm::mat4 getScale() const{ return m_scale; }
	glm::vec3 getScaleVec() const { return glm::vec3(m_scale[0][0], m_scale[1][1], m_scale[2][2]); }*/
	Transform* GetTransform() { return m_transform; }
	Shader getShader() const { return m_shader; }
	GLuint getVertexArrayObject() const{ return m_vertexArrayObject; }
	unsigned int getNumIndices() const{ return m_numIndices; }
	GLenum getMode() const { return m_mode; }

	// set position of mesh center to specified 3D position vector
	void setPos(glm::vec3 p) { m_transform->SetPos(p); }
	// set i_th coordinate of mesh center to float p (x: i=0, y: i=1, z: i=2)
	void setPos(int i, float p) { m_transform->SetPos(i, p); }

	//// set rotation matrix
	//void setRotate(const glm::mat4 &mat) { m_rotate = mat; }

	// allocate shader to mesh
	void setShader(const Shader &shader) {
		m_shader = shader;
		m_shader.Use();
	}


	/* 
	** INITIALISATION AND UTILITY METHODS
	*/

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


	/*
	** TRANSFORMATION METHODS
	*/
	
	//// translate mesh by a vector
	//void translate(const glm::vec3 &vect);
	//// rotate mesh by a vector
	//void rotate(const float &angle, const glm::vec3 &vect);
	//// scale mesh by a vector
	//void scale(const glm::vec3 &vect);

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

	/*glm::mat4 m_translate;
	glm::mat4 m_rotate;
	glm::mat4 m_scale;*/

	Shader m_shader;
};


