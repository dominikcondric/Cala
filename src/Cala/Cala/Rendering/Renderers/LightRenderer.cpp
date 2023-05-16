#include "LightRenderer.h"
#include <glad/glad.h>
#include <iostream>
#include <glm/gtx/string_cast.hpp>

#define BIT(x) (1 << x)

#define DIFFUSE_MAP_BINDING 0
#define SPECULAR_MAP_BINDING 1
#define NORMAL_MAP_BINDING 2
#define SHADOW_MAP_BINDING 3

namespace Cala {
	LightRenderer::LightRenderer(glm::uvec2 shadowMapDimensions)
	{
		std::filesystem::path shadersDir(SHADERS_DIR);
		mainShader.attachShader(Shader::ShaderType::VertexShader, shadersDir / "GeneralVertexShader.glsl");
		mainShader.attachShader(Shader::ShaderType::FragmentShader, shadersDir / "LightFragmentShader.glsl");
		mainShader.createProgram();

		shadowPassShader.attachShader(Shader::ShaderType::VertexShader, shadersDir / "ShadowPassVertexShader.glsl");
		shadowPassShader.attachShader(Shader::ShaderType::GeometryShader, shadersDir / "ShadowPassGeometryShader.glsl");
		shadowPassShader.attachShader(Shader::ShaderType::FragmentShader, shadersDir / "ShadowPassFragmentShader.glsl");
		shadowPassShader.createProgram();

		mvpBuffer.setData(mainShader.getConstantBufferInfo("MVP"), true);
		materialsBuffer.setData(mainShader.getConstantBufferInfo("MeshData"), true);
		lightsBuffer.setData(mainShader.getConstantBufferInfo("LightsData"), true);

		TextureArray* depthTextureArray = new TextureArray;
		Texture::Specification depthTextureSpecification(
			shadowMapDimensions.x, shadowMapDimensions.y, ITexture::Format::DEPTH32, 
			Texture::Dimensionality::TwoDimensional
		);

		depthTextureArray->load(depthTextureSpecification, 0, MAX_LIGHTS_COUNT * 6); // MAX_LIGHTS * 6 faces of a cubemap in case of point lights
		shadowsFramebuffer.addDepthTarget(depthTextureArray, true, 0);
		shadowsFramebuffer.load();
	}

    void LightRenderer::pushRenderable(const Renderable& renderable)
	{
		int state = 0;

		if (renderable.diffuseMap != nullptr)
			state |= BIT(0);

		if (renderable.specularMap != nullptr)
			state |= BIT(1);

		if (renderable.normalMap != nullptr)
			state |= BIT(2);

		renderables[state].push_back(renderable);
	}

	void LightRenderer::pushLight(const Light& light)
	{
		if (lights.size() < MAX_LIGHTS_COUNT)
			lights.push_back(light);
	}

	void LightRenderer::updateLight(const Light& light, uint32_t lightIndex)
	{
		/** 
		 * Setting up light uniforms
		*/
		glm::vec3 color, direction;
		float constant, linear, quadratic, cutoff;
		glm::mat4 projection;

		switch (light.type)
		{
			case Light::Type::Point:
			{
				color = light.intensity * light.color;
				direction = glm::vec3(0.f, 0.f, 0.f);
				constant = 1.1f;
				linear = 0.024f;
				quadratic = 0.0021f;
				cutoff = -2.f;
				projection = glm::perspective(90.f, 1.f, 1.f, light.intensity * 100.f);
				break;
			}
			case Light::Type::Directional:
			{
				color = light.intensity * light.color;
				direction = glm::vec3(0.f, -1.f, 0.f);
				constant = 0.f;
				linear = 0.f;
				quadratic = 0.f;
				cutoff = -3.f;
				projection = glm::ortho(-50.f, 50.f, -50.f, 50.f, 1.f, 100.f);
				break;
			}
			case Light::Type::Spotlight:
			{
				color = light.intensity * light.color;
				direction = glm::vec3(light.transformation.getRotationMatrix() * glm::vec4(0.f, -1.f, 0.f, 0.f));
				constant = 1.f;
				linear = 0.045f;
				quadratic = 0.0075f;
				cutoff = glm::cos(glm::radians(light.spotlightCutoff));
				projection = glm::perspective(light.spotlightCutoff * 2.f, 1.f, 1.f, light.intensity * 100.f);
				break;
			}
		}

		if (!light.shadowCaster)
			projection = glm::mat4(0.f);

		std::string lightsString = "lights[" + std::to_string(lightIndex) + "].";
		lightsBuffer.updateData(lightsString + "position", &light.transformation.getTranslation().x, sizeof(glm::vec4));
		lightsBuffer.updateData(lightsString + "color", &color, sizeof(glm::vec4));
		lightsBuffer.updateData(lightsString + "direction", &direction, sizeof(glm::vec4));
		lightsBuffer.updateData(lightsString + "constant", &constant, sizeof(float));
		lightsBuffer.updateData(lightsString + "linear", &linear, sizeof(float));
		lightsBuffer.updateData(lightsString + "quadratic", &quadratic, sizeof(float));
		lightsBuffer.updateData(lightsString + "cutoff", &cutoff, sizeof(float));
		lightsBuffer.updateData(lightsString + "projection", &projection[0][0], sizeof(glm::mat4));
	}

