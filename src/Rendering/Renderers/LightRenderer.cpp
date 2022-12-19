#include "LightRenderer.h"

LightRenderer::Renderable::~Renderable() {}

LightRenderer::LightRenderer()
{
	std::filesystem::path shadersDir(SHADERS_DIR);
	shader.attachShader(Shader::ShaderType::VertexShader, shadersDir / "GeneralVertexShader.glsl");
	shader.attachShader(Shader::ShaderType::FragmentShader, shadersDir / "LightFragmentShader.glsl");
	shader.createProgram();

	mvpBuffer.setData(shader.getConstantBufferInfo("MVP"), true);
	materialsBuffer.setData(shader.getConstantBufferInfo("MeshData"), true);
	lightsBuffer.setData(shader.getConstantBufferInfo("LightsData"), true);
}

void LightRenderer::render(const GraphicsAPI* api, const Camera& camera)
{
	if (camera.viewChanged)
	{
		mvpBuffer.updateData("view", &camera.getView()[0][0], sizeof(glm::mat4));
		mvpBuffer.updateData("eyePosition", &camera.getPosition().x, sizeof(glm::vec4));
	}

	if (camera.projectionChanged)
	{
		mvpBuffer.updateData("projection", &camera.getProjection()[0][0], sizeof(glm::mat4));
	}

	uint32_t lightsCount = (uint32_t)lightsStack.size();
	materialsBuffer.updateData("lightSourceCount", &lightsCount, sizeof(uint32_t));
	uint32_t lightCounter = 0;
	while (!lightsStack.empty())
	{
		const Light& light = *lightsStack.top();
		updateLight(light, lightCounter++);
		lightsStack.pop();
	}

	shader.activate();
	int lightened = 1;
	int state = 1;
	int shadows = 0;
	materialsBuffer.updateData("lightened", &lightened, sizeof(int));
	materialsBuffer.updateData("state", &state, sizeof(int));
	materialsBuffer.updateData("shadows", &shadows, sizeof(int));

	while (!texturedRenderables.empty())
	{
		const TexturedRenderable& renderable = *texturedRenderables.top();
		renderable.diffuseMap.setForSampling(0);
		mvpBuffer.updateData("model", &renderable.transformation.getTransformMatrix()[0][0], sizeof(glm::mat4));
		materialsBuffer.updateData("material.ambientCoefficient", &renderable.ambientCoefficient, sizeof(float));
		materialsBuffer.updateData("material.diffuseCoefficient", &renderable.diffuseCoefficient, sizeof(float));
		materialsBuffer.updateData("material.specularCoefficient", &renderable.specularCoefficient, sizeof(float));
		materialsBuffer.updateData("material.shininess", &renderable.shininess, sizeof(float));
		api->render(renderable.mesh);
		texturedRenderables.pop();
	}

	state = 2;
	materialsBuffer.updateData("state", &state, sizeof(int));
	while (!texturedSpecularRenderables.empty())
	{
		const TexturedSpecularRenderable& renderable = *texturedSpecularRenderables.top();
		renderable.diffuseMap.setForSampling(0);
		renderable.specularMap.setForSampling(1);
		mvpBuffer.updateData("model", &renderable.transformation.getTransformMatrix()[0][0], sizeof(glm::mat4));
		materialsBuffer.updateData("material.ambientCoefficient", &renderable.ambientCoefficient, sizeof(float));
		materialsBuffer.updateData("material.diffuseCoefficient", &renderable.diffuseCoefficient, sizeof(float));
		materialsBuffer.updateData("material.specularCoefficient", &renderable.specularCoefficient, sizeof(float));
		materialsBuffer.updateData("material.shininess", &renderable.shininess, sizeof(float));
		api->render(renderable.mesh);
		texturedSpecularRenderables.pop();
	}

	/*int state = 3;
	materialsBuffer.updateData("state", &state, sizeof(int));
	while (!texturedNormalRenderables.empty())
	{
		const TexturedSpecularNormalRenderable& renderable = *texturedNormalRenderables.top();
		renderable.diffuseMap.setForSampling(0);
		renderable.specularMap.setForSampling(1);
		renderable.normalMap.setForSampling(2);
		mvpBuffer.updateData("model", &renderable.transformation.getTransformMatrix()[0][0], sizeof(glm::mat4));
		materialsBuffer.updateData("material.ambientCoefficient", &renderable.ambientCoefficient, sizeof(float));
		materialsBuffer.updateData("material.diffuseCoefficient", &renderable.diffuseCoefficient, sizeof(float));
		materialsBuffer.updateData("material.specularCoefficient", &renderable.specularCoefficient, sizeof(float));
		materialsBuffer.updateData("material.shininess", &renderable.shininess, sizeof(float));
		api->render(renderable.mesh);
		texturedRenderables.pop();
	}*/

	state = 0;
	materialsBuffer.updateData("state", &state, sizeof(int));
	while (!coloredRenderables.empty())
	{
		const ColoredRenderable& renderable = *coloredRenderables.top();
		mvpBuffer.updateData("model", &renderable.transformation.getTransformMatrix()[0][0], sizeof(glm::mat4));
		materialsBuffer.updateData("material.color", &renderable.color.x, sizeof(glm::vec4));
		materialsBuffer.updateData("material.ambientCoefficient", &renderable.ambientCoefficient, sizeof(float));
		materialsBuffer.updateData("material.diffuseCoefficient", &renderable.diffuseCoefficient, sizeof(float));
		materialsBuffer.updateData("material.specularCoefficient", &renderable.specularCoefficient, sizeof(float));
		materialsBuffer.updateData("material.shininess", &renderable.shininess, sizeof(float));
		api->render(renderable.mesh);
		coloredRenderables.pop();
	}
}

