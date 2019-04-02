#include "CollisionHandler.h"

CollisionHandler* CollisionHandler::m_instance = NULL;

using namespace std;
using namespace glm;

ColManifold::ColManifold()
{
	mtv = glm::vec3();
	depth = 0.0f;
	col = false;
}
ColManifold::~ColManifold()
{

}

CollisionHandler::CollisionHandler()
{
}

CollisionHandler::~CollisionHandler()
{
}

CollisionHandler* CollisionHandler::Get() { return m_instance != nullptr ? m_instance : m_instance = new CollisionHandler(); }

void CollisionHandler::CollidePair(BoundingObject* a, BoundingObject* b)
{
	int id = 0;
	bool reverse = false;

	if (a->GetBound() == BoundingObject::BoundType::POLY)
		id++;
	if (b->GetBound() == BoundingObject::BoundType::POLY)
		id++;

	if (id == 1 && a->GetBound() == BoundingObject::BoundType::POLY)
		reverse = true;

	if (id == 0)
	{
		BoundingSphere* b1 = dynamic_cast<BoundingSphere*>(a);
		BoundingSphere* b2 = dynamic_cast<BoundingSphere*>(b);
		ColManifold d = SphereToSphereData(b1, b2);
		if (d.col)
		{
			Transform* tA = b1->GetTransform();
			tA->Translate(d.mtv * -d.depth);
			SphereToSphere(b1, b2, d, 1, 1);
		}
			
		//SphereToSphere(b1, b2, d);
	}
	else if (id == 1)
	{
		if (reverse)
		{
			BoundingPoly* b1 = dynamic_cast<BoundingPoly*>(a);
			BoundingSphere* b2 = dynamic_cast<BoundingSphere*>(b);
			//SphereToPoly(b2, b1);
		}
		else
		{
			BoundingSphere* b1 = dynamic_cast<BoundingSphere*>(a);
			BoundingPoly* b2 = dynamic_cast<BoundingPoly*>(b);
			//SphereToPoly(b1, b2);
		}
	}
	else if (id == 2)
	{
		BoundingPoly* b1 = dynamic_cast<BoundingPoly*>(a);
		BoundingPoly* b2 = dynamic_cast<BoundingPoly*>(b);
		//PolyToPoly(b1, b2);
	}

}