	void LightRenderer::setupCamera(const Camera &camera)
    {
		mvpBuffer.updateData("eyePosition", &camera.getPosition().x, sizeof(glm::vec4));
		mvpBuffer.updateData("view", &camera.getView()[0][0], sizeof(glm::mat4));
		mvpBuffer.updateData("projection", &camera.getProjection()[0][0], sizeof(glm::mat4));
    }

    void LightRenderer::render(GraphicsAPI* const api, const Framebuffer* renderingTarget)
    {
		int shadowsInt = shadows;
		lightsBuffer.updateData("shadows", &shadowsInt, sizeof(shadowsInt));
		lightsBuffer.updateData("celShadingLevelCount", &celShadingLevelCount, sizeof(uint32_t));

		auto currentViewport = api->getCurrentViewport();

		/**
		 * Setting up shader for light setup
		*/ 
		const uint32_t lightsCount = (uint32_t)lights.size();
		lightsBuffer.updateData("lightSourceCount", &lightsCount, sizeof(uint32_t));

		for (uint32_t lightCounter = 0U; lightCounter < lights.size(); ++lightCounter)
		{
			updateLight(lights[lightCounter], lightCounter);
		}
		lights.clear();

		/**
		 * Rendering to shadow map
		*/
		api->enableSetting(GraphicsAPI::DepthTesting);
		if (shadows)
		{
			shadowPassShader.activate();
			api->setBufferClearingBits(false, true, false);
			api->activateFramebuffer(&shadowsFramebuffer);
			glm::ivec2 depthTextureSize = shadowsFramebuffer.getDepthTarget().getDimensions();
			api->setViewport({ 0, 0, depthTextureSize.x, depthTextureSize.y });
			api->clearFramebuffer();
			for (int state = 0; state < 8; state++)
			{
				for (const Renderable& renderable : renderables[state])
				{
					mvpBuffer.updateData("model", &renderable.transformation.getTransformMatrix()[0][0], sizeof(glm::mat4));
					api->render(renderable.mesh);
				}
			}

			shadowsFramebuffer.getDepthTarget().setForSampling(SHADOW_MAP_BINDING);
		}

		/**
		 * Setting up uniforms for normal rendering
		*/
		mainShader.activate();
		

		api->activateFramebuffer(renderingTarget);			
		api->setViewport(currentViewport);
		api->setBufferClearingBits(true, true, true);
		int lightened = 1;
		materialsBuffer.updateData("lightened", &lightened, sizeof(int));

		for (uint32_t state = 0; state < 8; ++state)
		{
			if (!renderables[state].empty())
			{
				materialsBuffer.updateData("state", &state, sizeof(int));
			}

			for (const Renderable& renderable : renderables[state])
			{
				if ((state & BIT(DIFFUSE_MAP_BINDING)) != 0)
					renderable.diffuseMap->setForSampling(DIFFUSE_MAP_BINDING);

				if ((state & BIT(SPECULAR_MAP_BINDING)) != 0)
					renderable.specularMap->setForSampling(SPECULAR_MAP_BINDING);

				if ((state & BIT(NORMAL_MAP_BINDING)) != 0)
					renderable.normalMap->setForSampling(NORMAL_MAP_BINDING);

				mvpBuffer.updateData("model", &renderable.transformation.getTransformMatrix()[0][0], sizeof(glm::mat4));
				materialsBuffer.updateData("material.color", &renderable.color.x, sizeof(glm::vec4));
				materialsBuffer.updateData("material.ambientCoefficient", &renderable.ambientCoefficient, sizeof(float));
				materialsBuffer.updateData("material.diffuseCoefficient", &renderable.diffuseCoefficient, sizeof(float));
				materialsBuffer.updateData("material.specularCoefficient", &renderable.specularCoefficient, sizeof(float));
				materialsBuffer.updateData("material.shininess", &renderable.shininess, sizeof(float));
				api->render(renderable.mesh);
			}

			renderables[state].clear();
		}

		api->disableSetting(GraphicsAPI::DepthTesting);
    }
}
