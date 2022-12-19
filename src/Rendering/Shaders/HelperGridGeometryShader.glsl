#version 440 core
#define VERTEX_COUNT 202 * 2 // 101 line per dimension, 2 vertices per line

layout (lines) in;
layout (line_strip, max_vertices = VERTEX_COUNT) out;

layout (binding = 0) uniform MVP
{
	mat4 model;
	mat4 view;
	mat4 projection;
	vec3 eyePosition;
};

vec4 transformWithMVP(in vec4 vertexPosition) 
{
	return projection * view * model * vertexPosition;
}

void main()
{
	vec4 orthogonalLine[2] = vec4[2](
		vec4(gl_in[1].gl_Position.z, 0.f, 0.f, 1.f),
		vec4(gl_in[0].gl_Position.z, 0.f, 0.f, 1.f)
	);

	// Central lines
	gl_Position = transformWithMVP(gl_in[0].gl_Position);
	EmitVertex();
	gl_Position = transformWithMVP(gl_in[1].gl_Position);
	EmitVertex();
	EndPrimitive();

	gl_Position = transformWithMVP(orthogonalLine[0]);
	EmitVertex();
	gl_Position = transformWithMVP(orthogonalLine[1]);
	EmitVertex();
	EndPrimitive();

	const uint scalingFactor = uint(model[0][0]);
	for (uint i = 1; i <= scalingFactor; ++i)
	{
		// Vertical negative
		gl_Position = transformWithMVP(gl_in[0].gl_Position - vec4(float(i) / float(scalingFactor), 0.f, 0.f, 0.f));
		EmitVertex();
		gl_Position = transformWithMVP(gl_in[1].gl_Position - vec4(float(i) / float(scalingFactor), 0.f, 0.f, 0.f));
		EmitVertex();
		EndPrimitive();

		// Vertical positive
		gl_Position = transformWithMVP(gl_in[0].gl_Position + vec4(float(i) / float(scalingFactor), 0.f, 0.f, 0.f));
		EmitVertex();
		gl_Position = transformWithMVP(gl_in[1].gl_Position + vec4(float(i) / float(scalingFactor), 0.f, 0.f, 0.f));
		EmitVertex();
		EndPrimitive();

		// Horizontal lines
		gl_Position = transformWithMVP(orthogonalLine[0] - vec4(0.f, 0.f, float(i) / float(scalingFactor), 0.f));
		EmitVertex();
		gl_Position = transformWithMVP(orthogonalLine[1] - vec4(0.f, 0.f, float(i) / float(scalingFactor), 0.f));
		EmitVertex();
		EndPrimitive();

		gl_Position = transformWithMVP(orthogonalLine[0] + vec4(0.f, 0.f, float(i) / float(scalingFactor), 0.f));
		EmitVertex();
		gl_Position = transformWithMVP(orthogonalLine[1] + vec4(0.f, 0.f, float(i) / float(scalingFactor), 0.f));
		EmitVertex();
		EndPrimitive();
	}
}