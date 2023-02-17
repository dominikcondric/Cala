#version 440 core

layout (location = 0) in vec3 inPosition;

out vec3 direction;

layout (binding = 0) uniform MVP 
{
	mat4 model;
	mat4 view;
	mat4 projection;
	vec3 eyePosition;
};

void main() 
{
	direction = inPosition;
	vec4 pos = projection * mat4(mat3(view)) * vec4(inPosition, 1.0);
	gl_Position = pos.xyww;
}