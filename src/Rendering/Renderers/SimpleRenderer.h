#pragma once 
#include "IRenderer.h"
#include <stack>
#include "Utility/Transformation.h"

class SimpleRenderer : public IRenderer {
public:
	SimpleRenderer();
	~SimpleRenderer() override = default;
	void render(const GraphicsAPI* api, const Camera& camera) override;

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
	std::stack<const Renderable*> renderablesStack;
};