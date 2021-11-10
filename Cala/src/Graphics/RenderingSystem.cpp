#include "RenderingSystem.h"
#include <glm/mat4x4.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/string_cast.hpp>
#include <iostream>
#include "Camera.h"
#include "OpenGL/OGLGraphicsAPI.h"
#include "OpenGL/OGLConstantBuffer.h"

RenderingSystem::~RenderingSystem()
{
}

void RenderingSystem::initialize(GraphicsAPI* api)
{
	if (api == nullptr)
		return;

	renderer = api;

	std::filesystem::path shadersPath = Cala::getProjectPath() / "Cala/src/Shaders/";

	generalShader = Unique<Shader>(renderer->createShader());
	generalShader->attachShader(Shader::ShaderType::VertexShader, shadersPath / "GeneralVertexShader.glsl");
	generalShader->attachShader(Shader::ShaderType::FragmentShader, shadersPath / "GeneralFragmentShader.glsl");
	generalShader->createProgram();

	cameraTransforms.reset(renderer->createConstantBuffer());
	cameraTransforms->setData(generalShader->getConstantBufferInfo("MVP"), true);
	
	materialsBuffer.reset(renderer->createConstantBuffer());
	materialsBuffer->setData(generalShader->getConstantBufferInfo("ModelData"), true);
	uint32_t shadows = 0;
	materialsBuffer->updateData("shadows", &shadows, sizeof(uint32_t));

	lightsBuffer.reset(renderer->createConstantBuffer());
	lightsBuffer->setData(generalShader->getConstantBufferInfo("LightData"), true);

	skyboxShader = Unique<Shader>(renderer->createShader());
	skyboxShader->attachShader(Shader::ShaderType::VertexShader, shadersPath / "SkyboxVertexShader.glsl");
	skyboxShader->attachShader(Shader::ShaderType::FragmentShader, shadersPath / "SkyboxFragmentShader.glsl");
	skyboxShader->createProgram();

	skyboxBlurBuffer.reset(renderer->createConstantBuffer());
	skyboxBlurBuffer->setData(skyboxShader->getConstantBufferInfo("SkyboxBlur"), false);

	helperGrid.shader = Unique<Shader>(renderer->createShader());
	helperGrid.shader->attachShader(Shader::ShaderType::VertexShader, shadersPath / "BaseMeshVertexShader.glsl");
	helperGrid.shader->attachShader(Shader::ShaderType::GeometryShader, shadersPath / "BaseMeshGeometryShader.glsl");
	helperGrid.shader->attachShader(Shader::ShaderType::FragmentShader, shadersPath / "BaseMeshFragmentShader.glsl");
	helperGrid.shader->createProgram();

	postProcessingUtility.shader = Unique<Shader>(renderer->createShader());
	postProcessingUtility.shader->attachShader(Shader::ShaderType::VertexShader, shadersPath / "PostProcessingVertexShader.glsl");
	postProcessingUtility.shader->attachShader(Shader::ShaderType::FragmentShader, shadersPath / "PostProcessingFragmentShader.glsl");
	postProcessingUtility.shader->createProgram();

	postProcessingUtility.effectsBuffer.reset(renderer->createConstantBuffer());
	postProcessingUtility.effectsBuffer->setData(postProcessingUtility.shader->getConstantBufferInfo("EffectValues"), true);
	
	postProcessingUtility.helperFramebuffers[0].reset(renderer->createFramebuffer());
	postProcessingUtility.helperFramebuffers[0]->generateRenderingTarget(1024, 768, 1);
	postProcessingUtility.helperFramebuffers[1].reset(renderer->createFramebuffer());
	postProcessingUtility.helperFramebuffers[1]->generateColorBuffer(1024, 768, 1);
	postProcessingUtility.helperFramebuffers[2].reset(renderer->createFramebuffer());
	postProcessingUtility.helperFramebuffers[2]->generateColorBuffer(1024, 768, 1);

	postProcessingUtility.effectsValues[PostProcessingEffect::BoxBlur] = 400.f;
	postProcessingUtility.effectsValues[PostProcessingEffect::GaussianBlur] = 400.f;
	postProcessingUtility.effectsValues[PostProcessingEffect::EdgeDetection] = 400.f;
	postProcessingUtility.effectsValues[PostProcessingEffect::HDR] = 2.f;
	postProcessingUtility.effectsValues[PostProcessingEffect::Bloom] = 2.f;

	// Skybox cube
	skyboxMesh.reset(renderer->createMesh());
	skyboxMesh->loadCube();

	// renderingQuad
	float renderingQuadVertices[] = {
		-1.f, 1.f, 0.f, 1.f,
		-1.f, -1.f, 0.f, 0.f,
		1.f, 1.f, 1.f, 1.f,
		1.f, -1.f, 1.f, 0.f
	};
	
	postProcessingUtility.renderingQuad.reset(renderer->createMesh());
	Mesh* renderingQuad = postProcessingUtility.renderingQuad.get();
	renderingQuad->setDrawingMode(Mesh::DrawingMode::TriangleStrip);
	std::vector<Mesh::VertexLayoutSpecification> modelLayouts;
	modelLayouts.push_back({ 0, 2, 4 * sizeof(float), 0, 1 });
	modelLayouts.push_back({ 1, 2, 4 * sizeof(float), 2 * sizeof(float), 1 });
	renderingQuad->setVertexBufferData(renderingQuadVertices, sizeof(renderingQuadVertices) / sizeof(float), 4U, modelLayouts, false);

	/////////////////////////////////// BASE MESH //////////////////////////////////////////////////////////
	
	float gridVertices[6] = {
		0.f, 0.f, 1.f,
		0.f, 0.f, -1.f
	};

	helperGrid.mesh.reset(renderer->createMesh());
	Mesh* helperGridMesh = helperGrid.mesh.get();
	modelLayouts.clear();
	modelLayouts.push_back({ 0, 3, 0, 0, 1 });
	helperGridMesh->setDrawingMode(Mesh::DrawingMode::Lines);
	helperGridMesh->setVertexBufferData(gridVertices, 6, 2, modelLayouts, false);

	renderer->setBufferClearingColor({ 0.01f, 0.01f, 0.01f, 1.f });
	renderer->setBufferClearingBits(true, true, true);
	renderer->enableSetting(GraphicsAPI::Multisampling);
	renderer->enableSetting(GraphicsAPI::GammaCorrection);
	renderer->enableSetting(GraphicsAPI::Blending);
	renderer->enableSetting(GraphicsAPI::DepthTesting);
	renderer->enableSetting(GraphicsAPI::StencilTesting);
	renderer->setStencilMask(0xFF);
}

