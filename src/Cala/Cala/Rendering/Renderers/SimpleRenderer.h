#pragma once 
#include "ICameraRenderer.h"
#include <stack>
#include "Cala/Utility/Transformation.h"
#include "Cala/Rendering/Shader.h"

namespace Cala {
	class SimpleRenderer : public ICameraRenderer {
	public:
		SimpleRenderer();
        ~SimpleRenderer() override = default;
        void render(GraphicsAPI *const api, const Framebuffer* renderingTarget);
		void setupCamera(const Camera& camera) override;

		struct Renderable {
			Renderable(const Mesh& _mesh, const Transformation& _transformation, const glm::vec4 _color) :
				mesh(_mesh), transformation(_transformation), color(_color) {}

			const Mesh& mesh;
			Transformation transformation;
			glm::vec4 color;
		};

		void pushRenderable(const Renderable& renderable);

	private:
		Shader shader;
		ConstantBuffer mvpBuffer;
		ConstantBuffer materialsBuffer;
		std::stack<Renderable> renderablesStack;
	};
}