ColManifold CollisionHandler::SphereToSphereData(BoundingSphere* a, BoundingSphere* b)
{
	Transform* tA = a->GetTransform();
	Transform* tB = b->GetTransform();
	vec3 r = tB->GetPosVec() - tA->GetPosVec();
	float totalR = a->GetRadius() + b->GetRadius();
	float lR = glm::length(r);
	ColManifold data = ColManifold();

	if (lR > 0.0f)
	{
		if (lR <= totalR)
		{
			bool dynA = tA->IsDynamic();
			bool dynB = tB->IsDynamic();
			if (dynA || dynB)
			{
				vec3 mtv = glm::normalize(r);

				
				data.mtv = mtv;
				data.col = true;

				data.refObj = a;
				data.incObj = b;

				data.incDyn = dynB;
				data.refDyn = dynA;

				data.depth = totalR - lR;

				vec3 r_a = mtv * a->GetRadius();
				vec3 r_b = -mtv * b->GetRadius();

				vec3 vA = tA->GetVel();
				vec3 wA = tA->GetAngVel();

				vec3 vB = tB->GetVel();
				vec3 wB = tB->GetAngVel();

				vec3 finalRelV = vec3();
				int c = 0;
				if (dynA)
				{
					vec3 relVa = vA + (cross(wA, r_a));
					finalRelV -= relVa;
					c++;
				}
				if (dynB)
				{
					vec3 relVB = vB - (cross(wB, r_b));
					finalRelV += relVB;
					c += 2;
				}

				data.pointRelVel.push_back(finalRelV);
				data.incContacts.push_back(r_b);
				data.refContacts.push_back(r_a);
			}
		}
		else
		{
			data.col = false;
		}
	}
	else
	{

	}

	return data;
}
void CollisionHandler::SphereToSphere(BoundingSphere* a, BoundingSphere* b, ColManifold& data, int colsA, int colsB)
{
	Transform* tA = a->GetTransform();
	Transform* tB = b->GetTransform();

	bool dynA = tA->IsDynamic();
	bool dynB = tB->IsDynamic();
	if (dynA || dynB)
	{
		// Extract info from the collision manifold
		vec3 mtv = data.mtv;
		vec3 r_a = data.refContacts[0];
		vec3 r_b = data.incContacts[0];
		vec3 finalRelV = data.pointRelVel[0];

		float relVDotN = dot(finalRelV, mtv);
		float totalCoR = std::max(tA->GetCoR(), tB->GetCoR());

		// Create the numerator
		float numer = -(1.0f + totalCoR) * relVDotN;

		float denom = 0.0f;
		vec3 finalProd = vec3();
		float invMass = 0.0f;
		if (dynA)
		{
			float iMassA = 1.0f / a->GetTransform()->GetMass();
			invMass += iMassA;

			vec3 prodA = tA->GetInertia().GetFullInverse(tA->GetOrientation()) * cross(cross(r_a, mtv), r_a);
			finalProd += prodA;
		}
		if (dynB)
		{
			float iMassB = 1.0f / b->GetTransform()->GetMass();
			invMass += iMassB;

			vec3 prodB = tB->GetInertia().GetFullInverse(tB->GetOrientation()) * cross(cross(r_b, mtv), r_b);
			finalProd += prodB;
		}

		denom = invMass + dot(finalProd, mtv);

		// Impulse magnitude
		float j = numer / denom;

		

		tA->AddImpulse(r_a, (mtv * -j) / (data.refContacts.size() * colsA));
		tB->AddImpulse(r_b, (mtv * j) / (data.incContacts.size() * colsB));

		vec3 vTan = finalRelV - (dot(finalRelV, mtv) * mtv);

		float lVTan = glm::length(vTan);

		if (lVTan != 0.0f)
		{
			vec3 vT = glm::normalize(vTan);

			// Calculate tangential J
			numer = -dot(finalRelV, vT);
			denom = invMass + dot(finalProd, vT);
			float jT = numer / denom;

			float tCoF = std::max(tA->GetCoF(), tB->GetCoF());

			// Cap jT
			if (jT > j * tCoF)
				jT = j * tCoF;
			else if (jT < -j * tCoF)
				jT = -j * tCoF;

			tA->AddImpulse(r_a, (vT * -jT) / (data.refContacts.size() * colsA));
			tB->AddImpulse(r_b, (vT * jT) / (data.incContacts.size() * colsB));
		}
	}
}

