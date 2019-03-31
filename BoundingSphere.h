#pragma once
#include "BoundingObject.h"

class BoundingSphere : public BoundingObject
{
public:
	BoundingSphere();
	BoundingSphere(Transform* t, float radius);
	~BoundingSphere();

	glm::vec2 GetProjection(glm::vec3 n) override;

	void SetRadius(float r);
	float GetRadius() { return m_radius; }
protected:
	float m_radius;
};

