#pragma once 
#include "../GraphicsAPI.h"
#include "../Shader.h"
#include "../Camera.h"

class IRenderer {
public:
	IRenderer() = default;
	virtual ~IRenderer() = default;
	virtual void render(const GraphicsAPI* api, const Camera& camera) = 0;
};