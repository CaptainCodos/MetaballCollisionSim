#pragma once

#include "Transform.h"

class BoundingObject
{
public:
	enum BoundType
	{
		SPHERE,
		POLY
	};

	BoundingObject();
	BoundingObject(Transform* t);
	~BoundingObject();

	Transform* GetTransform();
	virtual glm::vec2 GetProjection(glm::vec3 n) = 0;

	BoundType GetBound() { return m_type; }
protected:
	bool m_ownTransform;
	Transform* m_transform;	// Physics data for the object such as position and orientation
	BoundType m_type;	// Type of object such as polyhedron or sphere
};

