#pragma once
// Math constants
#define _USE_MATH_DEFINES
#include <cmath>  
#include <random>

// Std. Includes
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <windows.h>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/matrix_operation.hpp>
#include "glm/ext.hpp"


// Other Libs
#include "SOIL2/SOIL2.h"

// project includes
#include "CreateTexture.h"
#include "Application.h"
#include "Shader.h"
#include "Mesh.h"
#include "MetaSurface.h"
#include "BoundingSphere.h"
#include "CollisionHandler.h"

#include "Spring.h"
#include "Octree.h"

// time
GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;
GLfloat t = 0.0f;

string paths[5] = { "resources//shaders//Vertex//core.vert", "", "", "", "resources//shaders//Fragment//core.frag" };
int flags[5] = { 1, 0, 0, 0, 1 };

GLuint tbo;
GLuint tbo_tex;
GLuint txr;
GLuint txr2;

bool drawCollisionSpheres = false;

using namespace std;
using namespace glm;

void OpenStream(string path, ofstream &file)
{
	file.open(path);
}

void WriteLine(string line, ofstream &file)
{
	file << (line + "\n");
}

void CloseStream(ofstream &file)
{
	file.close();
}

float Rand(float min, float max)
{
	float range = max - min;
	return fmod((float)rand(), range) + min;
}
void CreateSphere(vector<BoundingObject*>& objs, vector<Mesh>& meshes, float r, vec3 p, vec3 iVel)
{
	Mesh b = Mesh(Sphere);
	b.GetTransform()->SetPos(p);
	b.GetTransform()->AddImpulse(glm::vec3(0.0f, 0.0f, 0.0f), iVel);
	b.setShader(Shader(paths, flags));
	b.GetTransform()->SetDrag(0.05f);
	meshes.push_back(b);

	BoundingSphere* a = new BoundingSphere(b.GetTransform(), r);
	objs.push_back(a);
}