void RenderingSystem::pushToDrawQueue(Entity drawableEntity, const RenderingComponent& renderingComp)
{
	if (renderingComp.mesh == nullptr)
		return;

	if (renderingComp.lightened)
		lightenedModelsStack.push(drawableEntity);
	else
		coloredModelsStack.push(drawableEntity);
}

void RenderingSystem::setSkybox(Entity skyboxEnt)
{
	skyboxEntity = skyboxEnt;
}

void RenderingSystem::draw(const Scene& scene, const Camera& cam)
{
	if (postProcessingUtility.effectsStates != 0)
		renderWithPostProcessing(scene, cam);
	else
		render(scene, cam);
}

void RenderingSystem::renderWithPostProcessing(const Scene& scene, const Camera& cam)
{
	// Setup offscreen framebuffer
	const glm::uvec4 sceneViewport = renderer->getCurrentViewport();
	postProcessingUtility.helperFramebuffers[0]->activate();
	const glm::uvec2& renderingDims = postProcessingUtility.helperFramebuffers[0]->getRenderingTargetDimensions();
	renderer->setViewport({ 0, 0, renderingDims.x, renderingDims.y });

	render(scene, cam);

	renderer->disableSetting(GraphicsAPI::DepthTesting);
	renderer->disableSetting(GraphicsAPI::FaceCulling);
	renderer->setPolygonFillingMode(GraphicsAPI::FrontAndBack, GraphicsAPI::Fill);

	postProcessingUtility.shader->activate();
	for (uint32_t i = Bloom; i <= Negative; ++i)
		if (postProcessingUtility.effectsStates.test(i))
			applyPostProcessingEffect(PostProcessingEffect(i), postProcessingUtility.effectsValues[i]);

	renderer->activateDefaultFramebuffer();
	renderer->clearFramebuffer();
	renderer->setViewport(sceneViewport);
	uint32_t effect = Copy;
	postProcessingUtility.effectsBuffer->updateData("effect", &effect, sizeof(uint32_t));
	postProcessingUtility.helperFramebuffers[postProcessingUtility.currentFB]->applyColorTexture(0, 0);
	renderer->render(postProcessingUtility.renderingQuad.get());
	postProcessingUtility.currentFB = 0;
}

