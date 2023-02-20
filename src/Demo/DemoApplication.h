#pragma once
#include "Utility/BaseApplication.h"

using namespace Cala;

class DemoApplication : public BaseApplication {
public:
	DemoApplication();
	~DemoApplication() = default;

private:
	void loop() override;

	Mesh sphereMesh;
	SimpleRenderer simpleRenderer;
	LightRenderer lightRenderer;
	std::vector<Texture> diffuseMaps, normalMaps;
	std::vector<LightRenderer::Renderable> renderables;
	LightRenderer::Light light;
	SimpleRenderer::Renderable lightRenderable;
};