void Demo1(MetaSurface* surf, vector<BoundingObject*>& objs, vector<Mesh>& meshes, vector<Spring>& springs)
{
	for (int i = 0; i < 20; i++)
	{
		CreateSphere(objs, meshes, 1.0f, vec3(Rand(-0.5f, 30.5f), 15.0f + (i * 2.0f), Rand(-0.5f, 30.5f)), vec3(0.0f, 0.0f, 0.0f));
	}

	for (int z = 0; z < 10; z++)
	{
		for (int x = 0; x < 10; x++)
		{
			Metaball m = Metaball();
			m.GetTransform()->SetDynamic(true);
			m.SetCellSize(0.5);
			m.setPos(vec3(x * 3.0f, 0.0f, z * 3.0f));
			m.GetTransform()->SetDrag(0.3f);
			surf->AddMetaball(m);
		}
	}

	vector<BoundingSphere>& metaBalls = surf->GetBoundSpheres();

	for (int z = 1; z < 9; z++)
	{
		for (int x = 1; x < 9; x++)
		{
			int ids[4] = {
				(x - 1) + (10 * (z)),
				(x)+(10 * (z + 1)),
				(x + 1) + (10 * (z)),
				(x)+(10 * (z - 1))
			};


			int i = x + (10 * z);

			Transform* tI = metaBalls[i].GetTransform();

			for (int i = 0; i < 4; i++)
			{
				Transform* t = metaBalls[ids[i]].GetTransform();

				Spring s = Spring(tI, t);
				s.SetLength(2.9f);
				s.SetConsts(5.0f, 20.0f);

				springs.push_back(s);
			}
		}
	}

	for (int x = 0; x < 10; x++)
	{
		int a = x;
		int b = x + 90;

		Transform* tA = metaBalls[a].GetTransform();
		Transform* tB = metaBalls[b].GetTransform();

		Transform* tA1 = new Transform();
		Transform* tB1 = new Transform();

		tA1->SetPos(tA->GetPosVec() + vec3(0.0f, 3.0f, 0.0f));
		tB1->SetPos(tB->GetPosVec() + vec3(0.0f, 3.0f, 0.0f));

		tA1->SetDynamic(false);
		tB1->SetDynamic(false);

		Spring s1 = Spring(tA, tA1);
		s1.SetLength(2.9f);
		s1.SetConsts(5.0f, 20.0f);

		Spring s2 = Spring(tB, tB1);
		s2.SetLength(2.9f);
		s2.SetConsts(5.0f, 20.0f);

		springs.push_back(s1);
		springs.push_back(s2);
	}

	for (int z = 0; z < 10; z++)
	{
		int a = (z * 10);
		int b = (z * 10) + 9;

		Transform* tA = metaBalls[a].GetTransform();
		Transform* tB = metaBalls[b].GetTransform();

		Transform* tA1 = new Transform();
		Transform* tB1 = new Transform();

		tA1->SetPos(tA->GetPosVec() + vec3(0.0f, 3.0f, 0.0f));
		tB1->SetPos(tB->GetPosVec() + vec3(0.0f, 3.0f, 0.0f));

		tA1->SetDynamic(false);
		tB1->SetDynamic(false);

		Spring s1 = Spring(tA, tA1);
		s1.SetLength(2.9f);
		s1.SetConsts(5.0f, 20.0f);

		Spring s2 = Spring(tB, tB1);
		s2.SetLength(2.9f);
		s2.SetConsts(5.0f, 20.0f);

		springs.push_back(s1);
		springs.push_back(s2);
	}

	/*vector<Metaball>& metaBalls = surf->GetMetaballs();

	metaBalls[0].GetTransform()->SetPos(vec3(15.0f, 1.0f, 15.05 - 8.0f));*/
}
void Demo2(MetaSurface* surf, vector<BoundingObject*>& objs, vector<Mesh>& meshes, vector<Spring>& springs)
{
	vec3 dirs[3] = { vec3(2.0f, -1.0f, 0.0f), vec3(0.0f, -1.0f, 2.0f), vec3(-2.0f, -1.0f, 0.0f) };
	vec3 starts[3] = { vec3(0.0f, 0.0f, 0.0f), (dirs[0] * 10.0f) + dirs[1], (dirs[0] * 10.0f) + dirs[1] + (dirs[1] * 10.0f) + dirs[2] };

	float sideHeight = 2.5f;
	int ids[3] = { 2, 0, 2};
	vec3 sides[3] = { vec3(0.0f, sideHeight, 2.5f), vec3(2.5f, sideHeight, 0.0f), vec3(0.0f, sideHeight, 2.5f) };
	
	vec3 corners[2] = { (dirs[0] * 10.0f), (dirs[0] * 10.0f) + dirs[1] + (dirs[1] * 10.0f) };
	
	for (int i = 0; i < 3; i++)
	{
		for (int k = 0; k < 10; k++)
		{
			vec3 sideA = sides[i];
			vec3 sideB = sides[i];
			sideB[ids[i]] *= -1.0f;

			Metaball m1 = Metaball();
			m1.GetTransform()->SetDynamic(false);
			m1.SetCellSize(0.5);
			m1.setPos(starts[i] + (k * dirs[i]));
			m1.GetTransform()->SetDrag(0.3f);
			surf->AddMetaball(m1);

			Metaball m2 = Metaball();
			m2.GetTransform()->SetDynamic(false);
			m2.SetCellSize(0.5);
			m2.setPos(starts[i] + (k * dirs[i]) + sideA);
			m2.GetTransform()->SetDrag(0.3f);
			surf->AddMetaball(m2);

			Metaball m3 = Metaball();
			m3.GetTransform()->SetDynamic(false);
			m3.SetCellSize(0.5);
			m3.setPos(starts[i] + (k * dirs[i]) + sideB);
			m3.GetTransform()->SetDrag(0.3f);
			surf->AddMetaball(m3);
		}
	}

	for (int i = 0; i < 2; i++)
	{
		Metaball m1 = Metaball();
		m1.GetTransform()->SetDynamic(false);
		m1.SetCellSize(0.5);
		m1.setPos(corners[i]);
		m1.GetTransform()->SetDrag(0.3f);
		surf->AddMetaball(m1);

		Metaball m2 = Metaball();
		m2.GetTransform()->SetDynamic(false);
		m2.SetCellSize(0.5);
		m2.setPos(corners[i]);
		m2.GetTransform()->SetDrag(0.3f);
		surf->AddMetaball(m2);
	}

	Metaball m1 = Metaball();
	m1.GetTransform()->SetDynamic(false);
	m1.SetCellSize(0.5);
	m1.setPos(corners[0] + vec3(0.0f, sideHeight, -2.5f));
	m1.GetTransform()->SetDrag(0.3f);
	surf->AddMetaball(m1);
	
	Metaball m2 = Metaball();
	m2.GetTransform()->SetDynamic(false);
	m2.SetCellSize(0.5);
	m2.setPos(corners[0] + vec3(2.5f, sideHeight, 0.0f));
	m2.GetTransform()->SetDrag(0.3f);
	surf->AddMetaball(m2);

	Metaball m3 = Metaball();
	m3.GetTransform()->SetDynamic(false);
	m3.SetCellSize(0.5);
	vec3 p1 = glm::normalize(vec3(1.0f, 0.0f, -1.0f));
	p1 *= 2.0f;
	p1[1] += sideHeight;
	m3.setPos(corners[0] + p1);
	m3.GetTransform()->SetDrag(0.3f);
	surf->AddMetaball(m3);

	Metaball m4 = Metaball();
	m4.GetTransform()->SetDynamic(false);
	m4.SetCellSize(0.5);
	m4.setPos(corners[1] + vec3(2.5f, sideHeight, 0.0f));
	m4.GetTransform()->SetDrag(0.3f);
	surf->AddMetaball(m4);
	
	Metaball m5 = Metaball();
	m5.GetTransform()->SetDynamic(false);
	m5.SetCellSize(0.5);
	m5.setPos(corners[1] + vec3(0.0f, sideHeight, 2.5f));
	m5.GetTransform()->SetDrag(0.3f);
	surf->AddMetaball(m5);

	Metaball m6 = Metaball();
	m6.GetTransform()->SetDynamic(false);
	m6.SetCellSize(0.5);
	vec3 p2 = glm::normalize(vec3(1.0f, 0.0f, 1.0f));
	p2 *= 2.0f;
	p2[1] += sideHeight;
	m6.setPos(corners[1] + p2);
	m6.GetTransform()->SetDrag(0.3f);
	surf->AddMetaball(m6);
}
void Demo3(MetaSurface* surf, vector<BoundingObject*>& objs, vector<Mesh>& meshes, vector<Spring>& springs)
{
	for (int i = 0; i < 10; i++)
	{
		vec3 c = vec3(3.0f * 4.0f, 0.0f, 3.0f * 4.0f);
		vec3 s = vec3(Rand(0.5f, (3.0f * 9) - 0.5f), -1.0f + (i * 1.0f), Rand(0.5f, (3.0f * 9) - 0.5f));

		vec3 d = c - s;
		d.y = 0.0f;
		d.x += Rand(-2.5f, 2.5f);
		d.z += Rand(-2.5f, 2.5f);
		CreateSphere(objs, meshes, 1.0f, s, d);
	}

	for (int z = 0; z < 9; z++)
	{
		for (int x = 0; x < 9; x++)
		{
			vec3 c = vec3(3.0f * 4.0f, 0.0f, 3.0f * 4.0f);
			vec3 p = vec3(x * 3.0f, 0.0f, z * 3.0f);

			vec3 d = c - p;
			float r = glm::length(d) / 2.0f;

			p.y -= r;

			Metaball m = Metaball();
			m.GetTransform()->SetDynamic(true);
			m.SetCellSize(0.5);
			m.setPos(vec3(x * 3.0f, Rand(0.0f, 2.0f), z * 3.0f));
			m.GetTransform()->SetDrag(0.3f);
			surf->AddMetaball(m);
		}
	}
}
void Demo4(MetaSurface* surf, vector<BoundingObject*>& objs, vector<Mesh>& meshes, vector<Spring>& springs)
{
	CreateSphere(objs, meshes, 1.0f, vec3(-35.0f, 0.0f, 0.0f), vec3(24.0f, 0.0f, 0.0f));

	Metaball m = Metaball();
	m.GetTransform()->SetDynamic(true);
	m.SetCellSize(0.5);
	m.setPos(vec3(10.0f, 0.5f, 0.0f));
	m.GetTransform()->SetDrag(0.3f);
	surf->AddMetaball(m);

	Metaball m2 = Metaball();
	m2.GetTransform()->SetDynamic(true);
	m2.SetCellSize(0.5);
	m2.setPos(vec3(3.0f, 0.0f, 0.0f));
	m2.GetTransform()->SetDrag(0.3f);
	surf->AddMetaball(m2);

	Metaball m3 = Metaball();
	m3.GetTransform()->SetDynamic(true);
	m3.SetCellSize(0.5);
	m3.setPos(vec3(14.0f, 2.0f, 0.0f));
	m3.GetTransform()->SetDrag(0.3f);
	surf->AddMetaball(m3);
}
void ControlA(MetaSurface* surf, vector<BoundingObject*>& objs, vector<Mesh>& meshes, vector<Spring>& springs)
{
	CreateSphere(objs, meshes, 1.0f, vec3(0.0f, 15.0f, 0.0f), vec3(0.0f, 0.0f, 0.0f));

	Metaball m = Metaball();
	m.GetTransform()->SetDynamic(false);
	m.SetCellSize(0.5);
	m.setPos(vec3(0.0f, 0.0f, 0.0f));
	m.GetTransform()->SetDrag(0.3f);
	surf->AddMetaball(m);
}
void SetupA(MetaSurface* surf, vector<BoundingObject*>& objs, vector<Mesh>& meshes, vector<Spring>& springs, int spheres, int metas)
{
	for (int i = 0; i < spheres; i++)
	{
		CreateSphere(objs, meshes, 1.0f, vec3(Rand(-0.5f, (3.0f * metas) + 0.5f), 15.0f + (i * 2.0f), Rand(-0.5f, (3.0f * metas) + 0.5f)), vec3(0.0f, 0.0f, 0.0f));
	}
	
	for (int z = 0; z < metas; z++)
	{
		for (int x = 0; x < metas; x++)
		{
			Metaball m = Metaball();
			m.GetTransform()->SetDynamic(false);
			m.SetCellSize(0.5);
			m.setPos(vec3(x * 3.0f, 0.0f, z * 3.0f));
			m.GetTransform()->SetDrag(0.3f);
			surf->AddMetaball(m);
		}
	}
}

