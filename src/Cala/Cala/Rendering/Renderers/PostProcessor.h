#pragma once
#include "Cala/Rendering/Framebuffer.h"
#include "Cala/Rendering/GraphicsAPI.h"
#include "Cala/Rendering/Shader.h"
#include "Cala/Rendering/ConstantBuffer.h"

namespace Cala {
	class PostProcessor {
	public:
		PostProcessor();
		~PostProcessor() = default;
		void begin(GraphicsAPI* api);

		enum PostProcessingEffect {
			Bloom = 0,
			HDR = 1,
			BoxBlur = 2,
			EdgeDetection = 3,
			GaussianBlur = 4,
			Negative = 5
		};

		void applyEffects(GraphicsAPI* api, std::vector<std::pair<PostProcessingEffect, float>> effects);


	private:
		enum HelperPostProcessingEffect {
			HorizontalGaussianBlur = 6,
			VerticalGaussianBlur = 7,
			Extract = 8,
			Copy = 9
		};

		void applyEffect(const GraphicsAPI* api, PostProcessingEffect effect, float effectValue);
		void applyBloom(const GraphicsAPI* api, int bloomValue);

		bool beginCalled = false;
		Framebuffer helperFramebuffers[3];
		Mesh renderingQuad;
		uint8_t currentFB = 0;
		Shader shader;
		ConstantBuffer effectsBuffer;
		glm::uvec4 sceneViewport;
	};
}