#pragma once 
#include "IRenderer.h"
#include <stack>
#include "Cala/Utility/Transformation.h"

namespace Cala {
	class SimpleRenderer : public IRenderer {
	public:
		SimpleRenderer();
		~SimpleRenderer() override = default;
		void render(GraphicsAPI* const, const Camera& camera) override;

		struct Renderable {
			Renderable(const Mesh& _mesh, const Transformation& _transformation, const glm::vec4 _color) :
				mesh(_mesh), transformation(_transformation), color(_color) {}

			const Mesh& mesh;
			const Transformation& transformation;
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