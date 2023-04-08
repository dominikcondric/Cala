#version 440 core

#define DIFFUSE 0 
#define SPECULAR 1
#define NORMAL 2

#define MAX_LIGHTS_COUNT 8

#define BIT(x) (1 << x)

struct Material {
	vec4 color;
	float ambientCoefficient;
	float diffuseCoefficient;
	float specularCoefficient;
	float shininess;
};

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

in Attributes {
	vec3 fragPosition;
	mat3 TBN;
	vec2 texCoords;
} inAttributes;

layout (binding = 4) uniform LightsData 
{
	Light lights[MAX_LIGHTS_COUNT];
	uint lightSourceCount;
	bool shadows;
};

layout (binding = 2) uniform MeshData
{
	Material material;
	uint state;
	bool lightened;
};

layout (binding = 0) uniform MVP 
{
	mat4 model;
	mat4 view;
	mat4 projection;
	vec3 eyePosition;
};

layout (binding = DIFFUSE) uniform sampler2D diffuseMap;
layout (binding = SPECULAR) uniform sampler2D specularMap;
layout (binding = NORMAL) uniform sampler2D normalMap;
layout (binding = 3) uniform sampler2DArrayShadow shadowMaps;

out vec4 outColor;
vec3 normal;
vec3 lightViewPosition;

float attenuation(const Light light) 
{
	float distance = length(light.position - inAttributes.fragPosition);
	float attenuation = 1.0 / (light.constant + light.linear * distance * light.quadratic * distance * distance);
	return attenuation;
}

bool isPoint(const Light light)
{
	return light.cutoff < 1.5f;
}

bool isDirectional(const Light light)
{
	return light.constant < 0.9f;
}

bool isSpotlight(const Light light)
{
	return !isPoint(light) && !isDirectional(light);
}

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

float shadowFactor(const Light light, out uint shadowMapCounter)
{
	if (!shadows || light.projection == mat4(0.f))
		return 0.f;

	mat4 view;
	uint shadowMapIndex = shadowMapCounter;
	if (!isPoint(light))
	{
		view = calculateLightViewMatrix(light, light.direction);
		shadowMapCounter++;
	}
	else
	{
		vec3 direction = normalize(inAttributes.fragPosition - light.position);
		vec3 lightDirection = vec3(0.f);
		if (abs(direction.x) > abs(direction.y) && abs(direction.x) > abs(direction.z))
			if (direction.x > 0)
			{
				shadowMapIndex += 0;
				lightDirection.x = 1.f;
			}
			else
			{
				shadowMapIndex += 1;
				lightDirection.x = -1.f;
			} 
		else if (abs(direction.y) > abs(direction.z))
			if (direction.y > 0)
			{
				shadowMapIndex += 2;
				lightDirection.y = 1.f;
			}
			else
			{
				shadowMapIndex += 3;
				lightDirection.y = -1.f;
			} 
		else
			if (direction.z > 0)
			{
				shadowMapIndex += 4;
				lightDirection.z = 1.f;
			}
			else
			{
				shadowMapIndex += 5;
				lightDirection.z = -1.f;
			} 

		view = calculateLightViewMatrix(light, lightDirection);
		shadowMapCounter += 6;
	}

	vec4 lightViewPositionClip = light.projection * view * vec4(inAttributes.fragPosition, 1.f);
	lightViewPosition = lightViewPositionClip.xyz / lightViewPositionClip.w;
	lightViewPosition = lightViewPosition * 0.5f + 0.5f;

	float shadow = 0.f;
	vec3 lightDirection = normalize(light.position - inAttributes.fragPosition);
	float bias = max(0.05f * (1.0f - dot(normal, lightDirection)), 0.005f);
    float currentDepth = lightViewPosition.z;
	if (currentDepth >= 1.f)
        return 0.f;

    shadow = texture(shadowMaps, vec4(lightViewPosition.xy, shadowMapIndex, currentDepth - bias));

	// PCF
	/* vec2 texelSize = 1.f / textureSize(shadowMaps, 0).xy;
	for (int x = -1; x <= 1; ++x)
	{
		for(int y = -1; y <= 1; ++y)
		{
			float pcfDepth = texture(shadowMaps, vec3(lightViewPosition.xy + vec2(x, y) * texelSize, shadowMapIndex)).r; 
			shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;        
		}    
	} 

	shadow /= 9.f; */

    return shadow;
}  

vec3 calculatePointLight(inout vec3 colorAmbient, inout vec3 colorDiffuse, inout vec3 colorSpecular, inout vec3 eyeDirection, const Light light, out uint shadowMapCounter) 
{
	// ambient
	vec3 ambientComponent = colorAmbient; 

	// diffuse
	vec3 lightDirection = normalize(light.position - inAttributes.fragPosition);
	float lightAngle = dot(lightDirection, normal);
	lightAngle = max(lightAngle, 0.0);
	vec3 diffuseComponent = colorDiffuse * light.color * lightAngle;

	// specular
	// vec3 reflectedLight = reflect(-lightDirection, normal);
	vec3 halfwayVector = normalize(lightDirection + eyeDirection);
	float eyeAngle = pow(max(dot(halfwayVector, normal), 0.0), material.shininess);
	vec3 specularComponent = colorSpecular * light.color * eyeAngle;
		
	vec3 result = ambientComponent + ((diffuseComponent + specularComponent) * attenuation(light) * (1.f - shadowFactor(light, shadowMapCounter)));
	return result;
}