void ControlB(MetaSurface* surf, vector<BoundingObject*>& objs, vector<Mesh>& meshes, vector<Spring>& springs)
{
	CreateSphere(objs, meshes, 1.0f, vec3(0.0f, 15.0f, 0.0f), vec3(0.0f, 0.0f, 0.0f));

	Metaball m = Metaball();
	m.GetTransform()->SetDynamic(true);
	m.SetCellSize(0.5);
	m.setPos(vec3(0.0f, 0.0f, 0.0f));
	m.GetTransform()->SetDrag(0.3f);
	surf->AddMetaball(m);
}
void SetupB(MetaSurface* surf, vector<BoundingObject*>& objs, vector<Mesh>& meshes, vector<Spring>& springs, int spheres, int metas)
{
	for (int i = 0; i < spheres; i++)
	{
		CreateSphere(objs, meshes, 1.0f, vec3(Rand(-0.5f, (3.0f * metas) + 0.5f), 15.0f + (i * 2.0f), Rand(-0.5f, (3.0f * metas) + 0.5f)), vec3(0.0f, 0.0f, 0.0f));
	}

	for (int z = 0; z < metas; z++)
	{
		for (int x = 0; x < metas; x++)
		{
			Metaball m = Metaball();
			m.GetTransform()->SetDynamic(true);
			m.SetCellSize(0.5);
			m.setPos(vec3(x * 3.0f, 0.0f, z * 3.0f));
			m.GetTransform()->SetDrag(0.3f);
			surf->AddMetaball(m);
		}
	}
}

