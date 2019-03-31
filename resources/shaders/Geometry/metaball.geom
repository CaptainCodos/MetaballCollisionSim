#version 450 core
//#extension GL_EXT_gpu_shader4 : enable
//#extension GL_EXT_geometry_shader4 : enable
//#extension GL_EXT_geometry_shader : enable

#define PI 3.1415926538

layout (points) in;
layout (points, max_vertices = 1) out;

layout(location = 1) out vec3 pos; 
layout(location = 2) out vec3 norm;

uniform samplerBuffer metaballBuffer;
uniform sampler1D metaballs;
uniform sampler1D rads;
uniform int number;
uniform float threshold;
uniform float cellSize;

uniform mat4 M;
uniform mat4 view;
uniform mat4 projection;

//const int edgeTable[14][8] = int[14][8]
//(
//	int[8](0,1,0,2,0,3,-1,-1),
//	int[8](1,2,1,0,1,3,-1,-1),
//	int[8](1,2,2,0,1,3,0,3),
//	int[8](2,3,2,0,2,1,-1,-1),
//	int[8](2,3,3,0,2,1,0,1),
//	int[8](2,3,2,0,1,3,0,1),
//	int[8](1,3,0,3,2,3,-1,-1),
//	int[8](3,2,3,0,3,1,-1,-1),
//	int[8](3,1,1,0,3,2,0,2),
//	int[8](3,2,3,0,1,2,1,0),
//	int[8](3,2,0,2,1,2,-1,-1),
//	int[8](1,2,2,0,1,3,0,3),
//	int[8](2,1,0,1,3,1,-1,-1),
//	int[8](1,0,2,0,3,0,-1,-1)
//);

vec4 SamplePositions(int idx);
vec4 SamplePositions(int idx)
{
	float unit = (1.0f / float(number));
	float h = unit / 2.0f;

	float s = h + (unit * idx);

	return texture(metaballs, s).rgba;
}

vec4 SampleRads(int idx);
vec4 SampleRads(int idx)
{
	float unit = (1.0f / float(number));
	float h = unit / 2.0f;

	float s = h + (unit * idx);

	return texture(rads, s).rgba;
}


vec3 GetNormal(vec3 p);
vec3 GetNormal(vec3 p)
{
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;
	
	float limDist = 5.0f;
	vec3 v = vec3(0.0f, 0.0f, 0.0f);
	float minD = 100000.0f;
	for (int i = 0; i < number; ++i)
	{
		vec4 curr = SamplePositions(i);
		//vec4 currRad = SampleRads(i);
		float r = curr.a;
		
		float dx = (p.x - curr.r);
		float dy = (p.y - curr.g);
		float dz = (p.z - curr.b);
		
		float l = length(vec3(dx, dy, dz));
		
		//if (l < r)
		//{
		//	if (l < minD)
		//	{
		//		minD = l;
		//		v = vec3(dx, dy, dz);
		//	}
		//}
		
		//if (l < limDist)
		//{
		//	x += dx/l;
		//	y += dy/l;
		//	z += dz/l;
		//}
		
		v.x += 2 * dx / pow(pow(dx, 2) + pow(dy, 2) +pow(dz, 2), 2);
		v.y += 2 * dy / pow(pow(dx, 2) + pow(dy, 2) +pow(dz, 2), 2);
		v.z += 2 * dz / pow(pow(dx, 2) + pow(dy, 2) +pow(dz, 2), 2);
	}
	
	return normalize(v);
}

bool SampleField(vec3 p);
bool SampleField(vec3 p)
{
	float scalarVal = 0.0f;
	
	float route = pow(threshold, 0.5f);
	float rBase = 1.0f / route;
	float upperBase = pow(2.0, 1.0f / 4.0f);
	float routeBase = pow(rBase, 0.5f);
	
	float threshSqr = pow(threshold, 2.0f);
	float route2 = pow(2.0f, 0.5f);
	
	float limDist = 5.0f;
	float lOuter = limDist - rBase;
	float lim = pow(limDist, 2.0f);
	float count = 0;
	for (int i = 0; i < number; ++i)
	{
		//if (scalarVal >= threshold)
			//return true;
		
		vec4 curr = SamplePositions(i);
		//vec4 currRad = SampleRads(i);
		float r = curr.a;
		
		float dx = (p.x - curr.r);
		float dy = (p.y - curr.g);
		float dz = (p.z - curr.b);
		
		float dL = (pow(dx, 2) + pow(dy, 2) + pow(dz, 2));
		float l = length(vec3(dx, dy, dz));
		
		
		if (dL < lim)
		{
			//float val = 1.0f / (dL);
			float l = length(vec3(dx, dy, dz));
			
			// Finalized Attemp 3 ##############################
			//float first = (-2.0f * atan(2.0f * (l - rBase))) / PI;
			//float second = (2.0f * atan(2.0f * (limDist - rBase))) / PI;
			//scalarVal += (rBase / second) * (first + second);
			
			// Finalized Attemp 2 ##############################
			/*if (l > rBase)
			{
				
				float lerp = (l - rBase) / lOuter;
				float val = (rBase) * (1.0f - pow(lerp, 0.5f));
				scalarVal += val;
			}
			else
			{
				float f = (l - rBase) / rBase;
				
				scalarVal += ((rBase) + pow(-f, 0.5f) * (upperBase - rBase));
				//scalarVal += (rBase);
				
				//return true;
			}*/
			
			// Finalized Attemp 1 ##############################
			/*if (l <= r)
			{
				return true;
			}
			else
			{
				if (l > rBase)
				{
					
					float lerp = (l - rBase) / lOuter;
					scalarVal += (upperBase - rBase) * (1.0f - pow(lerp, 0.5f));
					//scalarVal += val;
				}
				else
				{
					float f = (l - rBase) / rBase;
					
					scalarVal += (rBase + pow(-f, 0.5f) * ((upperBase) - rBase));
					
					//return true;
				}
			}*/
			
		}
		
		if (l <= r)
			return true;
	}
	
	//return scalarVal >= threshold;
	return false;
}

void CreateVertex(vec3 p);
void CreateVertex(vec3 p)
{
	gl_Position = projection * view * vec4(p, 1.0f);
	norm = GetNormal(p);
	pos = vec3(M * vec4(p, 1.0f));
	EmitVertex();
}
void CreateMesh(vec3 p, int i);
void CreateMesh(vec3 p, int i)
{

	if (SampleField(p))
	{
		CreateVertex(p);
		EndPrimitive();
	}
}

void main(void)
{
	int i;

    for (i = 0; i < gl_in.length(); i++)
    {
        CreateMesh(vec3(gl_in[i].gl_Position), 0);
    }
}