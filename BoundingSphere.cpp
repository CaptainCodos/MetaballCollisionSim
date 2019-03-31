#include "BoundingSphere.h"

using namespace glm;

BoundingSphere::BoundingSphere() : BoundingObject()
{
	m_radius = 1.0f;
	m_type = BoundType::SPHERE;
}
BoundingSphere::BoundingSphere(Transform* t, float radius) : BoundingObject(t) 
{ 
	m_radius = radius;
	m_type = BoundType::SPHERE;
}

BoundingSphere::~BoundingSphere()
{
}

vec2 BoundingSphere::GetProjection(vec3 n)
{
	/*if (glm::length(n) <= 0.0f)
	{
		n = vec3(0.0f, 1.0f, 0.0f);
	}*/

	vec3 p = m_transform->GetPosVec();

	vec3 p1 = p + (n * -m_radius);
	vec3 p2 = p + (n * m_radius);

	float a = dot(n, p1);
	float b = dot(n, p2);

	return vec2(a, b);
}

void BoundingSphere::SetRadius(float r) { m_radius = r; }