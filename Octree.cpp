#include "Octree.h"
#include "CollisionHandler.h"

using namespace std;
using namespace glm;

Octree::Octree(int depth, vec3 iSize)
{
	m_depth = depth;
	m_size = iSize;

	float halves[3] = 
	{
		iSize.x,
		iSize.y,
		iSize.z
	};

	for (int z = 0; z < 2; z++)
	{
		for (int y = 0; y < 2; y++)
		{
			for (int x = 0; x < 2; x++)
			{
				vec2 b[3] =
				{
					vec2((halves[0] * x) - iSize.x, (halves[0] * (x + 1)) - iSize.x),
					vec2((halves[1] * y) - iSize.y, (halves[1] * (y + 1)) - iSize.y),
					vec2((halves[2] * z) - iSize.z, (halves[2] * (z + 1)) - iSize.z)
				};

				m_containers.push_back(new OctCell(depth, 1, b));
			}
		}
	}
}

Octree::~Octree()
{
}

void Octree::InsertObj(BoundingSphere* obj)
{
	for (int i = 0; i < m_containers.size(); i++)
	{
		if (m_containers[i]->CheckObj(obj))
		{
			m_containers[i]->InsertObj(obj);
			return;
		}
	}

	vec3 n[3] = { vec3(1.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f), vec3(0.0f, 0.0f, 1.0f) };

	bool largeEnough = true;
	for (int i = 0; i < 3; i++)
	{
		vec2 proj = obj->GetProjection(n[i]);
		float l = glm::length(proj);

		if (m_size[i] <= l)
		{
			largeEnough = false;
			m_spheres.push_back(obj);
			return;
		}
	}

	vec2 bounds[3];
	for (int i = 0; i < 3; i++)
	{
		vec2 proj = obj->GetProjection(n[i]);
		vec2 cell = vec2(proj.x / (int)m_size.x, proj.y / (int)m_size.y);

		bounds[i] = vec2(cell.x * m_size.x, cell.y * m_size.y);
	}

	OctCell* cell = new OctCell(m_depth, 1, bounds);
	cell->InsertObj(obj);
}

void Octree::DoCollisions()
{
	for (int i = 0; i < m_spheres.size(); i++)
	{
		for (int k = i + 1; k < m_spheres.size(); k++)
		{
			CollisionHandler::Get()->CollidePair(m_spheres[i], m_spheres[k]);
		}
	}

	for (int i = 0; i < m_containers.size(); i++)
	{
		vector<BoundingSphere*>& spheres = m_containers[i]->GetSpheres();

		for (int n = 0; n < m_spheres.size(); n++)
		{
			for (int m = 0; m < spheres.size(); m++)
			{
				CollisionHandler::Get()->CollidePair(m_spheres[n], spheres[m]);
			}
		}

		m_containers[i]->DoCollisions();
	}
}

void Octree::Clean()
{
	while (!m_spheres.empty())
	{
		m_spheres.pop_back();
	}

	for (int i = 0; i < m_containers.size(); i++)
	{
		m_containers[i]->CleanCell();
	}
}