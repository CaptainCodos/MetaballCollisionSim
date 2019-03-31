#include "Spring.h"

using namespace std;
using namespace glm;

Spring::Spring(Transform* pA, Transform* pB)
{
	m_posA = pA;
	m_posB = pB;

	m_length = 1.0f;
	m_spring = 1.0f;
	m_damp = 1.0f;

	m_frozenA = false;
	m_frozenB = false;
}

Spring::~Spring()
{
}

void Spring::SetLength(float l) { m_length = l; }
void Spring::SetConsts(float d, float s) { m_damp = d; m_spring = s; }
void Spring::SetFrozen(int i, bool frozen) { if (i == 0) m_frozenA = frozen; else m_frozenB = frozen; }

void Spring::SetPosA(Transform* p) { m_posA = p; }
void Spring::SetPosB(Transform* p) { m_posB = p; }

void Spring::ApplyForce()
{
	vec3 a = m_posA->GetPosVec();
	vec3 b = m_posB->GetPosVec();

	vec3 v = b - a;
	float l = glm::length(v);

	if (l > 0.0f)
	{
		vec3 e = glm::normalize(v);
		float vA = dot(m_posA->GetVel(), e);
		float vB = dot(m_posB->GetVel(), e);

		float fSD = -(m_spring * (m_length - l)) - (m_damp * (vA - vB));
		
		m_posA->AddForce(e * fSD);
		m_posB->AddForce(e * -fSD);
	}
}