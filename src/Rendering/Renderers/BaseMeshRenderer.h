#pragma once 
#include "../GraphicsAPI.h"
#include "../Mesh.h"
#include "../Texture.h"
#include "../Shader.h"
#include "../ConstantBuffer.h"
#include "../Camera.h"

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