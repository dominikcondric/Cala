#pragma once 
#include "ICameraRenderer.h"
#include "Cala/Rendering/Texture.h"
#include "Cala/Rendering/Shader.h"

namespace Cala {
	class SkyboxRenderer : public ICameraRenderer {
	public:
		SkyboxRenderer(const Texture* _texture = nullptr);
        ~SkyboxRenderer() override = default;
        void render(GraphicsAPI *const api, const Framebuffer *renderingTarget);
		void setupCamera(const Camera& camera) override;
		void setTexture(const Texture* _texture);
        void setBlurLevel(uint32_t blur);

    private:
		Mesh mesh;
		Shader shader;
		ConstantBuffer mvpBuffer;
		ConstantBuffer skyboxBlurBuffer;
		const Texture* texture = nullptr;
	};
}