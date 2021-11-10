#version 460
#define MAX_BLUR 705

in vec3 direction;

out vec4 color;

layout (binding = 3) uniform SkyboxBlur
{
	uint blurValue;
};

// Texture sampler
layout (binding = 0) uniform samplerCube cubeSampler;

void main()
{
	float offset = 1.0 / (MAX_BLUR - blurValue);
	vec3 offsets[9] = vec3[](
		vec3(-offset, offset, 0.0),
		vec3(0.0, offset, 0.0),
		vec3(offset, offset, 0.0),
		vec3(-offset, 0.0, 0.0),
		vec3(0.0, 0.0, 0.0),
		vec3(offset, 0.0, 0.0),
		vec3(-offset, -offset, 0.0),
		vec3(0.0, -offset, 0.0),
		vec3(offset, -offset, 0.0)
	);

	float blurKernel[9] = float[](
		1, 2, 1,
		2, 4, 2,
		1, 2, 1
	);

	vec3 col = vec3(0.0);
	for (int i = 0; i < 9; ++i)
		col += texture(cubeSampler, direction + offsets[i]).rgb * blurKernel[i] / 16.0;

	color = vec4(col, 1.0);
}
