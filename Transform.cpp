#include "Transform.h"

using namespace glm;

Impulse::Impulse(vec3 p, vec3 m)
{
	m_pos = p;
	m_momentum = m;
}
Impulse::~Impulse()
{

}

Transform::Transform()
{
	m_dynamic = true;
	m_inertia = Inertia();
	m_mass = 1.0f;
	m_coR = 0.3f;
	m_coF = 0.2f;
	m_dragCo = 0.06f;

	for (int i = 0; i < 3; i++)
	{
		m_linLocks[i] = false;
		m_rotLocks[i] = false;
	}
}

Transform::~Transform()
{
}

void Transform::SetDynamic(bool b) { m_dynamic = b; }
void Transform::SetLinLock(int i, bool l) { m_linLocks[i] = l; }
void Transform::SetRotLock(int i, bool l) { m_rotLocks[i] = l; }
void Transform::SetCoR(float coR) { m_coR = coR; }
void Transform::SetCoF(float coF) { m_coF = coF; }
void Transform::SetDrag(float drag) { m_dragCo = drag; }
void Transform::SetDensity(float val, InertiaShape shape)
{
	m_density = val;

	if (shape == CUBE)
	{
		float vol = m_scale[0][0] * m_scale[1][1] * m_scale[2][2];
		m_mass = m_density * vol;
	}
	else if (shape == SPHERE)
	{
		float vol = (4.0f / 3.0f) * glm::pi<float>() * pow(m_scale[0][0], 3.0f);
		m_mass = m_density * val;
	}
	else if (shape == OCTAHEDRON)
	{
		float vol = (pow(2.0f, 0.5f) / 3.0f) * pow(m_scale[0][0], 3.0f);
		m_mass = m_density * val;
	}
}
void Transform::SetDensity(float val, InertiaShape shape, float scale)
{
	m_density = val;

	if (shape == CUBE)
	{
		float vol = scale * scale * scale;
		m_mass = m_density * vol;
	}
	else if (shape == SPHERE)
	{
		float vol = (4.0f / 3.0f) * glm::pi<float>() * pow(scale, 3.0f);
		m_mass = m_density * val;
	}
	else if (shape == OCTAHEDRON)
	{
		float vol = (pow(2.0f, 0.5f) / 3.0f) * pow(scale, 3.0f);
		m_mass = m_density * val;
	}
}
void Transform::SetVel(vec3 v) { m_vel = v; }
void Transform::SetAVel(vec3 aV) { m_aVel = aV; }
void Transform::SetPos(vec3 p) { m_translation[3][0] = p.x; m_translation[3][1] = p.y; m_translation[3][2] = p.z; }
void Transform::SetPos(int i, float p) { m_translation[3][i] = p; }
void Transform::SetRot(float ang, vec3 axis) { m_orientation = mat4(1.0f); Rotate(ang, axis); }
void Transform::SetScale(vec3 s) { m_scale = mat4(1.0f); Scale(s); }

bool Transform::IsDynamic() { return m_dynamic; }
bool Transform::HasMoved() { return m_hasMoved; }
float Transform::GetCoR() { return m_coR; }
float Transform::GetCoF() { return m_coF; }
float Transform::GetMass() { return m_mass; }
mat4 Transform::GetTranslation() { return m_translation; }
mat4 Transform::GetOrientation() { return m_orientation; }
mat4 Transform::GetScale() { return m_scale; }
mat4 Transform::GetFullMat() { return m_translation * m_orientation * m_scale; }
vec3 Transform::GetVel() { return m_vel; }
vec3 Transform::GetAngVel() { return m_aVel; }
vec3& Transform::GetRefVel() { return m_vel; }
vec3& Transform::GetRefAngVel() { return m_aVel; }
Inertia& Transform::GetInertia() { return m_inertia; }
vec3 Transform::GetPosVec() { return vec3(m_translation[3][0], m_translation[3][1], m_translation[3][2]); }
vec3 Transform::GetScaleVec() { return vec3(m_scale[0][0], m_scale[1][1], m_scale[2][2]); }

void Transform::AddForce(vec3 f) { m_force += f; }
void Transform::AddTorque(vec3 t) { m_torque = t; }
void Transform::AddImpulse(vec3 p, vec3 m) { m_frameImpulses.push_back(Impulse(p, m)); }
void Transform::ApplyImpulse(vec3 p, vec3 m)
{
	glm::vec3 L = glm::cross(p, m);
	m_aVel += (L * m_inertia.GetFullInverse(m_orientation));
	m_vel += (m / m_mass);
}
void Transform::RemoveImpulses() { m_frameImpulses.clear(); }
void Transform::RemoveForces() { m_force = vec3(); }
void Transform::RemoveTorque() { m_torque = vec3(); }

void Transform::Translate(vec3 v) { m_translation = translate(m_translation, v); }
void Transform::Rotate(float ang, vec3 axis) { m_orientation = rotate(m_orientation, ang, axis); }
void Transform::Scale(vec3 s) { m_scale = scale(m_scale, s); }

void Transform::Update(float dt)
{
	if (m_dynamic)
	{
		m_hasMoved = false;

		// Apply impulses
		for (int i = 0; i < m_frameImpulses.size(); i++)
		{
			ApplyImpulse(m_frameImpulses[i].m_pos, m_frameImpulses[i].m_momentum);
		}
		m_frameImpulses.clear();

		// Apply linear air friction
		if (m_vel[0] != 0.0f || m_vel[1] != 0.0f || m_vel[2] != 0.0f)
		{
			m_force += 0.5f * glm::length2(m_vel) * m_dragCo * -glm::normalize(m_vel);
		}

		vec3 acc = m_force / m_mass;
		m_vel += acc * dt;

		// Lock axes and check if moving
		for (int i = 0; i < 3; i++)
		{
			if (m_linLocks[i])
				m_vel[i] = 0.0f;
			else
			{
				if (m_vel[i] != 0.0f)
					m_hasMoved = true;
			}
		}

		m_translation = translate(m_translation, m_vel * dt);

		// Apply angular air friction
		if (m_aVel[0] != 0.0f || m_aVel[1] != 0.0f || m_aVel[2] != 0.0f)
		{
			m_torque += 0.5f * glm::length2(m_aVel) * m_dragCo * 2.0f * -glm::normalize(m_aVel);
		}

		vec3 aAcc = m_torque / m_inertia.GetFullTensor(m_orientation);
		// integration ( rotation )
		glm::vec3 dRotA = aAcc * dt;
		if (glm::dot(dRotA, dRotA) > 0.0f)
			m_aVel += dRotA;

		// Lock axes and check if moving
		for (int i = 0; i < 3; i++)
		{
			if (m_rotLocks[i])
				m_aVel[i] = 0.0f;
			else
			{
				if (m_aVel[i] != 0.0f)
					m_hasMoved = true;
			}
		}

		// create skew symmetric matrix for w
		glm::mat3 angVelSkew = glm::matrixCross3(m_aVel);
		// create 3x3 rotation matrix from rb rotation matrix
		glm::mat3 R = glm::mat3(m_orientation);
		// update rotation matrix
		R += dt * angVelSkew * R;
		R = glm::orthonormalize(R);
		m_orientation = glm::mat4(R);

		m_torque = vec3();
		m_force = vec3();
	}
}