#pragma once 
#include "../GraphicsAPI.h"
#include "../Mesh.h"
#include "../Shader.h"
#include "../ConstantBuffer.h"
#include "../Camera.h"
#include "IRenderer.h"
#include "Utility/Transformation.h"

class HelperGridRenderer : public IRenderer {
public:
	HelperGridRenderer();
	~HelperGridRenderer() = default;
	void render(const GraphicsAPI* api, const Camera& camera) override;

private:
	Shader shader;
	ConstantBuffer mvpBuffer;
	Mesh gridMesh;
	float updateDistance = 1.f;
	uint32_t gridSize = 50;
	Transformation transformation;
};