void RenderingSystem::render(const Scene& scene, const Camera& cam)
{
	// Setup renderer
	renderer->setDepthComparisonFunction(GraphicsAPI::LessOrEqual);		// Prevent skybox z-fighting
	renderer->disableSetting(GraphicsAPI::StencilTesting);
	renderer->setStencilComparisonFunction(GraphicsAPI::Always, 1, 0xFF);
	renderer->enableSetting(GraphicsAPI::DepthTesting);
	renderer->setPolygonFillingMode(GraphicsAPI::FrontAndBack, GraphicsAPI::Fill);
	renderer->clearFramebuffer();

	// Setup camera
	if (cam.projectionChanged)
	{
		cameraTransforms->updateData("projection", glm::value_ptr(cam.getProjection()), sizeof(glm::mat4));
	}

	if (cam.viewChanged)
	{
		cameraTransforms->updateData("view", glm::value_ptr(cam.getView()), sizeof(glm::mat4));
		cameraTransforms->updateData("eyePosition", glm::value_ptr(cam.getPosition()), sizeof(glm::vec4));
	}

	if (helperGridEnabled)
		drawHelperGrid(cam.getPosition());

	// Updating lights
	auto lightEntities = scene.getComponentEntityList<LightComponent>();
	uint32_t lightsCount = (uint32_t)lightEntities.getSize();
	materialsBuffer->updateData("lightSourceCount", &lightsCount, sizeof(uint32_t));
	for (uint32_t lightIndex = 0; lightIndex < lightsCount; ++lightIndex)
	{
		const LightComponent& component = scene.getComponent<LightComponent>(lightEntities[lightIndex]);
		updateShaderLight(component, scene.getComponent<TransformComponent>(lightEntities[lightIndex]), lightIndex);
	}

	// Drawing skybox
	if (skyboxEntity != -1)
	{
		drawSkybox(scene.getComponent<SkyboxComponent>(skyboxEntity));
		skyboxEntity = -1;
	}

	// Drawing lightened models
	uint32_t lightened = 1;
	materialsBuffer->updateData("lightened", &lightened, sizeof(uint32_t));
	generalShader->activate();
	while (!lightenedModelsStack.empty())
	{
		Entity entity = lightenedModelsStack.top();
		const RenderingComponent& renderingComp = scene.getComponent<RenderingComponent>(entity);
		const TransformComponent& transformComp = scene.getComponent<TransformComponent>(entity);

		if (scene.hasComponent<TextureComponent>(entity))
			drawLightenedTextured(renderingComp, transformComp, scene.getComponent<TextureComponent>(entity));
		else
			drawLightened(renderingComp, scene.getComponent<TransformComponent>(entity));

		if (renderingComp.outlined)
			outliningStack.push({ renderingComp, transformComp.getTransformMatrix() });

		lightenedModelsStack.pop();
	}

	lightened = 0;
	materialsBuffer->updateData("lightened", &lightened, sizeof(uint32_t));
	while (!coloredModelsStack.empty())
	{
		Entity entity = coloredModelsStack.top();
		const TransformComponent& transformComp = scene.getComponent<TransformComponent>(entity);
		const RenderingComponent& renderingComp = scene.getComponent<RenderingComponent>(entity);

		if (postProcessingUtility.effectsStates.test(HDR) && scene.hasComponent<LightComponent>(entity))
		{
			RenderingComponent copy = renderingComp;
			copy.color *= scene.getComponent<LightComponent>(entity).strength;
			drawColored(copy, transformComp);
		}
		else
		{
			drawColored(renderingComp, transformComp);
		}

		if (renderingComp.outlined)
			outliningStack.push({ renderingComp, transformComp.getTransformMatrix() });

		coloredModelsStack.pop();
	}

	// Outlining meshes
	if (!outliningStack.empty())
		outlineMeshes();
}

