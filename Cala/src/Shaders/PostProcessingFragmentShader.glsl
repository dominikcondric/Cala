#version 460

#define BLOOM 0
#define HDR 1
#define BOX_BLUR 2
#define EDGE_DETECTION 3
#define GAUSSIAN_BLUR 4
#define NEGATIVE 5
#define HORIZONTAL_PASS_GAUSSIAN_BLUR 6
#define VERTICAL_PASS_GAUSSIAN_BLUR 7
#define EXTRACT 8
#define COPY 9

in vec2 texCoords;

layout (binding = 6) uniform EffectValues {
	uint effect;
	float effectStrength;
};

layout (binding = 0) uniform sampler2D scene;
layout (binding = 1) uniform sampler2D bloomedScene;

vec3 gaussianBlurKernel()
{
	float offset = 1.f / clamp(900.f - effectStrength, 50.f, 900.f);
	vec2 offsets[9] = vec2[](
		vec2(-offset, offset),
		vec2(0.0, offset),
		vec2(offset, offset),
		vec2(-offset, 0.0),
		vec2(0.0, 0.0),
		vec2(offset, 0.0),
		vec2(-offset, -offset),
		vec2(0.0, -offset),
		vec2(offset, -offset)
	);

	float kernel[9] = float[](
		1, 2, 1,
		2, 4, 2,
		1, 2, 1
	);

	vec3 col = vec3(0.f);
	for (int i = 0; i < 9; ++i)
	{
		vec3 texSample = texture(scene, texCoords + offsets[i]).rgb;
		col += texSample * kernel[i] / 16.f;
	}

	return col;
}

vec3 edgeDetection()
{
	float offset = 1.f / effectStrength;
	vec2 offsets[9] = vec2[](
		vec2(-offset, offset),
		vec2(0.0, offset),
		vec2(offset, offset),
		vec2(-offset, 0.0),
		vec2(0.0, 0.0),
		vec2(offset, 0.0),
		vec2(-offset, -offset),
		vec2(0.0, -offset),
		vec2(offset, -offset)
	);

	float strength = effectStrength / 50.0;
	float kernel[9] = float[](
		-1,		-1,    -1,
		-1,  strength, -1,
		-1,		-1,	   -1
	);

	vec3 col = vec3(0.0);
	for (int i = 0; i < 9; ++i)
		col += texture(scene, texCoords + offsets[i]).rgb * kernel[i];

	return col;
}

vec3 boxBlur() 
{
	float offset = 1.f / clamp(900.f - effectStrength, 50.f, 900.f);
	vec2 offsets[9] = vec2[](
		vec2(-offset, offset),
		vec2(0.0, offset),
		vec2(offset, offset),
		vec2(-offset, 0.0),
		vec2(0.0, 0.0),
		vec2(offset, 0.0),
		vec2(-offset, -offset),
		vec2(0.0, -offset),
		vec2(offset, -offset)
	);

	vec3 col = vec3(0.0);
	for (int i = 0; i < 9; ++i)
		col += texture(scene, texCoords + offsets[i]).rgb / 9.0;

	return col;
}

vec3 negative()
{
	vec3 texSample = texture(scene, texCoords).rgb;
	return vec3(1.f - texSample);
}

vec3 twoPassGaussianBlur(in bool horizontal)
{
	float weights[10] = float[](
		0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216,
		0.0124057, 0.01005246, 0.0092616, 0.007454, 0.0042616
	);

	vec2 texelSize = 1.f / textureSize(scene, 0);
	vec3 result = texture(scene, texCoords).rgb * weights[0];
	if (horizontal)
	{
		for (int i = 1; i < 10; ++i)
		{
			result += texture(scene, texCoords + vec2(texelSize.x * i, 0.0)).rgb * weights[i];
			result += texture(scene, texCoords - vec2(texelSize.x * i, 0.0)).rgb * weights[i];
		}
	}
	else 
	{
		for (int i = 1; i < 10; ++i) 
		{
			result += texture(scene, texCoords + vec2(0.0, texelSize.y * i)).rgb * weights[i];
			result += texture(scene, texCoords - vec2(0.0, texelSize.y * i)).rgb * weights[i];
		}
	}
	
	return result;
}

vec4 extract()
{
	const vec3 texSample = texture(scene, texCoords).rgb;
	const vec3 intensityWeights = vec3(0.2126f, 0.7152f, 0.0722f);

	if (dot(intensityWeights, texSample) >= 1.f) 
	{
		return vec4(texSample, 1.f);
	}
	else
	{
		/* vec2 pixelSize = 1.f / textureSize(scene, 0);
		for (int i = 0; i < 10; ++i)
		{
			for (int j = 0; j < 10; ++j)
			{
				const vec3 firstSample = texture(scene, vec2(texCoords.x + pixelSize.x * i, texCoords.y + pixelSize.y * j)).rgb;
				const vec3 secondSample = texture(scene, vec2(texCoords.x - pixelSize.x * i, texCoords.y - pixelSize.y * j)).rgb;
				if (dot(intensityWeights, firstSample) >= 1.f || dot(intensityWeights, secondSample) >= 1.f)
					return vec4(0.f, 0.f, 0.f, 1.f);
			}
		}		 */

		return vec4(0.f);
	}
}

void main()
{
	switch (effect)
	{ 
		case NEGATIVE:
			gl_FragColor = vec4(negative(), 1.f);
			break;
		case BOX_BLUR:
			gl_FragColor = vec4(boxBlur(), 1.f);
			break;
		case EDGE_DETECTION:
			gl_FragColor = vec4(edgeDetection(), 1.f);
			break;
		case GAUSSIAN_BLUR:
			gl_FragColor = vec4(gaussianBlurKernel(), 1.f);
			break;
		case HDR:
			gl_FragColor = vec4(vec3(1.f) - exp(-texture(scene, texCoords).rgb * effectStrength), 1.f);
			break;
		case HORIZONTAL_PASS_GAUSSIAN_BLUR:
			gl_FragColor = vec4(twoPassGaussianBlur(true), 1.f);
			break;
		case VERTICAL_PASS_GAUSSIAN_BLUR:
			gl_FragColor = vec4(twoPassGaussianBlur(false), 1.f);
			break;
		case EXTRACT:
			gl_FragColor = extract();
			break;
		case BLOOM:
			gl_FragColor = vec4(texture(scene, texCoords).rgb + texture(bloomedScene, texCoords).rgb, 1.f);
			break;
		case COPY:
			gl_FragColor = vec4(texture(scene, texCoords).rgb, 1.f);
			break;
	}
}