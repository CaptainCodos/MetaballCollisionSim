#version 450 core

layout(location = 0) out vec4 out_color;

in vec4 color;
uniform samplerBuffer metaballBuffer;
uniform int number;
uniform float threshold;

float SampleScalarField(vec3 pos)
{
	float scalarVal = 0.0f;
	
	for (int i = 0; i < number && scalarVal < threshold; ++i)
	{
		vec4 curr = texelFetchBuffer(metaballBuffer, i);
		
		scalarVal += (1.0f / pow(curr.x, 2) + pow(curr.y, 2) +pow(curr.z, 2));
	}
	
	return scalarVal;
}

float FurthestMetaDist(vec3 pos)
{
	float maxVal = 0.0f;
	
	for (int i = 0; i < number; ++i)
	{
		vec4 curr = texelFetchBuffer(metaballBuffer, i);
		vec3 dist = curr.xyz - pos;
		float val = pow(dist.x, 2) + pow(dist.y, 2) +pow(dist.z, 2);
		
		if (val > maxVal)
			maxVal = val;
	}
	
	return maxVal;
}

void main(void)
{
	u = (gl_FragCoord.x + drand48(col.xy + s)) / window_size.x;
      v = (gl_FragCoord.y + drand48(col.xz + s)) / window_size.y;
      r = get_ray(u, v);
	  
	  
	out_color = color;
}