#pragma once

#include "OctCell.h"



class Octree
{
public:
	Octree(int depth, glm::vec3 iSize);
	~Octree();

	void InsertObj(BoundingSphere* b);
	void DoCollisions();
	void Clean();
private:
	glm::vec3 m_size;
	float m_depth;
	std::vector<OctCell*> m_containers;
	std::vector<BoundingSphere*> m_spheres;
};

