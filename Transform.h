#pragma once

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/matrix_operation.hpp>
#include "glm/ext.hpp"

#include "Inertia.h"
#include <vector>

struct Impulse
{
	Impulse(glm::vec3 p, glm::vec3 m);
	~Impulse();

	glm::vec3 m_pos;
	glm::vec3 m_momentum;
};

class Transform
{
public:
	Transform();
	~Transform();

	void Update(float dt);

	void AddForce(glm::vec3 f);
	void AddTorque(glm::vec3 t);
	void AddImpulse(glm::vec3 p, glm::vec3 m);
	void RemoveImpulses();
	void RemoveForces();
	void RemoveTorque();

	void SetDynamic(bool b);
	void SetLinLock(int i, bool l);
	void SetRotLock(int i, bool l);
	void SetCoR(float coR);
	void SetCoF(float coF);
	void SetDrag(float drag);
	void SetDensity(float val, InertiaShape shape);
	void SetDensity(float val, InertiaShape shape, float scale);
	void SetVel(glm::vec3 v);
	void SetAVel(glm::vec3 aV);
	void SetPos(glm::vec3 p);
	void SetPos(int i, float p);
	void SetRot(float ang, glm::vec3 axis);
	void SetScale(glm::vec3 s);

	bool IsDynamic();
	bool HasMoved();
	float GetCoR();
	float GetCoF();
	float GetMass();
	glm::mat4 GetTranslation();
	glm::mat4 GetOrientation();
	glm::mat4 GetScale();
	glm::mat4 GetFullMat();
	glm::vec3 GetVel();
	glm::vec3 GetAngVel();
	glm::vec3& GetRefVel();
	glm::vec3& GetRefAngVel();
	Inertia& GetInertia();
	glm::vec3 GetPosVec();
	glm::vec3 GetScaleVec();

	void Translate(glm::vec3 v);
	void Rotate(float ang, glm::vec3 axis);
	void Scale(glm::vec3 s);
private:
	bool m_hasMoved;
	bool m_dynamic;
	bool m_linLocks[3];
	bool m_rotLocks[3];

	float m_coR;
	float m_coF;

	float m_density;
	float m_mass;

	float m_dragCo;

	glm::mat4 m_translation;
	glm::mat4 m_orientation;
	glm::mat4 m_scale;

	glm::vec3 m_force;
	glm::vec3 m_torque;

	glm::vec3 m_vel;
	glm::vec3 m_aVel;

	Inertia m_inertia;

	std::vector<Impulse> m_frameImpulses;

	void ApplyImpulse(glm::vec3 p, glm::vec3 m);
	/*void ApplyForce(float dt);
	void ApplyTorque(float dt);

	void LinearMechanics(float dt);
	void AngularMechanics(float dt);*/
};