// Not used
ColManifold CollisionHandler::SphereToPolyData(BoundingSphere* a, BoundingPoly* b)
{
	Transform* tA = a->GetTransform();
	Transform* tB = b->GetTransform();

	ColManifold data = ColManifold();
	data.col = false;

	bool dynA = tA->IsDynamic();
	bool dynB = tB->IsDynamic();
	bool rev = false;

	if (dynA || dynB)
	{
		vector<vec3> normalsB = b->GetNormals();
		vec3 normalA;

		vec3 dist = tB->GetPosVec() - tA->GetPosVec();

		float min = FLT_MAX;
		for (int i = 0; i < normalsB.size(); i++)
		{
			vec3 n = normalsB[i];

			vec2 pA = a->GetProjection(n);
			vec2 pB = b->GetProjection(n);

			//if (aMax < bMin || bMax < aMin)
			//	return false; //no overlap
			//else
			//	return true; //yes overlap
			if (pB.x <= pA.y && pA.x <= pB.y)
			{
				float lA = std::min(pA.x, pB.x);
				float lB = std::max(pA.y, pB.y);
				float l = lB - lA;

				float d = (glm::length(pA) + glm::length(pB)) - l;

				if (d < min)
				{
					min = d;

					data.depth = d;
					data.mtv = n;

					data.refObj = b;
					data.incObj = a;

					data.refDyn = dynB;
					data.incDyn = dynA;
				}
				
			}
			else
			{
				data.col = false;
				return data;
			}
		}

		if (glm::length(dist) > 0.0f)
		{
			glm::vec3 n = glm::normalize(dist);

			vec2 pA = a->GetProjection(n);
			vec2 pB = b->GetProjection(n);

			//if (aMax < bMin || bMax < aMin)
			//	return false; //no overlap
			//else
			//	return true; //yes overlap
			if (pB.x <= pA.y && pA.x <= pB.y)
			{
				float lA = std::min(pA.x, pB.x);
				float lB = std::max(pA.y, pB.y);
				float l = lB - lA;

				float d = (glm::length(pA) + glm::length(pB)) - l;

				if (d < min)
				{
					min = d;

					data.depth = d;
					data.mtv = n;

					data.refObj = a;
					data.incObj = b;

					data.refDyn = dynA;
					data.incDyn = dynB;
				}
				
			}
			else
			{
				data.col = false;
				return data;
			}
		}

		data.col = true;

		vec3 nD = glm::normalize(dist);
		vec3 mtv = data.mtv;
		if (dot(nD, mtv) < 0.0f)
		{
			mtv *= -1.0f;
		}
		vec3 p = tA->GetPosVec() + (nD * a->GetRadius());

		vec3 pRef = p - data.refObj->GetTransform()->GetPosVec();
		vec3 pInc = p - data.incObj->GetTransform()->GetPosVec();

		data.refContacts.push_back(pRef);
		data.incContacts.push_back(pInc);

		vec3 vA = tA->GetVel();
		vec3 wA = tA->GetAngVel();

		vec3 vB = tB->GetVel();
		vec3 wB = tB->GetAngVel();

		vec3 finalRelV = vec3();
		if (data.refDyn)
		{
			vec3 relVa = vA + (cross(wA, data.refContacts[0]));
			finalRelV += relVa;
		}
		if (data.incDyn)
		{
			vec3 relVB = vB - (cross(wB, data.incContacts[0]));
			finalRelV -= relVB;
		}
		data.pointRelVel.push_back(finalRelV);

		return data;
	}
	
}
// Not used
void CollisionHandler::SphereToPoly(BoundingSphere* a, BoundingPoly* b, ColManifold& data)
{
	Transform* tA = data.refObj->GetTransform();
	Transform* tB = data.incObj->GetTransform();

	bool dynA = tA->IsDynamic();
	bool dynB = tB->IsDynamic();
	if (dynA || dynB)
	{
		// Extract info from the collision manifold
		vec3 mtv = data.mtv;
		vec3 r_a = data.refContacts[0];
		vec3 r_b = data.incContacts[0];
		vec3 finalRelV = data.pointRelVel[0];

		float relVDotN = dot(finalRelV, mtv);
		float totalCoR = tA->GetCoR() * tB->GetCoR();

		// Create the numerator
		float numer = -(1.0f - totalCoR) * relVDotN;

		float denom = 0.0f;
		vec3 finalProd = vec3();
		float invMass = 0.0f;
		if (dynA)
		{
			float iMassA = 1.0f / a->GetTransform()->GetMass();
			invMass += iMassA;

			vec3 prodA = tA->GetInertia().GetFullInverse(tA->GetOrientation()) * cross(cross(r_a, mtv), r_a);
			finalProd += prodA;
		}
		if (dynB)
		{
			float iMassB = 1.0f / b->GetTransform()->GetMass();
			invMass += iMassB;

			vec3 prodB = tB->GetInertia().GetFullInverse(tB->GetOrientation()) * cross(cross(r_b, mtv), r_b);
			finalProd += prodB;
		}

		denom = invMass + dot(finalProd, mtv);

		// Impulse magnitude
		float j = numer / denom;

		tA->AddImpulse(r_a, mtv * j);
		tB->AddImpulse(r_b, -mtv * j);

		vec3 vTan = finalRelV - (dot(finalRelV, mtv) * mtv);

		float lVTan = glm::length(vTan);

		if (lVTan != 0.0f)
		{
			vec3 vT = glm::normalize(vTan);

			// Calculate tangential J
			numer = -dot(finalRelV, vT);
			denom = invMass + dot(finalProd, vT);
			float jT = numer / denom;

			float tCoF = std::sqrtf(tA->GetCoF() * tB->GetCoF());

			// Cap jT
			if (jT > j * tCoF)
				jT = j * tCoF;
			else if (jT < -j * tCoF)
				jT = -j * tCoF;

			tA->AddImpulse(r_a, vT * -jT);
			tB->AddImpulse(r_b, vT * jT);
		}
	}
}

