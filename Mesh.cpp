#include "Mesh.h"
#include <errno.h>

/*
**	MESH 
*/
using namespace std;

// default constructor creates a horizontal plane or dimensions 1 x 1 centered on the origin
Mesh::Mesh()
{
	// Create vertices
	Vertex vertices[] = { 
		Vertex(glm::vec3(-0.5f ,0.0f ,-0.5f)),
		Vertex(glm::vec3(0.5f, 0.0f, -0.5f)),
		Vertex(glm::vec3(-0.5f ,0.0f ,0.5f)),
		Vertex(glm::vec3(0.5f, 0.0f, -0.5f)),
		Vertex(glm::vec3(-0.5f ,0.0f ,0.5f)),
		Vertex(glm::vec3(0.5f ,0.0f ,0.5f))
	};

	m_mode = GL_TRIANGLES;

	//create mesh
	initMesh(vertices, sizeof(vertices) / sizeof(vertices[0]));

	// initialise tansform matrices (identify)
	m_transform = new Transform();
	m_transform->SetDensity(1.0f, CUBE);
}

Mesh::Mesh(BasicShapeType shape)
{
	m_mode = GL_TRIANGLES;

	if (shape == Cube)
	{
		// Create vertices
		Vertex v[] = {
			Vertex(glm::vec3(-0.5f ,-0.5f ,-0.5f)),
			Vertex(glm::vec3(0.5f ,-0.5f ,-0.5f)),
			Vertex(glm::vec3(0.5f ,-0.5f ,0.5f)),
			Vertex(glm::vec3(-0.5f ,-0.5f ,0.5f)),
			Vertex(glm::vec3(-0.5f ,0.5f ,-0.5f)),
			Vertex(glm::vec3(0.5f ,0.5f ,-0.5f)),
			Vertex(glm::vec3(0.5f ,0.5f ,0.5f)),
			Vertex(glm::vec3(-0.5f ,0.5f ,0.5f)),
		};

		vector<Vertex> verts;

		verts.push_back(v[0]); verts.push_back(v[1]); verts.push_back(v[3]);
		verts.push_back(v[1]); verts.push_back(v[2]); verts.push_back(v[3]);

		for (int i = 0; i < 4; i++)
		{
			int a = i;
			int b = (i + 1) % 4;
			int c = ((i + 1) % 4) + 4;
			int d = (i % 4) + 4;

			verts.push_back(v[a]); verts.push_back(v[b]); verts.push_back(v[d]);
			verts.push_back(v[b]); verts.push_back(v[c]); verts.push_back(v[d]);
		}

		verts.push_back(v[4]); verts.push_back(v[5]); verts.push_back(v[7]);
		verts.push_back(v[5]); verts.push_back(v[6]); verts.push_back(v[7]);

		m_mode = GL_TRIANGLES;

		//create mesh
		initMesh(verts.data(), verts.size());

		// initialise tansform matrices (identify)
		m_transform = new Transform();
		m_transform->SetDensity(1.0f, CUBE);
	}
	else if (shape == Sphere)
	{
		vector<Triangle> tris;

		Vertex a = Vertex(glm::normalize(glm::vec3(1.0f, 1.0f, 1.0f)));
		Vertex b = Vertex(glm::normalize(glm::vec3(-1.0f, -1.0f, 1.0f)));
		Vertex c = Vertex(glm::normalize(glm::vec3(-1.0f, 1.0f, -1.0f)));
		Vertex d = Vertex(glm::normalize(glm::vec3(1.0f, -1.0f, -1.0f)));

		tris.push_back(Triangle(a, b, c));
		tris.push_back(Triangle(c, d, a));
		tris.push_back(Triangle(c, b, d));
		tris.push_back(Triangle(a, d, b));

		vector<Triangle> t = SubDivide(tris, 3);
		tris.clear();

		vector<Vertex> verts;
		for (int i = 0; i < t.size(); i++)
		{
			//t[i].GetVert(0).Get() /= 2.0f;
			//t[i].GetVert(1).Get() /= 2.0f;
			//t[i].GetVert(2).Get() /= 2.0f;
			verts.push_back(t[i].GetVert(0));
			verts.push_back(t[i].GetVert(1));
			verts.push_back(t[i].GetVert(2));
		}

		Vertex* v = verts.data();

		initMesh(v, verts.size());

		m_transform = new Transform();
		m_transform->SetDensity(1.0f, SPHERE);
	}
	
}

