#include "OctCell.h"
#include "CollisionHandler.h"

using namespace std;
using namespace glm;

OctCell::OctCell(int depth, int curr, vec2 bounds[3])
{
	m_depth = depth;

	for (int i = 0; i < 3; i++)
	{
		m_bounds[i] = bounds[i];
	}

	if (depth < curr)
	{
		for (int z = 0; z < 2; z++)
		{
			for (int y = 0; y < 2; y++)
			{
				for (int x = 0; x < 2; x++)
				{
					float halves[3] = 
					{ 
						(bounds[0].y - bounds[0].x) / 2.0f,
						(bounds[1].y - bounds[1].x) / 2.0f,
						(bounds[2].y - bounds[2].x) / 2.0f
					};

					vec2 b[3] = 
					{
						vec2(halves[0] * x, halves[0] * (x + 1)),
						vec2(halves[1] * y, halves[1] * (y + 1)),
						vec2(halves[2] * z, halves[2] * (z + 1))
					};

					m_cells.push_back(new OctCell(depth, curr + 1, b));
				}
			}
		}
	}
}

OctCell::~OctCell()
{
}

vector<BoundingSphere*>& OctCell::GetSpheres() 
{ 
	vector<BoundingSphere*> spheres;

	for (int i = 0; i < m_spheres.size(); i++)
	{
		spheres.push_back(m_spheres[i]);
	}

	for (int i = 0; i < m_cells.size(); i++)
	{
		vector<BoundingSphere*>& s = m_cells[i]->GetSpheres();

		for (int k = 0; k < s.size(); k++)
		{
			spheres.push_back(s[i]);
		}
	}

	return spheres;
}

void OctCell::CleanCell()
{
	while (!m_spheres.empty())
	{
		m_spheres.pop_back();
	}

	for (int i = 0; i < m_cells.size(); i++)
	{
		m_cells[i]->CleanCell();
	}
}

void OctCell::InsertObj(BoundingSphere* b)
{
	//m_spheres.push_back(b);

	for (int i = 0; i < m_cells.size(); i++)
	{
		if (m_cells[i]->CheckObj(b))
		{
			m_cells[i]->InsertObj(b);
			return;
		}
	}

	m_spheres.push_back(b);
}

bool OctCell::CheckObj(BoundingSphere* b)
{
	vec3 n[3] = { vec3(1.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f), vec3(0.0f, 0.0f, 1.0f) };

	for (int i = 0; i < 3; i++)
	{
		vec2 proj = b->GetProjection(n[i]);

		if (!(proj.x <= m_bounds[i].y && m_bounds[i].x <= proj.y))
			return false;
	}

	return true;
}

void OctCell::DoCollisions()
{
	for (int i = 0; i < m_spheres.size(); i++)
	{
		for (int k = i + 1; k < m_spheres.size(); k++)
		{
			CollisionHandler::Get()->CollidePair(m_spheres[i], m_spheres[k]);
		}
	}

	for (int i = 0; i < m_cells.size(); i++)
	{
		vector<BoundingSphere*>& spheres = m_cells[i]->GetSpheres();

		for (int n = 0; n < m_spheres.size(); n++)
		{
			for (int m = 0; m < spheres.size(); m++)
			{
				CollisionHandler::Get()->CollidePair(m_spheres[n], spheres[m]);
			}
		}

		m_cells[i]->DoCollisions();
	}
}