void RenderingSystem::setPostProcessingEffectState(PostProcessingEffect effect, bool value)
{
	postProcessingUtility.effectsStates.set(effect, value);
}

bool RenderingSystem::getPostProcessingEffectState(PostProcessingEffect effect) const
{
	return bool(postProcessingUtility.effectsStates.test(effect));
}

float& RenderingSystem::getPostProcessingEffectValue(PostProcessingEffect effect)
{
	return postProcessingUtility.effectsValues[effect];
}

void RenderingSystem::applyPostProcessingEffect(PostProcessingEffect effect, float effectValue)
{
	const Framebuffer& fb0 = *postProcessingUtility.helperFramebuffers[0];
	const Framebuffer& fb1 = *postProcessingUtility.helperFramebuffers[1];
	const Framebuffer& fb2 = *postProcessingUtility.helperFramebuffers[2];

	switch (postProcessingUtility.currentFB)
	{
		case 0:
			if (effect == PostProcessingEffect::Bloom)
				applyBloom((int)effectValue);

			fb1.activate();
			fb0.applyColorTexture(0, 0);
			postProcessingUtility.currentFB = 1;
			break;
		case 1:
			fb0.activate();
			fb1.applyColorTexture(0, 0);
			postProcessingUtility.currentFB = 0;
			break;
		case 2:
			fb0.activate();
			fb2.applyColorTexture(0, 0);
			postProcessingUtility.currentFB = 0;
			break;
	}

	postProcessingUtility.effectsBuffer->updateData("effect", (int*)&effect, sizeof(int));
	postProcessingUtility.effectsBuffer->updateData("effectStrength", &effectValue, sizeof(float));
	renderer->render(postProcessingUtility.renderingQuad.get());
}

void RenderingSystem::applyBloom(int bloomIntensity)
{
	const Framebuffer& fb1 = *postProcessingUtility.helperFramebuffers[1];
	const Framebuffer& fb2 = *postProcessingUtility.helperFramebuffers[2];

	fb2.activate();
	postProcessingUtility.helperFramebuffers[postProcessingUtility.currentFB]->applyColorTexture(0, 0);
	uint32_t effect = HelperPostProcessingEffect::Extract;
	postProcessingUtility.effectsBuffer->updateData("effect", &effect, sizeof(uint32_t));
	renderer->render(postProcessingUtility.renderingQuad.get());

	for (int i = 0; i < bloomIntensity; ++i)
	{
		fb1.activate();
		fb2.applyColorTexture(0, 0);
		effect = HelperPostProcessingEffect::HorizontalGaussianBlur;
		postProcessingUtility.effectsBuffer->updateData("effect", &effect, sizeof(uint32_t));
		renderer->render(postProcessingUtility.renderingQuad.get());

		fb2.activate();
		fb1.applyColorTexture(0, 0);
		effect = HelperPostProcessingEffect::VerticalGaussianBlur;
		postProcessingUtility.effectsBuffer->updateData("effect", &effect, sizeof(uint32_t));
		renderer->render(postProcessingUtility.renderingQuad.get());
	}

	fb2.applyColorTexture(1, 0);
}

void RenderingSystem::setupRenderer(const RenderingComponent& renderingComp) const
{
	if (renderingComp.culled)
		renderer->enableSetting(GraphicsAPI::FaceCulling);
	else
		renderer->disableSetting(GraphicsAPI::FaceCulling);

	if (renderingComp.wireframe)
		renderer->setPolygonFillingMode(GraphicsAPI::FrontAndBack, GraphicsAPI::Lines);
	else
		renderer->setPolygonFillingMode(GraphicsAPI::FrontAndBack, GraphicsAPI::Fill);

	if (renderingComp.outlined)
	{
		renderer->enableSetting(GraphicsAPI::StencilTesting);
		renderer->setStencilOperation(GraphicsAPI::Keep, GraphicsAPI::Keep, GraphicsAPI::Replace);
	}
	else
	{
		renderer->disableSetting(GraphicsAPI::StencilTesting);
		renderer->setStencilOperation(GraphicsAPI::Keep, GraphicsAPI::Keep, GraphicsAPI::Keep);
	}
}

