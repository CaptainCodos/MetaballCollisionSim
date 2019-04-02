#include "MetaSurface.h"
#include "CreateTexture.h"
#include <algorithm>
#include <map>

using namespace std;
using namespace glm;

MetaSurface::MetaSurface(float thresh, float mult, vec3 p, vec3 s, float vertDensity) : Mesh()
{
	m_threshold = thresh;
	m_mult = mult;
	m_limits = 5.0f * (1.0f / pow(m_threshold, 0.5f));
	m_transform->SetPos(p);

	m_mode = GL_POINTS;

	string paths[] = {
		"resources//shaders//Vertex//metaball.vert",
		"",
		"",
		"resources//shaders//Geometry//metaball.geom",
		"resources//shaders//Fragment//metaball.frag" };

	int flags[] = {
	1,
	0,
	0,
	1,
	1 };

	setShader(Shader(paths, flags));

	UpdateVerts(vertDensity, s);
}


MetaSurface::~MetaSurface()
{
}

void MetaSurface::AddMetaball(Metaball b)
{
	float r = 1.0f / m_threshold;
	float radius = r + std::fmin(0.05f, r * 0.05f);
	b.GetTransform()->SetDensity(1.0f, SPHERE);
	b.GetTransform()->GetInertia().UpdateTensor(
		b.GetTransform()->GetMass(),
		glm::vec3(r, r, r),
		SPHERE);
	m_metaballs.push_back(b);
	BoundingSphere s = BoundingSphere(b.GetTransform(), r / 2.0f);
	
	m_spheres.push_back(s);
}

vector<Metaball>& MetaSurface::GetMetaballs() { return m_metaballs; }
vector<BoundingSphere>& MetaSurface::GetBoundSpheres() { return m_spheres; }
void MetaSurface::SetThreshold(float threshold) { m_threshold = threshold; }

bool MetaSurface::HasChanged()
{
	m_hasChanged = false;

	for (int i = 0; i < m_metaballs.size(); i++)
	{
		if (m_metaballs[i].GetTransform()->HasMoved())
		{
			m_hasChanged = true;
			return true;
		}
	}

	return false;
}
float MetaSurface::GetThreshold() { return m_threshold; }
float MetaSurface::GetTotalSqrX()
{
	float xS = 0.0f;

	for (int i = 0; i < m_metaballs.size(); i++)
	{
		xS += pow(m_metaballs[i].GetTransform()->GetPosVec()[0], 2.0f);
	}

	return xS;
}
float MetaSurface::GetTotalSqrY()
{
	float yS = 0.0f;

	for (int i = 0; i < m_metaballs.size(); i++)
	{
		yS += pow(m_metaballs[i].GetTransform()->GetPosVec()[1], 2.0f);
	}

	return yS;
}
float MetaSurface::GetTotalSqrZ()
{
	float zS = 0.0f;

	for (int i = 0; i < m_metaballs.size(); i++)
	{
		zS += pow(m_metaballs[i].GetTransform()->GetPosVec()[2], 2.0f);
	}

	return zS;
}

void MetaSurface::UpdateVerts(float density, vec3 s)
{
	m_density = density;

	vector<Vertex> v;
	ivec3 verts = ivec3(s.x * density, s.y * density, s.z * density);
	float step = 1.0f / density;
	for (int z = 0; z < verts.z; z++)
	{
		for (int y = 0; y < verts.y; y++)
		{
			for (int x = 0; x < verts.x; x++)
			{
				vec3 vert = vec3(x * step, y * step, z * step);
				vert -= s / 2.0f;
				v.push_back(Vertex(vert));
			}
		}
	}

	// create voxel vertices
	initMesh(v.data(), v.size());
}

void MetaSurface::Update(float dt)
{
	m_hasChanged = false;

	HasChanged();

	for (int i = 0; i < m_metaballs.size(); i++)
	{
		m_metaballs[i].UpdateMe(dt);
	}

	if (m_hasChanged)
	{
		UpdateRads();
	}
}

