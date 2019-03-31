#pragma once
#include "Metaball.h"
#include "BoundingSphere.h"


class MetaSurface : public Mesh
{
public:
	MetaSurface(float thresh, float mult, glm::vec3 p, glm::vec3 s, float vertDensity);
	~MetaSurface();

	void AddMetaball(Metaball b);

	void Update(float dt);
	void SetUniforms() override;
	void SetPosUniforms(GLuint &txr);
	void SetThreshold(float threshold);
	void Clean();
	
	std::vector<Metaball>& GetMetaballs();
	std::vector<BoundingSphere>& GetBoundSpheres();

	bool HasChanged();
	float GetThreshold();
	float GetTotalSqrX();
	float GetTotalSqrY();
	float GetTotalSqrZ();

	void UpdateVerts(float density, glm::vec3 s);
	void UpdateRads();
private:
	bool m_hasChanged;

	std::vector<BoundingSphere> m_spheres;
	std::vector<Metaball> m_metaballs;
	float m_threshold;
	float m_mult;
	float m_density;

	GLuint m_txr;
	std::vector<float> m_posVals;
};