void LightRenderer::pushTexturedRenderable(const TexturedRenderable& renderable)
{
	texturedRenderables.push(&renderable);
}

void LightRenderer::pushTexturedSpecularRenderable(const TexturedSpecularRenderable& renderable)
{
	texturedSpecularRenderables.push(&renderable);
}

void LightRenderer::pushColoredRenderable(const ColoredRenderable& renderable)
{
	coloredRenderables.push(&renderable);
}

void LightRenderer::pushLight(const Light& light)
{
	lightsStack.push(&light);
}

void LightRenderer::updateLight(const Light& light, uint32_t lightIndex) const
{
	glm::vec3 color, direction;
	float constant, linear, quadratic, cutoff;

	switch (light.type)
	{
		case Light::Type::Point:
		{
			color = light.intensity * light.color;
			direction = glm::vec3(0.f, 0.f, 0.f);
			constant = 1.1f;
			linear = 0.024f;
			quadratic = 0.0021f;
			cutoff = -1.f;
			break;
		}
		case Light::Type::Directional:
		{
			color = glm::vec4(light.intensity * light.color, 1.f);
			direction = glm::vec3(0.f, -1.f, 0.f);
			constant = 0.f;
			linear = 0.f;
			quadratic = 0.f;
			cutoff = 1.1f;
			break;
		}
		case Light::Type::Spotlight:
		{
			color = glm::vec4(light.intensity * light.color, 1.f);
			direction = glm::vec4(glm::vec3(light.transformation.getRotationMatrix() * glm::vec4(0.f, -1.f, 0.f, 0.f)), 0.f);
			constant = 1.f;
			linear = 0.045f;
			quadratic = 0.0075f;
			cutoff = glm::cos(light.spotlightCutoff);
			break;
		}
	}

	std::string lightsString = "lights[" + std::to_string(lightIndex) + "].";
	lightsBuffer.updateData(lightsString + "position", &light.transformation.getTranslation().x, sizeof(glm::vec4));
	lightsBuffer.updateData(lightsString + "color", &color, sizeof(glm::vec4));
	lightsBuffer.updateData(lightsString + "direction", &direction, sizeof(glm::vec4));
	lightsBuffer.updateData(lightsString + "constant", &constant, sizeof(float));
	lightsBuffer.updateData(lightsString + "linear", &linear, sizeof(float));
	lightsBuffer.updateData(lightsString + "quadratic", &quadratic, sizeof(float));
	lightsBuffer.updateData(lightsString + "cutoff", &cutoff, sizeof(float));
}
