#pragma once

#include "Transform.h"

class Spring
{
public:
	Spring(Transform* pA, Transform* pB);
	~Spring();

	void SetLength(float l);
	void SetConsts(float d, float s);

	void SetPosA(Transform* p);
	void SetPosB(Transform* p);

	void SetFrozen(int i, bool frozen);

	void ApplyForce();
private:
	float m_length;
	float m_damp;
	float m_spring;

	Transform* m_posA;
	Transform* m_posB;

	bool m_frozenA;
	bool m_frozenB;
};

