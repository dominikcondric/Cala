#version 440 core

layout (location = 0) in vec3 in_position;
layout (location = 1) in vec3 in_normal;
layout (location = 2) in vec2 in_texCoords;

out Attributes {
	vec3 fragPosition;
	vec3 normal;
	vec2 texCoords;
} outAttributes;

layout (binding = 0) uniform MVP 
{
	mat4 model;
	mat4 view;
	mat4 projection;
	vec3 eyePosition;
};

void main() 
{
	mat3 normalMatrix;
	outAttributes.fragPosition = vec3(model * vec4(in_position, 1.0));
	outAttributes.normal = normalize(mat3(transpose(inverse(model))) * in_normal);
	outAttributes.texCoords = in_texCoords;
	gl_Position = projection * view * vec4(outAttributes.fragPosition, 1.0);
}