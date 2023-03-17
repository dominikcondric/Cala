#pragma once 
#include "Cala/Rendering/GraphicsAPI.h"
#include "Cala/Rendering/Mesh.h"
#include "Cala/Rendering/Texture.h"
#include "Cala/Rendering/Shader.h"
#include "Cala/Rendering/ConstantBuffer.h"
#include "Cala/Rendering/Camera.h"

class Renderer {
public:
	struct RenderObject {
		Mesh mesh;
	};

public:
	Renderer(const GraphicsAPI* _api, const Shader& _shader) : api(_api), shader(_shader) {}
	virtual ~Renderer() = default;
	virtual void render(const Camera& camera) = 0;

private:
	const GraphicsAPI* api;
	const Shader& shader;
};