vector<Triangle> Mesh::SubDivide(vector<Triangle> in, int divisions)
{
	vector<Triangle> Itris = in;
	vector<vector<Triangle>> newTris;

	newTris.push_back(Itris);

	int passes = 0;

	while (passes < divisions)
	{
		vector<Triangle> tris;
		int curr = newTris.size() - 1;
		for (int i = 0; i < newTris[curr].size(); i++)
		{
			SubdivideTri(tris, newTris[curr][i]);
		}

		newTris.push_back(tris);

		passes++;
	}

	return newTris[newTris.size() - 1];
}

vector<Triangle> Mesh::CurrentDivide(vector<Triangle> in)
{
	vector<Triangle> result;

	for (int i = 0; i < in.size(); i++)
	{
		glm::vec3 a = in[i].GetVert(0).Get();
		glm::vec3 b = in[i].GetVert(1).Get();
		glm::vec3 c = in[i].GetVert(2).Get();

		glm::vec3 p1 = glm::normalize(a + b);
		glm::vec3 p2 = glm::normalize(b + c);
		glm::vec3 p3 = glm::normalize(c + a);

		result.push_back(Triangle(a, p1, p3));
		result.push_back(Triangle(b, p2, p1));
		result.push_back(Triangle(c, p3, p2));
		result.push_back(Triangle(p1, p2, p3));
	}

	return result;
}
void Mesh::SubdivideTri(vector<Triangle> &tris, Triangle &t)
{
	glm::vec3 a = t.GetVert(0).Get();
	glm::vec3 b = t.GetVert(1).Get();
	glm::vec3 c = t.GetVert(2).Get();

	glm::vec3 p1 = (a + b) / 2.0f;
	glm::vec3 p2 = (b + c) / 2.0f;
	glm::vec3 p3 = (c + a) / 2.0f;
	glm::vec3 m = (a + b + c) / 3.0f;

	p1 = glm::normalize(p1);
	p2 = glm::normalize(p2);
	p3 = glm::normalize(p3);
	m = glm::normalize(m);

	tris.push_back(Triangle(a, p1, p3));
	tris.push_back(Triangle(b, p2, p1));
	tris.push_back(Triangle(c, p3, p2));
	tris.push_back(Triangle(p1, p2, p3));
}

Mesh::~Mesh()
{

}

// initialise transform matrices to identity
void Mesh::initTransform() 
{
	m_transform = new Transform();
}

// create mesh from vertices
void Mesh::initMesh(Vertex vertices[], unsigned int numVertices) 
{
	m_numIndices = numVertices;

	glGenVertexArrays(1, &m_vertexArrayObject);
	glBindVertexArray(m_vertexArrayObject);
	glGenBuffers(NUM_BUFFERS, m_vertexArrayBuffers);
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexArrayBuffers[POSITION_VB]);
	glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(vertices[0]), vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glBindVertexArray(0);
}

// create mesh from vertices and normals
void Mesh::initMesh(Vertex vertices[], unsigned int numVertices, glm::vec3 normals[], unsigned int numNorms) 
{
	m_numIndices = numVertices;

	glGenVertexArrays(1, &m_vertexArrayObject);
	glBindVertexArray(m_vertexArrayObject);
	glGenBuffers(NUM_BUFFERS, m_vertexArrayBuffers);

	glBindBuffer(GL_ARRAY_BUFFER, m_vertexArrayBuffers[POSITION_VB]);
	glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(vertices[0]), vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_vertexArrayBuffers[NORMAL_VB]);
	glBufferData(GL_ARRAY_BUFFER, numNorms * sizeof(normals[0]), normals, GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindVertexArray(0);
}

void Mesh::SetUniforms()
{
	// Get the uniform locations
	GLint modelLoc = glGetUniformLocation(m_shader.Program, "model");

	// Pass the model matrix to the shader
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(m_transform->GetFullMat()));

	glBindVertexArray(getVertexArrayObject());
	glDrawArrays(getMode(), 0, getNumIndices());
}