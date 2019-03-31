#include "Metaball.h"

using namespace std;
using namespace glm;

Metaball::Metaball() : Mesh()
{
	m_mode = GL_POINTS;

	m_cSize = 0.25f;
	m_radius = 1.0f;

	/*string paths[] = {
		"resources//shaders//Vertex//metaball.vert",
		"",
		"",
		"resources//shaders//Geometry//metaball.geom",
		"resources//shaders//Fragment//metaball.frag" };*/

	int flags[] = {
	1,
	0,
	0,
	1,
	1 };

	//setShader(Shader(paths, flags));

	m_transform->SetDynamic(false);
	for (int i = 0; i < 3; i++)
	{
		m_transform->SetRotLock(i, true);
	}

	//UpdateVerts();
}

Metaball::~Metaball()
{
}

void Metaball::SetCellSize(float size)
{
	m_cSize = size;
	UpdateVerts();
}
void Metaball::SetRadius(float rad)
{
	m_radius = rad;
	UpdateVerts();
}
void Metaball::CancelImpulses()
{
	m_transform->RemoveImpulses();
}

void Metaball::UpdateVerts()
{
	int cells = (m_radius * 8.0f) / m_cSize;
	float half = m_radius * 4.0f;

	const int size = cells * cells * cells;
	vector<Vertex> verts;
	vec3 offset = vec3(half, half, half);

	for (int x = 0; x < cells; x++)
	{
		for (int y = 0; y < cells; y++)
		{
			for (int z = 0; z < cells; z++)
			{
				verts.push_back(Vertex(vec3((x * m_cSize) - half, (y * m_cSize) - half, (z * m_cSize) - half)));
			}
		}
	}

	Vertex* allVerts = verts.data();


	//create mesh
	initMesh(verts.data(), verts.size());
}

//array<Vertex> Metaball::ConvertVector(vector<Vertex> &verts, const int &size)
//{
//
//}

void Metaball::UpdateMe(float dt)
{
	m_transform->Update(dt);

	m_snapped = mat4(1.0f);

	vec3 sP = m_transform->GetPosVec();
	
	for (int i = 0; i < 3; i++)
	{
		sP[i] /= m_cSize;
		sP[i] = std::round(sP[i]);
		sP[i] *= m_cSize;

		m_snapped[3][i] = sP[i];
	}
}

void Metaball::SetUniforms()
{
	//Mesh::SetUniforms();

	// Get the uniform locations
	GLint modelLoc = glGetUniformLocation(m_shader.Program, "model");

	// Pass the model matrix to the shader
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(getModel()));

	// Get the uniform locations
	GLint MLoc = glGetUniformLocation(m_shader.Program, "M");
	GLint thresholdLoc = glGetUniformLocation(m_shader.Program, "threshold");
	GLint cellSizeLoc = glGetUniformLocation(m_shader.Program, "cellSize");
	GLint dirLoc = glGetUniformLocation(m_shader.Program, "lightDir");

	// Pass the model matrix to the shader
	glUniformMatrix4fv(MLoc, 1, GL_FALSE, glm::value_ptr(getModel()));
	glUniform1f(thresholdLoc, 0.6f);
	glUniform1f(cellSizeLoc, m_cSize);
	glUniform3fv(dirLoc, 1, value_ptr(glm::normalize(vec3(-1.0f, -1.0f, -1.0f))));

	glBindVertexArray(getVertexArrayObject());
	glDrawArrays(getMode(), 0, getNumIndices());
}

void Metaball::SetPosUniforms(GLuint &txr, int metaballs)
{
	GLint buffLoc = glGetUniformLocation(m_shader.Program, "metaballs");
	//GLint buffLoc2 = glGetUniformLocation(m_shader.Program, "metaballs2");
	glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_1D, txr);
	glUniform1i(buffLoc, 0);
	//glUniform1i(buffLoc2, 0);

	GLint numLoc = glGetUniformLocation(m_shader.Program, "number");
	glUniform1i(numLoc, metaballs);
	
	////GLuint tbo;
	//glGenBuffers(1, &tbo);
	//glBindBuffer(GL_TEXTURE_BUFFER, tbo);
	//glBufferData(GL_TEXTURE_BUFFER, sizeof(posVals), posVals, GL_STATIC_DRAW);

	////GLuint tbo_tex;
	//glActiveTexture(GL_TEXTURE0);
	//glGenTextures(1, &tboTex);
	//glBindBuffer(GL_TEXTURE_BUFFER, 0);
	//glTexBufferEXT(GL_TEXTURE_BUFFER_EXT, GL_RGB32F, tbo);
	//glUniform1i(buffLoc, 0);
}