#include "GraphicsAPI.h"
#include "OpenGL/OGLGraphicsAPI.h"
#include "OpenGL/OGLTexture.h"
#include "OpenGL/OGLMesh.h"
#include "OpenGL/OGLFramebuffer.h"
#include "OpenGL/OGLConstantBuffer.h"
#include "OpenGL/OGLShader.h"

bool GraphicsAPI::initialized = false;
GraphicsAPI::API GraphicsAPI::api = GraphicsAPI::API::OpenGL;

GraphicsAPI* GraphicsAPI::construct(API api)
{
	switch (api)
	{
		case GraphicsAPI::API::OpenGL:
			return OGLGraphicsAPI::construct();
		case GraphicsAPI::API::Direct3D:
			break;
		case GraphicsAPI::API::Vulkan:
			break;
		default:
			break;
	}
}

Texture* GraphicsAPI::createTexture()
{
	switch (api)
	{
		case GraphicsAPI::API::OpenGL:
			return new OGLTexture();
		case GraphicsAPI::API::Direct3D:
			break;
		case GraphicsAPI::API::Vulkan:
			break;
		default:
			break;
	}
}

Framebuffer* GraphicsAPI::createFramebuffer()
{
	switch (api)
	{
		case GraphicsAPI::API::OpenGL:
			return new OGLFramebuffer();
		case GraphicsAPI::API::Direct3D:
			break;
		case GraphicsAPI::API::Vulkan:
			break;
		default:
			break;
	}
}

Shader* GraphicsAPI::createShader()
{
	switch (api)
	{
		case GraphicsAPI::API::OpenGL:
			return new OGLShader();
		case GraphicsAPI::API::Direct3D:
			break;
		case GraphicsAPI::API::Vulkan:
			break;
		default:
			break;
	}
}

ConstantBuffer* GraphicsAPI::createConstantBuffer()
{
	switch (api)
	{
		case GraphicsAPI::API::OpenGL:
			return new OGLConstantBuffer();
		case GraphicsAPI::API::Direct3D:
			break;
		case GraphicsAPI::API::Vulkan:
			break;
		default:
			break;
	}
}

Mesh* GraphicsAPI::createMesh()
{
	switch (api)
	{
		case GraphicsAPI::API::OpenGL:
			return new OGLMesh();
		case GraphicsAPI::API::Direct3D:
			break;
		case GraphicsAPI::API::Vulkan:
			break;
		default:
			break;
	}
}
