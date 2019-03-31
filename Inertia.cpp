#include "Inertia.h"

using namespace glm;

Inertia::Inertia()
{
	m_iTensor = mat4(1.0f);
	m_inverseITensor = mat4(1.0f);
}

Inertia::~Inertia()
{
}

void Inertia::CustomTensor(glm::mat3 mat) { m_iTensor = mat; }
void Inertia::UpdateTensor(float m, vec3 s, InertiaShape i)
{
	if (i == CUBE)
	{
		float div = 1.0f / 12.0f;
		m_iTensor = glm::mat3(
			div * m * (pow(s[1], 2.0f) + pow(s[2], 2.0f)), 0.0f, 0.0f,
			0.0f, div * m * (pow(s[0], 2.0f) + pow(s[2], 2.0f)), 0.0f,
			0.0f, 0.0f, div * m * (pow(s[0], 2.0f) + pow(s[1], 2.0f)));
	}
	else if (i == SPHERE)
	{
		float div = 2.0f / 5.0f;
		m_iTensor = glm::mat3(
			div * m * (pow(s[0] / 2.0f, 2.0f)), 0.0f, 0.0f,
			0.0f, div * m * (pow(s[1] / 2.0f, 2.0f)), 0.0f,
			0.0f, 0.0f, div * m * (pow(s[2] / 2.0f, 2.0f)));
	}

	UpdateInverse();
}

void Inertia::UpdateInverse()
{
	m_inverseITensor = inverse(mat4(m_iTensor));
}

mat3 Inertia::GetTensor() { return m_iTensor; }
mat3 Inertia::GetInverse() { return m_inverseITensor; }
mat3 Inertia::GetFullTensor(mat4 o) { return o * mat4(m_iTensor) * transpose(o); }
mat3 Inertia::GetFullInverse(mat4 o) { return o * mat4(m_inverseITensor) * transpose(o); }