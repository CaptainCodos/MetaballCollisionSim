#include "BoundingObject.h"



BoundingObject::BoundingObject()
{
	m_transform = new Transform();
	m_ownTransform = true;
}
BoundingObject::BoundingObject(Transform* t) 
{
	m_transform = t;
	m_ownTransform = false;
}

BoundingObject::~BoundingObject()
{
	if (m_ownTransform)
		delete m_transform;
}

Transform* BoundingObject::GetTransform() { return m_transform; }
