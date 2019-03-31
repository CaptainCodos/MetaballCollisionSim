#version 450 core

uniform mat4 model;

layout(location = 0) in vec3 in_position;

void main()
{
	gl_Position = model * vec4(in_position, 1.0);
}