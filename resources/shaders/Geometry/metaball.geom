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
uniform float limits;
uniform float threshold;
uniform float cellSize;

uniform mat4 M;
uniform mat4 view;
uniform mat4 projection;

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
	float limDist = limits / 4.0f;
	vec3 v = vec3(0.0f, 0.0f, 0.0f);
	
	for (int i = 0; i < number; ++i)
	{
		vec4 curr = SamplePositions(i);
		float r = curr.a;
		
		float dx = (p.x - curr.r);
		float dy = (p.y - curr.g);
		float dz = (p.z - curr.b);
		
		float l = length(vec3(dx, dy, dz));
		
		if (l < limDist)
		{
			v.x += 2 * dx / pow(pow(dx, 2) + pow(dy, 2) +pow(dz, 2), 2);
			v.y += 2 * dy / pow(pow(dx, 2) + pow(dy, 2) +pow(dz, 2), 2);
			v.z += 2 * dz / pow(pow(dx, 2) + pow(dy, 2) +pow(dz, 2), 2);
		}
	}
	
	return normalize(v);
}

bool SampleField(vec3 p);
bool SampleField(vec3 p)
{
	float scalarVal = 0.0f;
	
	float route = pow(threshold, 0.5f);
	float rBase = 1.0f / route;
	
	float limDist = limits / 4.0f;
	
	for (int i = 0; i < number; ++i)
	{
		if (scalarVal >= threshold)
			return true;
		
		vec4 curr = SamplePositions(i);
		float r = curr.a;
		
		float dx = (p.x - curr.r);
		float dy = (p.y - curr.g);
		float dz = (p.z - curr.b);
		
		float dL = (pow(dx, 2) + pow(dy, 2) + pow(dz, 2));
		float l = length(vec3(dx, dy, dz));
		
		if (l <= limDist)
		{
			scalarVal += 1.0f /(dL);
		}
	}
	
	return scalarVal >= threshold;
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