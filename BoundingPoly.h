#pragma once

#include "BoundingObject.h"
#include <vector>
#include <array>

struct FaceDef
{
	FaceDef(std::vector<int> indices);
	FaceDef(std::vector<int> indices, glm::vec3 n);
	~FaceDef();

	void CalcNormal(glm::vec3 a, glm::vec3 b);

	std::vector<int> m_vertIndices;
	glm::vec3 m_normal;
};

struct LineDef
{
	LineDef(int a, int b);
	~LineDef();

	int m_a;
	int m_b;
};

class BoundingPoly :
	public BoundingObject
{
public:
	BoundingPoly(std::vector<glm::vec3> verts, std::vector<FaceDef> faceDefs);
	BoundingPoly(Transform* t, std::vector<glm::vec3> verts, std::vector<FaceDef> faceDefs);
	~BoundingPoly();

	void CalcNormals();

	glm::vec2 GetProjection(glm::vec3 n) override;
	std::vector<std::array<glm::vec3, 2>> GetLines();
	std::vector<glm::vec3> GetNormals();
	std::vector<glm::vec3> GetVerts();
protected:
	std::vector<FaceDef> m_faceDefs;
	std::vector<LineDef> m_lineDefs;
	std::vector<glm::vec3> m_verts;
};