// main function
int main()
{
	int dims;
	int NoSpheres;
	float thresh;
	float fMult;
	float dens;
	string exper;
	char format;
	char run;

	float avFPS = 0.0f;

	std::cout << "Please enter experiment (cA, cB, A, B or Demo): ";
	std::cin >> exper;

	if (exper == "Demo")
	{
		std::cout << "Please enter format (1, 2, 3, 4, 5 or 6): ";
		std::cin >> format;

		std::cout << "Please enter surface threshold: ";
		std::cin >> thresh;

		std::cout << "Please enter a funtion multiplier for field evaluation: ";
		std::cin >> fMult;

		std::cout << "Please enter a field density: ";
		std::cin >> dens;
	}
	else
	{
		std::cout << "Please enter format (1, 2 or 3): ";
		std::cin >> format;
		std::cout << "Please enter experiment (a, b or c): ";
		std::cin >> run;

		std::cout << "Please enter surface dimensions in metaballs: ";
		std::cin >> dims;
		std::cout << "Please enter surface threshold: ";
		std::cin >> thresh;
		std::cout << "Please enter a funtion multiplier for field evaluation: ";
		std::cin >> fMult;
		std::cout << "Please enter a field density: ";
		std::cin >> dens;
		std::cout << "Please enter number of spheres to drop: ";
		std::cin >> NoSpheres;
	}
	

	
	//Sleep(1);
	// create application
	Application app = Application::Application();
	app.initRender();
	Application::camera.setCameraPosition(glm::vec3(0.0f, 5.0f, 20.0f));

	std::srand(glfwGetTime());

	Octree octree = Octree(3, vec3(10.0f, 10.0f, 10.0f));

	vector<Mesh> meshes;
	vector<BoundingObject*> objs;
	vector<Spring> springs;
	
	float height = 10.0f;
	vec3 pos = vec3(0.0f, 0.0f, 0.0f);
	vec3 surfScale = vec3(10.0f, height, 10.0f);

	if (exper == "Demo")
	{
		if (format == '1')
		{
			dims = 10;
			height = 30.0f;

			pos = vec3((3.0f * dims) / 2.0f, 0.0f, (3.0f * dims) / 2.0f);
			surfScale = vec3((3.0f * dims) + (dims / 4.0f), height, (3.0f * dims) + (dims / 4.0f));
		}
		else if (format == '2')
		{
			dims = 10;
			height = 30.0f;

			pos = vec3((3.0f * dims) / 2.0f, 0.0f, (3.0f * dims) / 2.0f);
			surfScale = vec3((3.0f * dims) + (dims / 4.0f), height, (3.0f * dims) + (dims / 4.0f));
		}
		else if (format == '3')
		{
			dims = 10;
			height = 40.0f;

			pos = vec3((3.0f * dims) / 2.0f, 0.0f, (3.0f * dims) / 2.0f);
			surfScale = vec3((3.0f * dims) + (dims / 4.0f), height, (3.0f * dims) + (dims / 4.0f));
		}
		else if (format == '4')
		{
			dims = 15;
			height = 50.0f;

			pos = vec3((3.0f * dims) / 2.0f, -height / 2.0f, (3.0f * dims) / 2.0f);
			surfScale = vec3((3.0f * dims) + (dims / 4.0f), height * 1.2f, (3.0f * dims) + (dims / 4.0f));
		}
		else if (format == '5')
		{
			dims = 9;
			height = 40.0f;

			pos = vec3((3.0f * dims) / 2.0f, 0.0f, (3.0f * dims) / 2.0f);
			surfScale = vec3((3.0f * dims) + (dims / 4.0f), height * 1.2f, (3.0f * dims) + (dims / 4.0f));
		}
		else if (format == '6')
		{
			dims = 20;
			height = 20.0f;

			pos = vec3((3.0f * dims) / 2.0f, 0.0f, 0.0f);
			surfScale = vec3((3.0f * dims) + (dims / 4.0f), height * 1.2f, 10.0f);
		}
	}
	else
	{
		if (format == '1')
		{
			//dims = 10;
			height = 20.0f;

			pos = vec3((3.0f * dims) / 2.0f, 0.0f, (3.0f * dims) / 2.0f);
			surfScale = vec3((3.0f * dims) + (dims / 4.0f), height, (3.0f * dims) + (dims / 4.0f));
		}
		else if (format == '2')
		{
			//dims = 20;
			height = 20.0f;

			pos = vec3((3.0f * dims) / 2.0f, 0.0f, (3.0f * dims) / 2.0f);
			surfScale = vec3((3.0f * dims) + (dims / 4.0f), height, (3.0f * dims) + (dims / 4.0f));
		}
		else if (format == '3')
		{
			//dims = 30;
			height = 40.0f;

			pos = vec3((3.0f * dims) / 2.0f, 0.0f, (3.0f * dims) / 2.0f);
			surfScale = vec3((3.0f * dims) + (dims / 4.0f), height, (3.0f * dims) + (dims / 4.0f));
		}
	}

	/*float density = 1.0f;
	if (format == '6')
		density = 8.0f;*/

	// Create metaballs
	MetaSurface* surface = new MetaSurface(thresh, fMult, pos, surfScale + (vec3(1.0f) * (1.0f / pow(thresh, 2.0f))), dens);
	
	bool controlBegun = false;
	ofstream file;
	int samples = 0;
	int spheresMade = 0;
	float timer = 2.0f;

	// Call setups
	if (exper != "Demo")
	{
		if (exper == "A")
		{
			string Path = "resources\\Setup" + exper + format + run;
			string end = ".csv";
			string str = Path + end;
			OpenStream(str, file);

			SetupA(surface, objs, meshes, springs, NoSpheres, dims);
		}
		else if (exper == "B")
		{
			string Path = "resources\\Setup" + exper + format + run;
			string end = ".csv";
			string str = Path + end;
			OpenStream(str, file);

			SetupB(surface, objs, meshes, springs, NoSpheres, dims);
		}
		else if (exper == "cA")
		{
			string Path = "resources\\ControlA";
			string end = ".csv";
			string str = Path + end;
			OpenStream(str, file);

			ControlA(surface, objs, meshes, springs);
		}
		else if (exper == "cB")
		{
			string Path = "resources\\ControlB";
			string end = ".csv";
			string str = Path + end;
			OpenStream(str, file);

			ControlB(surface, objs, meshes, springs);
		}
	}
	else
	{
		if (format == '1')
		{
			Demo1(surface, objs, meshes, springs);
		}
		else if (format == '2')
		{
			Demo1(surface, objs, meshes, springs);
		}
		else if (format == '3')
		{
			Demo1(surface, objs, meshes, springs);

			vector<BoundingSphere>& metaBalls = surface->GetBoundSpheres();

			for (int x = 0; x < 2; x++)
			{
				for (int z = 0; z < 2; z++)
				{
					Transform* tA = metaBalls[((4 + z) * 10) + (4 + x)].GetTransform();
					Transform* tS = new Transform();
					tS->SetPos(tA->GetPosVec() + vec3(0.0f, 100.0f, 0.0f));

					Spring s1 = Spring(tA, tS);
					s1.SetLength(1.0f);
					s1.SetConsts(1.0f, 50.0f);
					springs.push_back(s1);
				}
			}
			
		}
		else if (format == '4')
		{
			Demo2(surface, objs, meshes, springs);
		}
		else if (format == '5')
		{
			Demo3(surface, objs, meshes, springs);
		}
		else if (format == '6')
		{
			Demo4(surface, objs, meshes, springs);
		}
	}

	surface->UpdateRads();

	vector<Mesh> bounds;
	vector<BoundingSphere>& surfSpheres = surface->GetBoundSpheres();

	for (int i = 0; i < surfSpheres.size(); i++)
	{
		Mesh s = Mesh(Sphere);
		s.GetTransform()->SetPos(surfSpheres[i].GetTransform()->GetPosVec());
		s.setShader(Shader(paths, flags));
		bounds.push_back(s);
	}

	vector<float> posVals;
	vector<float> radii;

	// time
	GLfloat firstFrame = (GLfloat) glfwGetTime();
	
	// Game loop
	while (!glfwWindowShouldClose(app.getWindow()))
	{
		// Set frame time
		GLfloat currentFrame = (GLfloat)  glfwGetTime() - firstFrame;
		// the animation can be sped up or slowed down by multiplying currentFrame by a factor.
		currentFrame *= 1.0f;
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		t += deltaTime;
		/*
		**	INTERACTION
		*/
		// Manage interaction
		app.doMovement(deltaTime);

		if (Application::keys[GLFW_KEY_1])
			drawCollisionSpheres = true;
		else if (Application::keys[GLFW_KEY_2])
			drawCollisionSpheres = false;

		vector<BoundingSphere>& spheres = surface->GetBoundSpheres();

		if (!controlBegun)
		{
			for (int i = 0; i < spheres.size() && !controlBegun; i++)
			{
				for (int k = 0; k < objs.size() && !controlBegun; k++)
				{
					ColManifold col = CollisionHandler::Get()->SphereToSphereData(&spheres[i], dynamic_cast<BoundingSphere*>(objs[k]));

					if (col.col)
						controlBegun = true;
				}
			}
		}

		// do collisions
		for (int i = 0; i < objs.size(); i++)
		{
			for (int k = i + 1; k < objs.size(); k++)
			{
				CollisionHandler::Get()->CollidePair(objs[i], objs[k]);
			}
		}
		for (int i = 0; i < objs.size(); i++)
		{
			CollisionHandler::Get()->CollideMeta(objs[i], surface);
		}

		for (int i = 0; i < objs.size(); i++)
		{
			if (format != '6')
				objs[i]->GetTransform()->AddForce(vec3(0.0f, -9.81f, 0.0f));
		}

		for (int i = 0; i < objs.size(); i++)
		{
			objs[i]->GetTransform()->Update(deltaTime);
		}

		vector<Metaball>& metaBalls = surface->GetMetaballs();

		if (exper == "Demo")
		{
			if (format == '1')
			{
				for (int i = 0; i < metaBalls.size(); i++)
				{
					Transform* tM = metaBalls[i].GetTransform();
					tM->AddForce(vec3(0.0f, -8.0f * tM->GetMass(), 0.0f));
				}
				for (int i = 0; i < springs.size(); i++)
				{
					springs[i].ApplyForce();
				}
			}
			else if (format == '2')
			{
				for (int i = 0; i < metaBalls.size(); i++)
				{
					Transform* tM = metaBalls[i].GetTransform();
					tM->AddForce(vec3(5.0f  * sin(1.3f * t), -8.0f * tM->GetMass(), 0.0f));
				}
				for (int i = 0; i < springs.size(); i++)
				{
					springs[i].ApplyForce();
				}
			}
			else if (format == '3')
			{
				for (int i = 0; i < metaBalls.size(); i++)
				{
					Transform* tM = metaBalls[i].GetTransform();
					tM->AddForce(vec3(0.0f, -5.0f * tM->GetMass(), 0.0f));
				}
				for (int i = 0; i < springs.size() - 4; i++)
				{
					springs[i].ApplyForce();
				}

				float tmpT = fmod(t, 10.0f);
				if (tmpT > 9.5f)
				{
					springs[springs.size() - 1].ApplyForce();
					springs[springs.size() - 2].ApplyForce();
					springs[springs.size() - 3].ApplyForce();
					springs[springs.size() - 4].ApplyForce();
				}
			}
			else if (format == '4')
			{
				if (spheresMade < 20)
				{
					if (timer <= 0.0f)
					{
						timer = 2.0f;

						CreateSphere(objs, meshes, 1.0f, vec3(2.5f, 2.0f, 0.0f), vec3());

						spheresMade++;
					}
					else
					{
						timer -= deltaTime;
					}
				}
			}
			else if (format == '5')
			{
				/*for (int i = 0; i < metaBalls.size(); i++)
				{
					Transform* tM = metaBalls[i].GetTransform();
					tM->AddForce(vec3(0.0f, (-5.0f * tM->GetMass()), 0.0f));

					if (t > 5.0f)
					{
						tM->AddForce(vec3(4.0f  * sin(t - 5.0f), 4.0f  * sin(t - 5.0f + (tM->GetPosVec()[2] / 2.0f)), 0.0f));
					}
				}
				for (int i = 0; i < springs.size() - 1; i++)
				{
					springs[i].ApplyForce();
				}*/
			}
		}

		surface->Update(deltaTime);

		/*
		**	RENDER 
		*/		
		// clear buffer
		app.clear();

		// draw meshes
		for (int i = 0; i < meshes.size(); i++)
		{
			app.useMeshShader(meshes[i]);
			app.draw(meshes[i]);
		}

		// get metaball positions and gather into a texture for shader
		for (int i = 0; i < metaBalls.size(); i++)
		{
			posVals.push_back(metaBalls[i].GetTransform()->GetPosVec().x);
			posVals.push_back(metaBalls[i].GetTransform()->GetPosVec().y);
			posVals.push_back(metaBalls[i].GetTransform()->GetPosVec().z);
			posVals.push_back(spheres[i].GetRadius());

			/*radii.push_back(spheres[i].GetRadius());
			radii.push_back(1.0f);
			radii.push_back(1.0f);
			radii.push_back(1.0f);*/
		}

		txr = Create1DTexture(posVals, GL_RGBA);
		//txr2 = Create1DTexture(radii, GL_RGBA);

		// Draw surface
		app.useMeshShader(*surface);
		surface->SetPosUniforms(txr);
		app.draw(*surface);

		if (drawCollisionSpheres)
		{
			for (int i = 0; i < bounds.size(); i++)
			{
				Transform* t = bounds[i].GetTransform();
				t->SetPos(spheres[i].GetTransform()->GetPosVec());

				t->SetScale(vec3(spheres[i].GetRadius()));

				app.useMeshShader(bounds[i]);
				app.draw(bounds[i], vec4(0.3f, 0.6f, 0.6f, 0.5f));
			}
		}

		// Clear and delete data
		posVals.clear();
		glDeleteTextures(1, &txr);
		radii.clear();
		glDeleteTextures(1, &txr2);

		app.display();

		if (exper != "Demo")
		{
			if (samples < 1000 && controlBegun)
			{
				float fps = 1.0f / deltaTime;
				WriteLine(to_string(fps), file);
				avFPS += fps;
				samples++;
			}
			else if (samples >= 1000)
			{
				WriteLine(to_string(avFPS / 1000.0f), file);
				CloseStream(file);
				glfwSetWindowShouldClose(app.getWindow(), GLFW_TRUE);
			}
		}
		
	}

	app.terminate();

	return EXIT_SUCCESS;
}