vec3 calculateDirectionalLight(inout vec3 colorAmbient, inout vec3 colorDiffuse, inout vec3 colorSpecular, inout vec3 eyeDirection, const Light light, out uint shadowMapCounter)
{
	// ambient
	vec3 ambientComponent = colorAmbient;

	// diffuse
	vec3 lightDirection = normalize(-light.direction);
	float lightAngle = dot(lightDirection, normal);
	lightAngle = max(lightAngle, 0.0);
	vec3 diffuseComponent = colorDiffuse * light.color * lightAngle;

	// specular
	// vec3 reflectedLight = reflect(-lightDirection, normal);
	vec3 halfwayVector = normalize(lightDirection + eyeDirection);
	float eyeAngle = pow(max(dot(halfwayVector, normal), 0.0), 3 * material.shininess);
	vec3 specularComponent = colorSpecular * light.color * eyeAngle;
		
	vec3 result = ambientComponent + (diffuseComponent + specularComponent) * (1.f - shadowFactor(light, shadowMapCounter));

	return result;
}


vec3 calculateSpotlightLight(inout vec3 colorAmbient, inout vec3 colorDiffuse, inout vec3 colorSpecular, inout vec3 eyeDirection, const Light light, out uint shadowMapCounter)
{
	// ambient
	vec3 ambientComponent = colorAmbient; 

	// diffuse
	vec3 lightDirection = normalize(light.position - inAttributes.fragPosition);
	float theta = dot(normalize(-light.direction), lightDirection);
	float epsilon = 0.12;
	float outerCutOff = light.cutoff - epsilon;
	if (theta < outerCutOff) 
		return ambientComponent;

	float lightAngle = dot(lightDirection, normal);
	lightAngle = max(lightAngle, 0.0);
	vec3 diffuseComponent = colorDiffuse * light.color * lightAngle;

	// specular
	// vec3 reflectedLight = reflect(-lightDirection,inAttributes. normal);
	vec3 halfwayVector = normalize(lightDirection + eyeDirection);
	float eyeAngle = pow(max(dot(halfwayVector, normal), 0.0), 3 * material.shininess);
	vec3 specularComponent = colorSpecular * light.color * eyeAngle;

	float intensity = clamp((theta - outerCutOff) / epsilon, 0.0, 1.0);
	vec3 result = ambientComponent + ((diffuseComponent + specularComponent) * intensity * attenuation(light) * (1.f - shadowFactor(light, shadowMapCounter)));

	return result;
}


void main() 
{
	if (!lightened)
	{
		outColor = material.color;	
		return;
	}

	vec3 colorDiffuse;
	vec3 colorSpecular;
	vec3 colorAmbient;
	float alpha;

	if ((state & BIT(DIFFUSE)) != 0)
	{
		vec4 diffuseMapSample = texture(diffuseMap, inAttributes.texCoords);
		colorAmbient = diffuseMapSample.rgb * material.ambientCoefficient;
		colorDiffuse = diffuseMapSample.rgb * material.diffuseCoefficient;
		colorSpecular = diffuseMapSample.rgb * material.specularCoefficient;
		alpha = diffuseMapSample.a > 0.f ? diffuseMapSample.a : material.color.a;
	} 
	else 
	{
		colorAmbient = material.color.rgb * material.ambientCoefficient;
		colorDiffuse = material.color.rgb * material.diffuseCoefficient;
		colorSpecular = material.color.rgb * material.specularCoefficient;
		alpha = material.color.a;
	}

	if ((state & BIT(SPECULAR)) != 0) 
	{
		colorSpecular = texture(specularMap, inAttributes.texCoords).rgb;
	}

	if ((state & BIT(NORMAL)) != 0)
	{
		normal = texture(normalMap, inAttributes.texCoords).xyz;
		normal = normalize(inAttributes.TBN * (normal * 2.0 - 1.0));
	}
	else
	{
		normal = inAttributes.TBN[2];
	}

	uint shadowMapCounter = 0;
	vec4 color = vec4(0.f, 0.f, 0.f, alpha);
	for (uint lightCounter = 0; lightCounter < lightSourceCount; ++lightCounter)
	{
		vec3 eyeDirection = normalize(eyePosition - inAttributes.fragPosition);
		const Light light = lights[lightCounter];
		if (isDirectional(light)) 
		{
			color.rgb += calculateDirectionalLight(colorAmbient, colorDiffuse, colorSpecular, eyeDirection, light, shadowMapCounter);
		}
		else if (isPoint(light))
		{
			color.rgb += calculatePointLight(colorAmbient, colorDiffuse, colorSpecular, eyeDirection, light, shadowMapCounter);
		}
		else
		{
			color.rgb += calculateSpotlightLight(colorAmbient, colorDiffuse, colorSpecular, eyeDirection, light, shadowMapCounter);
		}
	}

	outColor = color;
}