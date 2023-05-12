#pragma once
#include "Cala/Utility/BaseApplication.h"
#include "Cala/Rendering/Renderers/LightRenderer.h"
#include "Cala/Rendering/Renderers/SimpleRenderer.h"
#include "Cala/Rendering/Renderers/PostProcessingRenderer.h"

using namespace Cala;

class DemoApplication : public BaseApplication {
public:
	DemoApplication();
	~DemoApplication() = default;

private:
	void loop() override;

	Mesh sphereMesh, cubeMesh;
	std::array<Transformation, 5> wallTransforms;
	SimpleRenderer simpleRenderer;
	LightRenderer lightRenderer;
	std::vector<LightRenderer::Renderable> cubeRenderables;
	std::vector<Transformation> transformations;
	Transformation lightTransformation;
};