void RenderingSystem::drawColored(const RenderingComponent& renderingComp, const TransformComponent& transformComp) const 
{
	setupRenderer(renderingComp);
	materialsBuffer->updateData("material.color", &renderingComp.color.x, sizeof(glm::vec4));
	cameraTransforms->updateData("model", &transformComp.getTransformMatrix()[0][0], sizeof(glm::mat4));

	renderer->render(renderingComp.mesh.get());
}

void RenderingSystem::drawLightened(const RenderingComponent& renderingComp, const TransformComponent& transformComp) const
{
	uint32_t state = 0;
	setupRenderer(renderingComp);
	cameraTransforms->updateData("model", &transformComp.getTransformMatrix()[0][0], sizeof(glm::mat4));
	materialsBuffer->updateData("material.color", &renderingComp.color, sizeof(glm::vec4));
	materialsBuffer->updateData("state", &state, sizeof(uint32_t));
	updateShaderMaterialData(renderingComp.color, renderingComp.material);

	renderer->render(renderingComp.mesh.get());
}

void RenderingSystem::drawLightenedTextured(const RenderingComponent& renderingComp, const TransformComponent& transformComp, const TextureComponent& textureComp) const
{
	uint32_t state;
	if (textureComp.diffuseMap != nullptr)
	{
		textureComp.diffuseMap->setForSampling(0);
		state = 1;
	}
	else
	{
		drawLightened(renderingComp, transformComp);
	}

	if (textureComp.specularMap != nullptr)
	{
		textureComp.specularMap->setForSampling(1);
		state = 2;
	}

	//if (textureComp.normalMap != nullptr)
		//textureComp.normalMap->setForSampling(3);

	/*if (textureComp.skyboxReflection && skyboxEntity != -1)
	{
		float indexRatio = 0.0f;
		materialsBuffer->updateData("indexRatio", &indexRatio, sizeof(float));
		skybox->component.texture->setForSampling(2);
		state = 3;
	}*/

	setupRenderer(renderingComp);
	materialsBuffer->updateData("state", &state, sizeof(uint32_t));
	cameraTransforms->updateData("model", &transformComp.getTransformMatrix()[0][0], sizeof(glm::mat4));
	updateShaderMaterialData(renderingComp.color, renderingComp.material);

	renderer->render(renderingComp.mesh.get());
}

void RenderingSystem::drawSkybox(const SkyboxComponent& skyboxComp) const
{
	skyboxShader->activate();
	skyboxBlurBuffer->updateData("blurValue", &skyboxComp.blurLevel, sizeof(float));
	skyboxComp.texture->setForSampling(0);
	renderer->disableSetting(GraphicsAPI::FaceCulling);
	renderer->render(skyboxMesh.get());
}

void RenderingSystem::outlineMeshes()
{
	bool depthTestingState = renderer->getSettingState(GraphicsAPI::DepthTesting);
	renderer->enableSetting(GraphicsAPI::StencilTesting);
	renderer->disableSetting(GraphicsAPI::DepthTesting);
	renderer->setPolygonFillingMode(GraphicsAPI::FrontAndBack, GraphicsAPI::Fill);
	renderer->setStencilComparisonFunction(GraphicsAPI::NotEqual, 1, 0xFF);
	renderer->setStencilOperation(GraphicsAPI::Keep, GraphicsAPI::Keep, GraphicsAPI::Keep);

	while (!outliningStack.empty())
	{
		const auto& entity = outliningStack.top();
		glm::vec3 outliningColor = 1.f - entity.first.color;
		materialsBuffer->updateData("material.color", &outliningColor.x, sizeof(glm::vec4));
		cameraTransforms->updateData("model", &glm::scale(entity.second, glm::vec3(1.1f))[0][0], sizeof(glm::mat4));
		renderer->render(entity.first.mesh.get());
		outliningStack.pop();
	}

	if (depthTestingState)
		renderer->enableSetting(GraphicsAPI::DepthTesting);
}

