#pragma once
#include "MetaSurface.h"
#include "BoundingPoly.h"
#include "BoundingSphere.h"

struct ColManifold
{
	ColManifold();
	~ColManifold();

	glm::vec3 mtv;
	float depth;
	bool col;

	std::vector<glm::vec3> incContacts;
	std::vector<glm::vec3> refContacts;
	std::vector<glm::vec3> pointRelVel;

	BoundingObject* incObj;
	BoundingObject* refObj;	// Belongs to mtv shape
	bool incDyn;
	bool refDyn;	// Helps quickly know if each shape is dynamic or not
};

class CollisionHandler
{
public:
	static CollisionHandler* Get();
	~CollisionHandler();

	void HandleCollisions(std::vector<BoundingObject*> objs);
	void HandleCollisions(BoundingObject* a, std::vector<BoundingObject*> objs);
	void CollidePair(BoundingObject* a, BoundingObject* b);
	void CollideMeta(BoundingObject* a, MetaSurface* b);

	ColManifold SphereToSphereData(BoundingSphere* a, BoundingSphere* b);
private:
	CollisionHandler();
	static CollisionHandler* m_instance;

	//ColManifold SphereToSphereData(BoundingSphere* a, BoundingSphere* b);
	ColManifold SphereToPolyData(BoundingSphere* a, BoundingPoly* b);
	ColManifold PolyToPolyData(BoundingPoly* a, BoundingPoly* b);

	void SphereToSphere(BoundingSphere* a, BoundingSphere* b, ColManifold& data, int colsA, int colsB);
	void SphereToPoly(BoundingSphere* a, BoundingPoly* b, ColManifold& data);
	void PolyToPoly(BoundingPoly* a, BoundingPoly* b, ColManifold& data);

	void SphereToMeta(BoundingSphere* a, MetaSurface* m);
	void PolyToMeta(BoundingPoly* a, MetaSurface* m);

	//void Resolve(BoundingObject* a, BoundingObject* b);
};

