#version 450 core

layout(location = 0) out vec4 out_color;

layout(location = 1) in vec3 pos; 
layout(location = 2) in vec3 norm;

uniform vec3 lightDir;

void main(void)
{
	float diff = max(dot(-norm, lightDir), 0.0);
	vec3 color = vec3(0.8f, 0.8f, 0.8f) * diff;

	out_color = vec4(color, 1.0f);
}