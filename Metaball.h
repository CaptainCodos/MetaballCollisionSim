#pragma once
#include "Mesh.h"

class Metaball :
	public Mesh
{
public:
	Metaball();
	~Metaball();

	void SetCellSize(float size);
	void SetRadius(float rad);

	void CancelImpulses();
	void UpdateMe(float dt);
	void SetUniforms() override;
	void SetPosUniforms(GLuint &txr, int metaballs);

	glm::mat4 getModel() override { return m_snapped * m_transform->GetOrientation() * m_transform->GetScale(); }
	glm::mat4 getTrueModel() { return m_transform->GetTranslation() * m_transform->GetOrientation() * m_transform->GetScale(); }

	float getRadius() { return m_radius; }
protected:
	float m_radius;
	float m_cSize;

	glm::mat4 m_snapped;

	void UpdateVerts();
	//std::array<Vertex> ConvertVector(std::vector<Vertex> &verts, const int &size);
};

