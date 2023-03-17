#version 440 core

layout (location = 0) in vec3 in_position;
layout (location = 1) in vec3 in_normal;
layout (location = 2) in vec2 in_texCoords;
layout (location = 3) in vec3 in_tangent;

out Attributes {
	vec3 fragPosition;
	mat3 TBN;
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
	const mat3 normalMatrix = mat3(transpose(inverse(model)));
	const vec3 normal = normalize(normalMatrix * in_normal);
	const vec3 tangent = normalize(normalMatrix * in_tangent);
	const vec3 bitangent = normalize(cross(normal, tangent));

	outAttributes.fragPosition = vec3(model * vec4(in_position, 1.0));
	outAttributes.TBN = mat3(tangent, bitangent, normal);
	outAttributes.texCoords = in_texCoords;

	gl_Position = projection * view * vec4(outAttributes.fragPosition, 1.0);
}