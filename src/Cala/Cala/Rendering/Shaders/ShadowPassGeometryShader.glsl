#version 440 core
layout (triangles) in;

#define MAX_LIGHTS_COUNT 8

layout (triangle_strip, max_vertices = 3 * MAX_LIGHTS_COUNT * 6) out;

struct Light {
	vec3 position;
	vec3 color;
	vec3 direction;
	float constant;
	float linear;
	float quadratic;
	float cutoff;
	mat4 projection;
};

layout (binding = 4) uniform LightsData 
{
	Light lights[MAX_LIGHTS_COUNT];
    uint lightSourceCount;
	bool shadows;
};

mat4 calculateLightViewMatrix(const Light light, const vec3 direction)
{
	vec3 up = vec3(0.f, 1.f, 0.f);
	if (abs(direction.x) < 1e-7 && abs(direction.z) < 1e-7) 
		up.x = 0.001f;
		
	vec3 right = normalize(cross(up, direction)); // Get first perpendicular vector from random up and direction
	up = cross(direction, right); // Get third perpendicular vector from direction, and right

	mat4 view = transpose(mat4(
		vec4(right, 0.f), // Right
		vec4(up, 0.f), // Up
		vec4(-direction, 0.f), // Direction (inverse)
		vec4(0.f, 0.f, 0.f, 1.f)
	));

	mat4 translation = mat4(1.f);
	translation[3] = vec4(-light.position, 1.f);
	view = view * translation;

	return view;
}

void generatePointShadowMaps(const Light light)
{
	vec3 directions[6] = vec3[](
		vec3(1.f, 0.f, 0.f),  // +X
		vec3(-1.f, 0.f, 0.f), // -X
		vec3(0.f, 1.f, 0.f),  // +Y
		vec3(0.f, -1.f, 0.f), // -Y
		vec3(0.f, 0.f, 1.f),  // +Z
		vec3(0.f, 0.f, -1.f)  // -Z
	);

	for (uint j = 0; j < 6; ++j)
	{
		mat4 view = calculateLightViewMatrix(light, directions[j]);
		for (uint vertex = 0; vertex < 3; ++vertex)
		{
			gl_Position = light.projection * view * gl_in[vertex].gl_Position;
			EmitVertex();
		}
		EndPrimitive();
		gl_Layer++;
	}
}

void generateDirectionalSpotlightShadowMaps(const Light light)
{
	mat4 view = calculateLightViewMatrix(light, light.direction);
	for (int vertex = 0; vertex < 3; ++vertex)
	{
		gl_Position = light.projection * view * gl_in[vertex].gl_Position;
		EmitVertex();
	}
	EndPrimitive();
	gl_Layer++;
}

void main()
{
	gl_Layer = 0;
	for (uint i = 0; i < lightSourceCount; ++i)
	{
		if (lights[i].projection == mat4(0.f))
			continue;

		if (lights[i].constant > 1.f)
			generatePointShadowMaps(lights[i]);
		else
			generateDirectionalSpotlightShadowMaps(lights[i]);
	}
}
