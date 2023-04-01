#version 440 core

layout (location = 0) in vec3 in_position;

layout (binding = 0) uniform MVP 
{
	mat4 model;
	mat4 view;
	mat4 projection;
	vec3 eyePosition;
};

void main()
{
    gl_Position = model * vec4(in_position, 1.f);
}