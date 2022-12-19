#version 440 core

#define COLORED 0
#define TEXTURED_WITHOUT_SPECULAR_MAP 1 
#define TEXTURED_WITH_SPECULAR_MAP 2
#define SKYBOX_REFLECTION 3

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
	vec3 normal;
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
layout (binding = 2) uniform samplerCube cubeMap; 

out vec4 outColor;

vec3 reflectionVector() 
{
	vec3 vector = reflect(normalize(inAttributes.fragPosition - eyePosition), inAttributes.normal);
	return vector;
}

vec3 refractionVector() 
{
	vec3 vector = refract(normalize(inAttributes.fragPosition - eyePosition), inAttributes.normal, indexRatio);
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
	float lightAngle = dot(lightDirection, inAttributes.normal);
	lightAngle = max(lightAngle, 0.0);
	vec3 diffuseComponent = colorDiffuse.rgb * light.color * lightAngle;

	// specular
	// vec3 reflectedLight = reflect(-lightDirection, inAttributes.normal);
	vec3 halfwayVector = normalize(lightDirection + eyeDirection);
	float eyeAngle = pow(max(dot(halfwayVector, inAttributes.normal), 0.0), material.shininess);
	vec3 specularComponent = colorSpecular * light.color * eyeAngle;
		
	// float shadow = shadowCalculation(1.0 - dot(inAttributes.normal, lightDirection));
	vec3 result = ambientComponent + ((diffuseComponent + specularComponent) * attenuation(light));

	return result;
}


vec3 calculateDirectionalLight(in vec3 colorAmbient, in vec3 colorDiffuse, in vec3 colorSpecular, in vec3 eyeDirection, in Light light)
{
	// ambient
	vec3 ambientComponent = colorAmbient;

	// diffuse
	vec3 lightDirection = normalize(-light.direction);
	float lightAngle = dot(lightDirection, inAttributes.normal);
	lightAngle = max(lightAngle, 0.0);
	vec3 diffuseComponent = colorDiffuse.rgb * light.color * lightAngle;

	// specular
	// vec3 reflectedLight = reflect(-lightDirection, inAttributes.normal);
	vec3 halfwayVector = normalize(lightDirection + eyeDirection);
	float eyeAngle = pow(max(dot(halfwayVector, inAttributes.normal), 0.0), 3 * material.shininess);
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

	float lightAngle = dot(lightDirection, inAttributes.normal);
	lightAngle = max(lightAngle, 0.0);
	vec3 diffuseComponent = colorDiffuse.rgb * light.color * lightAngle;

	// specular
	// vec3 reflectedLight = reflect(-lightDirection,inAttributes. inAttributes.normal);
	vec3 halfwayVector = normalize(lightDirection + eyeDirection);
	float eyeAngle = pow(max(dot(halfwayVector, inAttributes.normal), 0.0), 3 * material.shininess);
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

	switch (state)
	{
		case COLORED:
		{
			outColor = vec4(1.f, 0.f, 0.f, 1.f);
			colorAmbient = material.color.rgb * material.ambientCoefficient;
			colorDiffuse = material.color.rgb * material.diffuseCoefficient;
			colorSpecular = material.color.rgb * material.specularCoefficient;
			alpha = material.color.a;
			break;	
		}

		case TEXTURED_WITHOUT_SPECULAR_MAP:
		{
			vec4 diffuseMapSample = texture(diffuseMap, inAttributes.texCoords);
			colorAmbient = diffuseMapSample.rgb * material.ambientCoefficient;
			colorDiffuse = diffuseMapSample.rgb * material.diffuseCoefficient;
			colorSpecular = diffuseMapSample.rgb * material.specularCoefficient;
			alpha = diffuseMapSample.a > 0.f ? diffuseMapSample.a : material.color.a;
			break;
		}

		case TEXTURED_WITH_SPECULAR_MAP:
		{
			vec4 diffuseMapSample = texture(diffuseMap, inAttributes.texCoords);
			colorAmbient = diffuseMapSample.rgb * material.ambientCoefficient;
			colorDiffuse = diffuseMapSample.rgb * material.diffuseCoefficient;
			colorSpecular = texture(specularMap, inAttributes.texCoords).rgb;
			alpha = diffuseMapSample.a > 0.f ? diffuseMapSample.a : material.color.a;
			break;
		}

		case SKYBOX_REFLECTION:
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
			break;
		}
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