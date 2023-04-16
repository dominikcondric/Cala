#pragma once
#include "IRenderer.h"
#include "Cala/Rendering/Framebuffer.h"
#include "Cala/Rendering/GraphicsAPI.h"
#include "Cala/Rendering/Shader.h"
#include "Cala/Rendering/ConstantBuffer.h"

namespace Cala {
	class PostProcessingRenderer : IRenderer {
	public:
		PostProcessingRenderer();
		~PostProcessingRenderer() = default;
		void begin(GraphicsAPI* api);
        void render(GraphicsAPI *api, const Framebuffer *renderingTarget);
		const Framebuffer* getRenderingTarget() const { return &helperFramebuffers[0]; }

        enum PostProcessingEffect {
			Bloom = 0,
			HDR = 1,
			BoxBlur = 2,
			EdgeDetection = 3,
			GaussianBlur = 4,
			Negative = 5
		};

		void pushEffect(PostProcessingEffect effect, float effectValue);

	private:
		enum HelperPostProcessingEffect {
			HorizontalGaussianBlur = 6,
			VerticalGaussianBlur = 7,
			Extract = 8,
			Copy = 9
		};

		void applyEffect(GraphicsAPI* const api, PostProcessingEffect effect, float effectValue);
		void applyBloom(GraphicsAPI* const api, int bloomValue);

		std::vector<std::pair<PostProcessingEffect, float>> effects;
		bool beginCalled = false;
		Framebuffer helperFramebuffers[3];
		Mesh renderingQuad;
		uint8_t currentFB = 0;
		Shader shader;
		ConstantBuffer effectsBuffer;
		glm::uvec4 sceneViewport;
		const glm::uvec2 framebufferTargetsSize{ 1920, 1080 };
	};
}