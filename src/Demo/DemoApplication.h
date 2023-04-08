#pragma once
#include "Cala/Utility/BaseApplication.h"
#include "Cala/Rendering/Renderers/LightRenderer.h"
#include "Cala/Rendering/Renderers/SimpleRenderer.h"

using namespace Cala;

class DemoApplication : public BaseApplication {
public:
	DemoApplication();
	~DemoApplication() = default;

private:
	void loop() override;

	Mesh sphereMesh, cubeMesh, planeMesh;
	Transformation planeTransform;
	std::array<Transformation, 4> wallTransforms;
	SimpleRenderer simpleRenderer;
	LightRenderer lightRenderer;
	std::vector<LightRenderer::Renderable> cubeRenderables;
	std::vector<Transformation> transformations;
	Transformation lightTransformation;
};
