#version 440 core

#define DIFFUSE 0 
#define SPECULAR 1
#define NORMAL 2
#define SKYBOX 3

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
};

in Attributes {
	vec3 fragPosition;
	mat3 TBN;
	vec2 texCoords;
} inAttributes;

layout (binding = 4) uniform LightsData 
{
	Light lights[16];
};

layout (binding = 2) uniform MeshData
{
	Material material;
	uint lightSourceCount;
	bool shadows;
	float indexRatio;
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

layout (binding = 0) uniform sampler2D diffuseMap;
layout (binding = 1) uniform sampler2D specularMap;
layout (binding = 2) uniform sampler2D normalMap;
layout (binding = 3) uniform samplerCube cubeMap; 

out vec4 outColor;
vec3 normal;

vec3 reflectionVector() 
{
	vec3 vector = reflect(normalize(inAttributes.fragPosition - eyePosition), normal);
	return vector;
}

vec3 refractionVector() 
{
	vec3 vector = refract(normalize(inAttributes.fragPosition - eyePosition), normal, indexRatio);
	return vector;
}

float attenuation(in Light light) 
{
	float distance = length(light.position - inAttributes.fragPosition);
	float attenuation = 1.0 / (light.constant + light.linear * distance * light.quadratic * distance * distance);
	return attenuation;
}

vec3 calculatePointLight(in vec3 colorAmbient, in vec3 colorDiffuse, in vec3 colorSpecular, in vec3 eyeDirection, in Light light) 
{
	// ambient
	vec3 ambientComponent = colorAmbient; 

	// diffuse
	vec3 lightDirection = normalize(light.position - inAttributes.fragPosition);
	float lightAngle = dot(lightDirection, normal);
	lightAngle = max(lightAngle, 0.0);
	vec3 diffuseComponent = colorDiffuse.rgb * light.color * lightAngle;

	// specular
	// vec3 reflectedLight = reflect(-lightDirection, normal);
	vec3 halfwayVector = normalize(lightDirection + eyeDirection);
	float eyeAngle = pow(max(dot(halfwayVector, normal), 0.0), material.shininess);
	vec3 specularComponent = colorSpecular * light.color * eyeAngle;
		
	// float shadow = shadowCalculation(1.0 - dot(normal, lightDirection));
	vec3 result = ambientComponent + ((diffuseComponent + specularComponent) * attenuation(light));

	return result;
}


vec3 calculateDirectionalLight(in vec3 colorAmbient, in vec3 colorDiffuse, in vec3 colorSpecular, in vec3 eyeDirection, in Light light)
{
	// ambient
	vec3 ambientComponent = colorAmbient;

	// diffuse
	vec3 lightDirection = normalize(-light.direction);
	float lightAngle = dot(lightDirection, normal);
	lightAngle = max(lightAngle, 0.0);
	vec3 diffuseComponent = colorDiffuse.rgb * light.color * lightAngle;

	// specular
	// vec3 reflectedLight = reflect(-lightDirection, normal);
	vec3 halfwayVector = normalize(lightDirection + eyeDirection);
	float eyeAngle = pow(max(dot(halfwayVector, normal), 0.0), 3 * material.shininess);
	vec3 specularComponent = colorSpecular * light.color * eyeAngle;
		
	vec3 result = ambientComponent + diffuseComponent + specularComponent;

	return result;
}


vec3 calculateSpotlightLight(in vec3 colorAmbient, in vec3 colorDiffuse, in vec3 colorSpecular, in vec3 eyeDirection, in Light light)
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
	vec3 diffuseComponent = colorDiffuse.rgb * light.color * lightAngle;

	// specular
	// vec3 reflectedLight = reflect(-lightDirection,inAttributes. normal);
	vec3 halfwayVector = normalize(lightDirection + eyeDirection);
	float eyeAngle = pow(max(dot(halfwayVector, normal), 0.0), 3 * material.shininess);
	vec3 specularComponent = colorSpecular * light.color * eyeAngle;

	float intensity = clamp((theta - outerCutOff) / epsilon, 0.0, 1.0);
	vec3 result = ambientComponent + ((diffuseComponent + specularComponent) * intensity * attenuation(light));

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

		
	if ((state & BIT(SKYBOX)) != 0)
	{
		vec3 mappingVector = vec3(0.0);
		if (indexRatio == 0.0f)
			mappingVector = reflectionVector();
		else 
			mappingVector = refractionVector();

		vec4 skyboxSample = texture(cubeMap, mappingVector);
		colorAmbient = skyboxSample.rgb * (material.ambientCoefficient + material.color.rgb);
		colorDiffuse = skyboxSample.rgb * material.diffuseCoefficient * material.color.rgb;
		colorSpecular = skyboxSample.rgb * material.specularCoefficient * material.color.rgb;
		alpha = material.color.a;
	}

	vec4 color = vec4(0.f, 0.f, 0.f, 1.f);
	for (int i = 0; i < lightSourceCount; ++i)
	{
		vec3 eyeDirection = normalize(eyePosition - inAttributes.fragPosition);

		if (lights[i].constant == 0.f) 
			color.rgb += calculateDirectionalLight(colorAmbient, colorDiffuse, colorSpecular, eyeDirection, lights[i]);
		else if (lights[i].cutoff == -1.f)
			color.rgb += calculatePointLight(colorAmbient, colorDiffuse, colorSpecular, eyeDirection, lights[i]);
		else 
			color.rgb += calculateSpotlightLight(colorAmbient, colorDiffuse, colorSpecular, eyeDirection, lights[i]);

	}

	outColor = color;
}