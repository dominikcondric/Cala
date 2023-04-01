#include "PostProcessingRenderer.h"
#include <algorithm>
#include "Cala/Utility/Logger.h"

namespace Cala {
	PostProcessingRenderer::PostProcessingRenderer()
	{
		std::filesystem::path shadersDir(SHADERS_DIR);
		shader.attachShader(Shader::ShaderType::VertexShader, shadersDir / "PostProcessingVertexShader.glsl");
		shader.attachShader(Shader::ShaderType::FragmentShader, shadersDir / "PostProcessingFragmentShader.glsl");
		shader.createProgram();

		Texture::Specification texSpec(framebufferTargetsSize.x, framebufferTargetsSize.y, ITexture::Format::RGBA, Texture::Dimensionality::TwoDimensional);
		for (int i = 0; i < 3; ++i)
		{
			Texture* colorTarget = new Texture;
			colorTarget->generateTexture(texSpec, nullptr);
			helperFramebuffers[i].addColorTarget(colorTarget, true);

			if (i == 0)
			{
				Texture* depthTarget = new Texture;
				Texture::Specification depthTexSpec(
					framebufferTargetsSize.x, framebufferTargetsSize.y, Texture::Format::DEPTH24_STENCIL8, Texture::Dimensionality::TwoDimensional);
				depthTarget->generateTexture(depthTexSpec, nullptr);
				helperFramebuffers[i].addDepthTarget(depthTarget, true);
			}

			helperFramebuffers[i].loadFramebuffer();
		}	

		effectsBuffer.setData(shader.getConstantBufferInfo("EffectValues"), true);

		Model quadModel;
		std::vector<float> renderingQuadVertices = {
			-1.f, 1.f, 0.f, 1.f,
			-1.f, -1.f, 0.f, 0.f,
			1.f, 1.f, 1.f, 1.f,
			1.f, -1.f, 1.f, 0.f
		};

		std::vector<Model::VertexLayoutSpecification> modelLayouts;
		modelLayouts.push_back({ 0, 2, 4 * sizeof(float), 0, 1 });
		modelLayouts.push_back({ 1, 2, 4 * sizeof(float), 2 * sizeof(float), 1 });

		quadModel.loadCustomModel(renderingQuadVertices, 4, {}, modelLayouts, Model::DrawingMode::TriangleStrip);
		renderingQuad.loadMesh(quadModel, false);

		effects.reserve(5);
	}

	void PostProcessingRenderer::begin(GraphicsAPI* api)
	{
		if (beginCalled) 
		{
			Logger::getInstance().logErrorToConsole("Begin already called!");
			return;
		}

		// Setup offscreen framebuffer
		sceneViewport = api->getCurrentViewport();
		api->activateFramebuffer(helperFramebuffers[0]);
		api->clearFramebuffer();
		api->setViewport({ 0, 0, framebufferTargetsSize.x, framebufferTargetsSize.y });

		beginCalled = true;
	}

	void PostProcessingRenderer::render(GraphicsAPI* api, const Camera& camera)
	{
		if (!beginCalled)
		{
			Logger::getInstance().logErrorToConsole("Begin not called!");
			return;
		}

		using EffectPair = std::pair<PostProcessingEffect, float>;

		auto comparator = [](const EffectPair& p) {
			return p.first == PostProcessingEffect::Bloom;
		};

		auto iterator = std::find_if(effects.begin(), effects.end(), comparator);
		if (iterator != effects.end())
		{
			EffectPair tmp = *iterator;
			*iterator = effects[0];
			effects[0] = tmp;
		}

		shader.activate();
		api->disableSetting(GraphicsAPI::DepthTesting);
		api->disableSetting(GraphicsAPI::FaceCulling);
		api->setPolygonFillingMode(GraphicsAPI::FrontAndBack, GraphicsAPI::Fill);

		for (const auto& [effect, value] : effects)
			applyEffect(api, effect, value);

		effects.clear();
		api->activateDefaultFramebuffer();
		api->setViewport(sceneViewport);
		uint32_t effect = Copy;
		effectsBuffer.updateData("effect", &effect, sizeof(uint32_t));
		helperFramebuffers[currentFB].getColorTarget(0).setForSampling(0);
		api->render(renderingQuad);
		currentFB = 0;
		beginCalled = false;
	}

    void PostProcessingRenderer::pushEffect(PostProcessingEffect effect, float effectValue)
    {
		effects.emplace_back(effect, effectValue);
    }

    void PostProcessingRenderer::applyEffect(GraphicsAPI *const api, PostProcessingEffect effect, float effectValue)
    {
		const Framebuffer& fb0 = helperFramebuffers[0];
		const Framebuffer& fb1 = helperFramebuffers[1];
		const Framebuffer& fb2 = helperFramebuffers[2];

		switch (currentFB)
		{
			case 0:
				if (effect == PostProcessingEffect::Bloom)
					applyBloom(api, (int)effectValue);

				api->activateFramebuffer(fb1);
				fb0.getColorTarget(0).setForSampling(0);
				currentFB = 1;
				break;
			case 1:
				api->activateFramebuffer(fb0);
				fb1.getColorTarget(0).setForSampling(0);
				currentFB = 0;
				break;
			case 2:
				api->activateFramebuffer(fb0);
				fb2.getColorTarget(0).setForSampling(0);
				currentFB = 0;
				break;
		}

		effectsBuffer.updateData("effect", (int*)&effect, sizeof(int));
		effectsBuffer.updateData("effectStrength", &effectValue, sizeof(float));
		api->render(renderingQuad);
	}

	void PostProcessingRenderer::applyBloom(GraphicsAPI* const api, int bloomIntensity)
	{
		const Framebuffer& fb1 = helperFramebuffers[1];
		const Framebuffer& fb2 = helperFramebuffers[2];

		api->activateFramebuffer(fb2);
		helperFramebuffers[currentFB].getColorTarget(0).setForSampling(0);
		uint32_t effect = HelperPostProcessingEffect::Extract;
		effectsBuffer.updateData("effect", &effect, sizeof(uint32_t));
		api->render(renderingQuad);

		for (int i = 0; i < bloomIntensity; ++i)
		{
			api->activateFramebuffer(fb1);
			fb2.getColorTarget(0).setForSampling(0);
			effect = HelperPostProcessingEffect::HorizontalGaussianBlur;
			effectsBuffer.updateData("effect", &effect, sizeof(uint32_t));
			api->render(renderingQuad);

			api->activateFramebuffer(fb2);
			fb1.getColorTarget(0).setForSampling(0);
			effect = HelperPostProcessingEffect::VerticalGaussianBlur;
			effectsBuffer.updateData("effect", &effect, sizeof(uint32_t));
			api->render(renderingQuad);
		}

	fb2.getColorTarget(0).setForSampling(1);
	}
}