void RenderingSystem::drawHelperGrid(const glm::vec3& camPosition)
{
	helperGrid.shader->activate();
	renderer->disableSetting(GraphicsAPI::FaceCulling);
	const float distance = glm::floor(glm::length(camPosition));
	if (distance != helperGrid.updateDistance)
	{
		helperGrid.updateDistance = distance;
		helperGrid.gridTransform = glm::translate(glm::mat4(1.f), glm::vec3(glm::floor(camPosition.x), 0.f, glm::floor(camPosition.z)));
		helperGrid.gridTransform = glm::scale(helperGrid.gridTransform, glm::vec3((float)helperGrid.gridSize));
	}

	cameraTransforms->updateData("model", glm::value_ptr(helperGrid.gridTransform), sizeof(glm::mat4));
	renderer->render(helperGrid.mesh.get());
}

void RenderingSystem::updateShaderLight(const LightComponent& light, const TransformComponent& transform, uint32_t lightIndex) const
{
	glm::vec3 color, direction;
	float constant, linear, quadratic, cutoff;

	switch (light.type)
	{
		case LightComponent::LightSourceType::Point:
		{
			color = light.strength * light.color;
			direction = glm::vec3(0.f, 0.f, 0.f);
			constant = 1.1f;
			linear = 0.024f;
			quadratic = 0.0021f;
			cutoff = -1.f;
			break;
		}
		case LightComponent::LightSourceType::Directional:
		{
			color = glm::vec4(light.strength * light.color, 1.f);
			direction = glm::vec3(0.f, -1.f, 0.f);
			constant = 0.f;
			linear = 0.f;
			quadratic = 0.f;
			cutoff = 1.1f;
			break;
		}
		case LightComponent::LightSourceType::Cone:
		{
			color = glm::vec4(light.strength * light.color, 1.f);
			direction = glm::vec4(glm::vec3(transform.getRotationMatrix() * glm::vec4(0.f, -1.f, 0.f, 0.f)), 0.f);
			constant = 1.f;
			linear = 0.045f;
			quadratic = 0.0075f;
			cutoff = glm::cos(glm::radians(light.coneCutoff));
			break;
		}
	}

	std::string lightsString = "lights[" + std::to_string(lightIndex) + "].";
	lightsBuffer->updateData(lightsString + "position", &transform.getTranslation().x, sizeof(glm::vec4));
	lightsBuffer->updateData(lightsString + "color", &color, sizeof(glm::vec4));
	lightsBuffer->updateData(lightsString + "direction", &direction, sizeof(glm::vec4));
	lightsBuffer->updateData(lightsString + "constant", &constant, sizeof(float));
	lightsBuffer->updateData(lightsString + "linear", &linear, sizeof(float));
	lightsBuffer->updateData(lightsString + "quadratic", &quadratic, sizeof(float));
	lightsBuffer->updateData(lightsString + "cutoff", &cutoff, sizeof(float));
}

void RenderingSystem::updateShaderMaterialData(const glm::vec4& color, RenderingComponent::Material material) const
{
	float ambientCoefficient, diffuseCoefficient, specularCoefficient, shininess;
	switch (material)
	{
		case RenderingComponent::Material::PLASTIC:
		{
			ambientCoefficient = 0.05f;
			diffuseCoefficient = 0.8f;
			specularCoefficient = 0.6f;
			shininess = 10.f;
		}
		break;

		case RenderingComponent::Material::METAL:
		{
			ambientCoefficient = 0.2f;
			diffuseCoefficient = 0.9f;
			specularCoefficient = 1.f;
			shininess = 90.f;
		}
		break;

	case RenderingComponent::Material::FABRIC:
		break;
	default:
		break;
	}

	materialsBuffer->updateData("material.ambientCoefficient", &ambientCoefficient, sizeof(float));
	materialsBuffer->updateData("material.diffuseCoefficient", &diffuseCoefficient, sizeof(float));
	materialsBuffer->updateData("material.specularCoefficient", &specularCoefficient, sizeof(float));
	materialsBuffer->updateData("material.shininess", &shininess, sizeof(float));
	materialsBuffer->updateData("material.color", &color, sizeof(glm::vec4));
}