#pragma once 
#include "ICameraRenderer.h"
#include "Cala/Rendering/Mesh.h"
#include "Cala/Rendering/Shader.h"
#include "Cala/Rendering/ConstantBuffer.h"
#include "Cala/Utility/Transformation.h"

namespace Cala {
	class HelperGridRenderer : public ICameraRenderer {
	public:
		HelperGridRenderer();
		~HelperGridRenderer() = default;
		void render(GraphicsAPI* const api, const Framebuffer* renderingTarget) override;
		void setupCamera(const Camera& camera) override;
		void setMoveXZWithCamera(bool move);

		glm::vec4 gridColor{ 0.1f, 0.1f, 0.1f, 0.1f };

	private:
		Shader shader;
		ConstantBuffer mvpBuffer;
		ConstantBuffer meshDataBuffer;
		Mesh gridMesh;
		uint32_t gridSize = 50;
		Transformation transformation;
		bool moveXZWithCamera = true;
	};
}