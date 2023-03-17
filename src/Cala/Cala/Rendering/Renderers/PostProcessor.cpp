#include "PostProcessor.h"
#include "Cala/Utility/Core.h"
#include <algorithm>

namespace Cala {
	PostProcessor::PostProcessor()
	{
		std::filesystem::path shadersDir(SHADERS_DIR);
		shader.attachShader(Shader::ShaderType::VertexShader, shadersDir / "PostProcessingVertexShader.glsl");
		shader.attachShader(Shader::ShaderType::FragmentShader, shadersDir / "PostProcessingFragmentShader.glsl");
		shader.createProgram();

		helperFramebuffers[0].generateRenderingTarget(1024, 768, 1);
		helperFramebuffers[1].generateColorBuffer(1024, 768, 1);
		helperFramebuffers[2].generateColorBuffer(1024, 768, 1);

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
	}

	void PostProcessor::begin(GraphicsAPI* api)
	{
		if (beginCalled) 
		{
			LOG_ERROR("Begin already called!");
			return;
		}

		// Setup offscreen framebuffer
		sceneViewport = api->getCurrentViewport();
		helperFramebuffers[0].activate();
		api->clearFramebuffer();
		const glm::uvec2& renderingDims = helperFramebuffers[0].getRenderingTargetDimensions();
		api->setViewport({ 0, 0, renderingDims.x, renderingDims.y });

		api->disableSetting(GraphicsAPI::DepthTesting);
		api->disableSetting(GraphicsAPI::FaceCulling);
		api->setPolygonFillingMode(GraphicsAPI::FrontAndBack, GraphicsAPI::Fill);
		beginCalled = true;
	}

	void PostProcessor::applyEffects(GraphicsAPI* api, std::vector<std::pair<PostProcessingEffect, float>> effects)
	{
		if (!beginCalled)
		{
			LOG_ERROR("Begin not called!");
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
		for (const auto& [effect, value] : effects)
			applyEffect(api, effect, value);

		api->activateDefaultFramebuffer();
		api->clearFramebuffer();
		api->setViewport(sceneViewport);
		uint32_t effect = Copy;
		effectsBuffer.updateData("effect", &effect, sizeof(uint32_t));
		helperFramebuffers[currentFB].applyColorTexture(0, 0);
		api->render(renderingQuad);
		currentFB = 0;
		beginCalled = false;
	}

	void PostProcessor::applyEffect(const GraphicsAPI* api, PostProcessingEffect effect, float effectValue)
	{
		const Framebuffer& fb0 = helperFramebuffers[0];
		const Framebuffer& fb1 = helperFramebuffers[1];
		const Framebuffer& fb2 = helperFramebuffers[2];

		switch (currentFB)
		{
			case 0:
				if (effect == PostProcessingEffect::Bloom)
					applyBloom(api, (int)effectValue);

				fb1.activate();
				fb0.applyColorTexture(0, 0);
				currentFB = 1;
				break;
			case 1:
				fb0.activate();
				fb1.applyColorTexture(0, 0);
				currentFB = 0;
				break;
			case 2:
				fb0.activate();
				fb2.applyColorTexture(0, 0);
				currentFB = 0;
				break;
		}

		effectsBuffer.updateData("effect", (int*)&effect, sizeof(int));
		effectsBuffer.updateData("effectStrength", &effectValue, sizeof(float));
		api->render(renderingQuad);
	}

	void PostProcessor::applyBloom(const GraphicsAPI* api, int bloomIntensity)
	{
		const Framebuffer& fb1 = helperFramebuffers[1];
		const Framebuffer& fb2 = helperFramebuffers[2];

		fb2.activate();
		helperFramebuffers[currentFB].applyColorTexture(0, 0);
		uint32_t effect = HelperPostProcessingEffect::Extract;
		effectsBuffer.updateData("effect", &effect, sizeof(uint32_t));
		api->render(renderingQuad);

		for (int i = 0; i < bloomIntensity; ++i)
		{
			fb1.activate();
			fb2.applyColorTexture(0, 0);
			effect = HelperPostProcessingEffect::HorizontalGaussianBlur;
			effectsBuffer.updateData("effect", &effect, sizeof(uint32_t));
			api->render(renderingQuad);

			fb2.activate();
			fb1.applyColorTexture(0, 0);
			effect = HelperPostProcessingEffect::VerticalGaussianBlur;
			effectsBuffer.updateData("effect", &effect, sizeof(uint32_t));
			api->render(renderingQuad);
		}

		fb2.applyColorTexture(1, 0);
	}
}
