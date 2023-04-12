#pragma once 
#include "IRenderer.h"
#include "Cala/Rendering/Texture.h"

namespace Cala {
	class SkyboxRenderer : public IRenderer {
	public:
		SkyboxRenderer(const Texture* _texture = nullptr);
		~SkyboxRenderer() override = default;
		void render(GraphicsAPI* const api, const Camera& camera) override;
		void setTexture(const Texture* _texture);

	private:
		Mesh mesh;
		Shader shader;
		ConstantBuffer mvpBuffer;
		ConstantBuffer skyboxBlurBuffer;
		const Texture* texture = nullptr;
	};
}