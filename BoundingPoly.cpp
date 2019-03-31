#include "BoundingPoly.h"

using namespace std;
using namespace glm;

FaceDef::FaceDef(vector<int> indices)
{
	m_vertIndices = indices;
}
FaceDef::FaceDef(vector<int> indices, vec3 n)
{
	m_vertIndices = indices;
	m_normal = n;
}

FaceDef::~FaceDef()
{

}

void FaceDef::CalcNormal(vec3 a, vec3 b)
{
	m_normal = cross(a, b);
}

LineDef::LineDef(int a, int b)
{

}

LineDef::~LineDef()
{

}

BoundingPoly::BoundingPoly(vector<vec3> verts, vector<FaceDef> faceDefs) : BoundingObject()
{
	m_type = BoundType::POLY;
	m_verts = verts;
	//m_faceDefs = faceDefs;

	vector<ivec2> foundEdges;

	for (int i = 0; i < faceDefs.size(); i++)
	{
		vector<int> vertIs = faceDefs[i].m_vertIndices;
		m_faceDefs.push_back(faceDefs[i]);

		for (int k = 0; k < vertIs.size(); k++)
		{
			int a = (k + 0) % vertIs.size();
			int b = (k + 1) % vertIs.size();
			ivec2 x = ivec2(a, b);
			if (!(std::find(foundEdges.begin(), foundEdges.end(), x) != foundEdges.end()))
			{
				foundEdges.push_back(x);
			}
		}
	}

	for (int i = 0; i < foundEdges.size(); i++)
	{
		m_lineDefs.push_back(LineDef(foundEdges[i].x, foundEdges[i].y));
	}
}
BoundingPoly::BoundingPoly(Transform* t, vector<vec3> verts, vector<FaceDef> faceDefs) : BoundingObject(t)
{
	m_type = BoundType::POLY;
	m_verts = verts;
	//m_faceDefs = faceDefs;

	vector<ivec2> foundEdges;

	for (int i = 0; i < faceDefs.size(); i++)
	{
		vector<int> vertIs = faceDefs[i].m_vertIndices;
		m_faceDefs.push_back(faceDefs[i]);

		for (int k = 0; k < vertIs.size(); k++)
		{
			int a = (k + 0) % vertIs.size();
			int b = (k + 1) % vertIs.size();
			ivec2 x = ivec2(a, b);
			if (!(std::find(foundEdges.begin(), foundEdges.end(), x) != foundEdges.end()))
			{
				foundEdges.push_back(x);
			}
		}
	}

	for (int i = 0; i < foundEdges.size(); i++)
	{
		m_lineDefs.push_back(LineDef(foundEdges[i].x, foundEdges[i].y));
	}
}

BoundingPoly::~BoundingPoly()
{
}

void BoundingPoly::CalcNormals()
{
	for (int i = 0; i < m_faceDefs.size(); i++)
	{
		vec3 a = m_verts[m_faceDefs[i].m_vertIndices[0]] - m_verts[m_faceDefs[i].m_vertIndices[1]];
		vec3 b = m_verts[m_faceDefs[i].m_vertIndices[2]] - m_verts[m_faceDefs[i].m_vertIndices[1]];
		m_faceDefs[i].CalcNormal(a, b);
	}
}

glm::vec2 BoundingPoly::GetProjection(vec3 n)
{
	float min = FLT_MAX;
	float max = FLT_MIN;

	for (int i = 0; i < m_verts.size(); i++)
	{
		vec3 v = (m_verts[i] * mat3(m_transform->GetOrientation() * m_transform->GetScale())) + m_transform->GetPosVec();
		//vec3 v = m_verts[i] * mat3(m_transform->GetFullMat());
		float d = glm::dot(n, v);

		if (d > max)
			max = d;
		if (d < min)
			min = d;
	}

	return vec2(min, max);
}
vector<array<vec3, 2>> BoundingPoly::GetLines()
{
	vector<array<vec3, 2>> vertPairs;

	for (int i = 0; i < m_lineDefs.size(); i++)
	{
		LineDef l = m_lineDefs[i];

		array<vec3, 2> v;
		v[0] = m_verts[l.m_a];
		v[1] = m_verts[l.m_b];

		vertPairs.push_back(v);
	}

	return vertPairs;
}
vector<vec3> BoundingPoly::GetNormals()
{
	vector<vec3> normals;

	for (int i = 0; i < m_faceDefs.size(); i++)
	{
		normals.push_back(m_faceDefs[i].m_normal * mat3(m_transform->GetOrientation()));
	}

	return normals;
}
vector<vec3> BoundingPoly::GetVerts()
{
	vector<vec3> verts;

	for (int i = 0; i < m_verts.size(); i++)
	{
		verts.push_back((m_verts[i] * mat3(m_transform->GetOrientation())) + m_transform->GetPosVec());
	}

	return verts;
}