void MetaSurface::UpdateRads()
{
	float threshRoute = pow(m_threshold, 0.5f);
	
	float rBase = 1.0f / threshRoute;
	float limDist = m_limits;

	for (int n = 0; n < m_metaballs.size(); n++)
	{
		float r = 0.0f;
		float count = 1.0f;
		float prev = 1.0f;

		vector<float> dists;

		for (int k = 0; k < m_metaballs.size(); k++)
		{
			if (k == n)
				continue;

			Transform* tA = m_metaballs[n].GetTransform();
			Transform* tB = m_metaballs[k].GetTransform();
			vec3 dist = tB->GetPosVec() - tA->GetPosVec();

			float l = glm::length(dist);

			if (dist != vec3())
			{
				// To ensure evaluation stability, the limits must be treated the same as distance l
				if (l <= limDist + 1.0f)
					dists.push_back(l);
				else
					continue;
			}
			else
			{
				dists.push_back(0.0f);
			}
		}

		// sort so lowest distances come first
		std::sort(dists.begin(), dists.end(), less<float>());

		for (int i = 0; i < dists.size(); i++)
		{
			// take current distance
			float l = dists[i];

			// calculate contribution this metaball makes to the radius
			float mult = pow(count + 1.0f, 0.5f) - pow(count, 0.5f);

			// calculate influence at the evaluation limits
			float infL = (1.0f / pow((limDist + 1.0f), 2.0f));

			// calculate the factor by which we ensure the value at l = 0 is rBase
			float fact = rBase / (1.0f - infL);

			// calculate value to add with respect to the current contribution
			float inf = (fact * std::min((1.0f / pow(l + 1.0f, 2.0f)), 1.0f)) * mult;
			r += inf;

			count += 1.0f;
		}

		m_spheres[n].SetRadius(rBase + (r));
		m_spheres[n].GetTransform()->SetDensity(1.0, SPHERE, m_spheres[n].GetRadius());
		vec3 s = m_spheres[n].GetTransform()->GetScaleVec();
		float m = m_spheres[n].GetTransform()->GetMass();
		m_spheres[n].GetTransform()->GetInertia().UpdateTensor(m, s, SPHERE);
	}
}

void MetaSurface::SetUniforms()
{
	// Get the uniform locations
	GLint modelLoc = glGetUniformLocation(m_shader.Program, "model");

	// Pass the model matrix to the shader
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(getModel()));

	// Get the uniform locations
	GLint MLoc = glGetUniformLocation(m_shader.Program, "M");
	GLint thresholdLoc = glGetUniformLocation(m_shader.Program, "threshold");
	GLint cellSizeLoc = glGetUniformLocation(m_shader.Program, "cellSize");
	GLint limLoc = glGetUniformLocation(m_shader.Program, "limits");
	GLint dirLoc = glGetUniformLocation(m_shader.Program, "lightDir");

	// Pass relevant data to the shader
	glUniformMatrix4fv(MLoc, 1, GL_FALSE, glm::value_ptr(getModel()));
	glUniform1f(thresholdLoc, m_threshold);
	glUniform1f(cellSizeLoc, 1.0f / m_density);
	glUniform1f(limLoc, m_limits);
	glUniform3fv(dirLoc, 1, value_ptr(glm::normalize(vec3(-1.0f, -1.0f, -1.0f))));

	glBindVertexArray(getVertexArrayObject());
	glDrawArrays(getMode(), 0, getNumIndices());
}

void MetaSurface::SetPosUniforms(GLuint &txr)
{
	// Pass metaball data to the shader
	GLint buffLoc = glGetUniformLocation(m_shader.Program, "metaballs");
	glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_1D, txr);
	glUniform1i(buffLoc, 0);

	GLint numLoc = glGetUniformLocation(m_shader.Program, "number");
	glUniform1i(numLoc, m_metaballs.size());
}

void MetaSurface::Clean()
{
	/*glDeleteTextures(1, &m_txr);
	m_posVals.clear();*/
}