#pragma once 
#include "IRenderer.h"
#include "Rendering/Texture.h"

namespace Cala {
	class SkyboxRenderer : public IRenderer {
	public:
		SkyboxRenderer(Texture* tex);
		~SkyboxRenderer() override = default;
		void render(const GraphicsAPI* api, const Camera& camera) override;
		void setTexture(Texture* tex) { texture = tex; }

	private:
		Mesh mesh;
		Shader shader;
		ConstantBuffer mvpBuffer;
		ConstantBuffer skyboxBlurBuffer;
		Texture* texture = nullptr;
	};
}