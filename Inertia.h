#pragma once

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/matrix_operation.hpp>
#include "glm/ext.hpp"

enum InertiaShape
{
	CUBE,
	SPHERE,
	OCTAHEDRON
};
class Inertia
{
public:
	Inertia();
	~Inertia();

	void CustomTensor(glm::mat3 mat);
	void UpdateTensor(float m, glm::vec3 s, InertiaShape i);
	void UpdateInverse();

	glm::mat3 GetTensor();
	glm::mat3 GetInverse();
	glm::mat3 GetFullTensor(glm::mat4 o);
	glm::mat3 GetFullInverse(glm::mat4 o);
private:
	glm::mat3 m_iTensor;
	glm::mat3 m_inverseITensor;
};

