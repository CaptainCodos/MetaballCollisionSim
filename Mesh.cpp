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

		/*vector<glm::vec3> vs{ glm::vec3(0.0f, 0.0f, 0.3f), glm::vec3(-0.5f, 0.3333f, -0.3333f), glm::vec3(0.5f, 0.3333f, -0.3333f),
				 glm::vec3(0.0f, -0.3333f, -0.333333f) };

		for (int i = 0; i < vs.size(); i++) {
			vs[i] = normalize(vs[i]);
		}

		tris.push_back(Triangle(vs[0], vs[1], vs[2]));
		tris.push_back(Triangle(vs[0], vs[2], vs[3]));
		tris.push_back(Triangle(vs[0], vs[3], vs[1]));
		tris.push_back(Triangle(vs[3], vs[2], vs[1]));*/
		Vertex a = Vertex(glm::normalize(glm::vec3(1.0f, 1.0f, 1.0f)));
		Vertex b = Vertex(glm::normalize(glm::vec3(-1.0f, -1.0f, 1.0f)));
		Vertex c = Vertex(glm::normalize(glm::vec3(-1.0f, 1.0f, -1.0f)));
		Vertex d = Vertex(glm::normalize(glm::vec3(1.0f, -1.0f, -1.0f)));
		//a.Get() = glm::normalize(a.Get());
		//b.Get() = glm::normalize(b.Get());
		//c.Get() = glm::normalize(c.Get());
		//d.Get() = glm::normalize(d.Get());

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

		// initialise tansform matrices (identify)
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

		//oldTris.clear();
		//oldTris = newTris;
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

		//glm::vec3 d = (a + b + c) / 3.0f;
		//glm::vec3 d = glm::normalize(a + b + c);
		glm::vec3 p1 = glm::normalize(a + b);
		glm::vec3 p2 = glm::normalize(b + c);
		glm::vec3 p3 = glm::normalize(c + a);
		//glm::vec3 d = glm::normalize(a + b + c);

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


// create mesh from a .obj file
Mesh::Mesh(const std::string& fileName)
{
	m_mode = GL_TRIANGLES;

	InitMesh(OBJModel(fileName).ToIndexedModel());
	initTransform();
}

Mesh::~Mesh()
{
	//delete m_transform;
}


/* 
** INIT METHODS 
*/

// initialise transform matrices to identity
void Mesh::initTransform() {
	m_transform = new Transform();
	/*m_translate = glm::mat4(1.0f);
	m_rotate = glm::mat4(1.0f);
	m_scale = glm::mat4(1.0f);*/
}

// create mesh from vertices
void Mesh::initMesh(Vertex vertices[], unsigned int numVertices) {
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
void Mesh::initMesh(Vertex vertices[], unsigned int numVertices, glm::vec3 normals[], unsigned int numNorms) {
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


// create mesh from model (typically loaded from file)
void Mesh::InitMesh(const IndexedModel& model)
{
	m_numIndices = model.indices.size();

	glGenVertexArrays(1, &m_vertexArrayObject);
	glBindVertexArray(m_vertexArrayObject);

	glGenBuffers(NUM_BUFFERS, m_vertexArrayBuffers);

	glBindBuffer(GL_ARRAY_BUFFER, m_vertexArrayBuffers[POSITION_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(model.positions[0]) * model.positions.size(), &model.positions[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_vertexArrayBuffers[TEXCOORD_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(model.texCoords[0]) * model.texCoords.size(), &model.texCoords[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_vertexArrayBuffers[NORMAL_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(model.normals[0]) * model.normals.size(), &model.normals[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vertexArrayBuffers[INDEX_VB]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(model.indices[0]) * model.indices.size(), &model.indices[0], GL_STATIC_DRAW);

	glBindVertexArray(0);
}

// load .obj file
void Mesh::loadOBJ(const char * path,
	std::vector < glm::vec3 > & out_vertices,
	std::vector < glm::vec2 > & out_uvs,
	std::vector < glm::vec3 > & out_normals
) {
	std::vector< unsigned int > vertexIndices, uvIndices, normalIndices;
	std::vector< glm::vec3 > temp_vertices;
	std::vector< glm::vec2 > temp_uvs;
	std::vector< glm::vec3 > temp_normals;

	FILE * stream;
	errno_t err;

	err = fopen_s(&stream, path, "r");

	if (err != 0)
	{
		printf("The file was not opened\n");
	}
	else{
		while (1) {

			char lineHeader[128];
			// read the first word of the line
			int res = fscanf(stream, "%s", lineHeader);
			if (res == EOF)
				break; // EOF = End Of File. Quit the loop.

				   // else : parse lineHeader
			if (strcmp(lineHeader, "v") == 0) {
				glm::vec3 vertex;
				fscanf_s(stream, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
				temp_vertices.push_back(vertex);
			}
			else if (strcmp(lineHeader, "vt") == 0) {
				glm::vec2 uv;
				fscanf_s(stream, "%f %f\n", &uv.x, &uv.y);
				temp_uvs.push_back(uv);
			}
			else if (strcmp(lineHeader, "vn") == 0) {
				glm::vec3 normal;
				fscanf_s(stream, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
				temp_normals.push_back(normal);
			}
			else if (strcmp(lineHeader, "f") == 0) {
				std::string vertex1, vertex2, vertex3;
				unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
				int matches = fscanf_s(stream, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
				if (matches != 9) {
					std::cerr << "file can't be read by parser" << std::endl;
				}
				vertexIndices.push_back(vertexIndex[0]);
				vertexIndices.push_back(vertexIndex[1]);
				vertexIndices.push_back(vertexIndex[2]);
				uvIndices.push_back(uvIndex[0]);
				uvIndices.push_back(uvIndex[1]);
				uvIndices.push_back(uvIndex[2]);
				normalIndices.push_back(normalIndex[0]);
				normalIndices.push_back(normalIndex[1]);
				normalIndices.push_back(normalIndex[2]);
			}
		}
	}
	// For each vertex of each triangle
	for (unsigned int i = 0; i < vertexIndices.size(); i++) {
		unsigned int vertexIndex = vertexIndices[i];
		glm::vec3 vertex = temp_vertices[vertexIndex - 1];
		out_vertices.push_back(vertex);
	}

}



/*
** TRANSFORMATION METHODS
*/

// translate
//void Mesh::translate(const glm::vec3 &vect) {
//	m_translate = glm::translate(m_translate, vect);
//}
//
//// rotate
//void Mesh::rotate(const float &angle, const glm::vec3 &vect) {
//	m_rotate = glm::rotate(m_rotate, angle, vect);
//}
//
//// scale
//void Mesh::scale(const glm::vec3 &vect) {
//	m_scale = glm::scale(m_scale, vect);
//}

void Mesh::SetUniforms()
{
	// Get the uniform locations
	GLint modelLoc = glGetUniformLocation(m_shader.Program, "model");

	// Pass the model matrix to the shader
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(m_transform->GetFullMat()));

	glBindVertexArray(getVertexArrayObject());
	glDrawArrays(getMode(), 0, getNumIndices());
}