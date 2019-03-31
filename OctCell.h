#pragma once

#include "BoundingSphere.h"

#include "glm/glm.hpp"

#include <vector>

class OctCell
{
public:
	OctCell(int depth, int curr, glm::vec2 bounds[3]);
	~OctCell();

	void CleanCell();
	void InsertObj(BoundingSphere* b);
	bool CheckObj(BoundingSphere* b);
	void DoCollisions();

	std::vector<BoundingSphere*>& GetSpheres();
private:
	int m_depth;
	std::vector<BoundingSphere*> m_spheres;
	glm::vec2 m_bounds[3];

	OctCell* m_parent;
	std::vector<OctCell*> m_cells;
};