// Not usedd
void CollisionHandler::PolyToPoly(BoundingPoly* a, BoundingPoly* b, ColManifold& data)
{

}

void CollisionHandler::CollideMeta(BoundingObject* a, MetaSurface* b)
{
	if (BoundingSphere* sphere = dynamic_cast<BoundingSphere*>(a))
	{
		SphereToMeta(sphere, b);
	}
	else if (BoundingPoly* poly = dynamic_cast<BoundingPoly*>(a))
	{
		PolyToMeta(poly, b);
	}
}
void CollisionHandler::SphereToMeta(BoundingSphere* a, MetaSurface* m)
{
	vector<BoundingSphere>& spheres = m->GetBoundSpheres();
	vector<ColManifold> data;
	vector<int> ids;
	for (int i = 0; i < spheres.size(); i++)
	{
		ColManifold d = SphereToSphereData(a, &spheres[i]);
		if (d.col)
		{
			ids.push_back(i);
			data.push_back(d);
		}
	}
	for (int i = 0; i < data.size(); i++)
	{
		Transform* tA = data[i].refObj->GetTransform();
		BoundingSphere* b = &spheres[ids[i]];
		Transform* tB = data[i].incObj->GetTransform();

		int c = 0;
		if (tB->IsDynamic())
			c += 2;
		if (tA->IsDynamic())
			c++;

		if (c == 1)
		{
			tA->Translate((data[i].mtv * -data[i].depth) / data.size());
		}
		else if (c == 2)
		{
			tB->Translate((data[i].mtv * data[i].depth) / data.size());
		}
		else if (c == 3)
		{
			tA->Translate(data[i].mtv * -(data[i].depth / (2.0f * data.size())));
			tB->Translate(data[i].mtv * (data[i].depth / (2.0f * data.size())));
		}
	}
	for (int i = 0; i < data.size(); i++)
	{
		SphereToSphere(a, &spheres[ids[i]], data[i], data.size(), data.size());
	}
}

// Not used
void CollisionHandler::PolyToMeta(BoundingPoly* a, MetaSurface* m)
{
	vector<BoundingSphere>& spheres = m->GetBoundSpheres();
	vector<ColManifold> data;
	vector<int> ids;
	for (int i = 0; i < spheres.size(); i++)
	{
		ColManifold d = SphereToPolyData(&spheres[i], a);
		if (d.col)
		{
			ids.push_back(i);
			data.push_back(d);
		}
	}
	for (int i = 0; i < data.size(); i++)
	{
		Transform* tA = data[i].refObj->GetTransform();
		BoundingSphere* b = &spheres[ids[i]];
		Transform* tB = data[i].incObj->GetTransform();

		int c = 0;
		if (tB->IsDynamic())
			c += 2;
		if (tA->IsDynamic())
			c++;

		if (c == 1)
		{
			tA->Translate(data[i].mtv * -data[i].depth);
		}
		else if (c == 2)
		{
			tB->Translate(data[i].mtv * data[i].depth);
		}
		else if (c == 3)
		{
			tA->Translate(data[i].mtv * -(data[i].depth / 2.0f));
			tB->Translate(data[i].mtv * (data[i].depth / 2.0f));
		}
	}
	for (int i = 0; i < data.size(); i++)
	{
		SphereToPoly(&spheres[ids[i]], a, data[i]);
	}
}
