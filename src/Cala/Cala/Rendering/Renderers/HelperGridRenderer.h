#pragma once 
#include "Cala/Rendering/GraphicsAPI.h"
#include "Cala/Rendering/Mesh.h"
#include "Cala/Rendering/Shader.h"
#include "Cala/Rendering/ConstantBuffer.h"
#include "Cala/Rendering/Camera.h"
#include "IRenderer.h"
#include "Cala/Utility/Transformation.h"

namespace Cala {
	class HelperGridRenderer : public IRenderer {
	public:
		HelperGridRenderer();
		~HelperGridRenderer() = default;
		void render(GraphicsAPI* const api, const Camera& camera) override;

		glm::vec4 color{ 0.1f, 0.1f, 0.1f, 0.1f };		

	private:
		Shader shader;
		ConstantBuffer mvpBuffer;
		ConstantBuffer meshDataBuffer;
		Mesh gridMesh;
		float updateDistance = 1.f;
		uint32_t gridSize = 50;
		Transformation transformation;
	};
}