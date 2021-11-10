#include <iostream>
#include "OGLGraphicsAPI.h"
#include <glm/gtx/string_cast.hpp>
#include "OGLShader.h"
#include "OGLMesh.h"
#include "OGLTexture.h"
#include "OGLConstantBuffer.h"
#include "OGLFramebuffer.h"
#include <GLFW/glfw3.h>

bool GraphicsAPI::initialized = false;

void OGLGraphicsAPI::setBufferClearingColor(const glm::vec4& color) const
{
	glClearColor(color.x, color.y, color.z, color.w);
}

void OGLGraphicsAPI::setBufferClearingBits(bool color, bool depth, bool stencil)
{
	bufferClearingBitmask = 0;
	if (color) bufferClearingBitmask |= GL_COLOR_BUFFER_BIT;
	if (depth) bufferClearingBitmask |= GL_DEPTH_BUFFER_BIT;
	if (stencil) bufferClearingBitmask |= GL_STENCIL_BUFFER_BIT;
}

void OGLGraphicsAPI::setViewport(const glm::ivec4& viewport)
{
	glViewport(viewport.x, viewport.y, viewport.z, viewport.w);
}

glm::ivec4 OGLGraphicsAPI::getCurrentViewport() const
{
	glm::ivec4 viewport;
	glGetIntegerv(GL_VIEWPORT, &viewport.x);
	return viewport;
}

void OGLGraphicsAPI::setRenderingPointSize(float size) const
{
	glPointSize(size);
}

void OGLGraphicsAPI::activateDefaultFramebuffer() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void OGLGraphicsAPI::readFromBoundFramebuffer(const glm::uvec2& pixelCoordinats, const glm::uvec2& areaSize, DataType type, void* dataToWrite) const
{
	GLenum dataType;
	switch (type)
	{
		case GraphicsAPI::DataType::Depth:
			dataType = GL_DEPTH_COMPONENT;
			break;
		case GraphicsAPI::DataType::Stencil:
			dataType = GL_STENCIL_COMPONENTS;
			break;
		case GraphicsAPI::DataType::Color:
			dataType = GL_COLOR_COMPONENTS;
			break;
	}

	glReadPixels(pixelCoordinats.x, pixelCoordinats.y, areaSize.x, areaSize.y, GL_FLOAT, dataType, dataToWrite);
}

void OGLGraphicsAPI::enableSetting(Constant setting) const
{
	glEnable(mapConstant(setting));
}

void OGLGraphicsAPI::disableSetting(Constant setting) const
{
	glDisable(mapConstant(setting));
}

bool OGLGraphicsAPI::getSettingState(Constant setting) const
{
	GLboolean state;
	glGetBooleanv(mapConstant(setting), &state);
	return (bool)state;
}

void OGLGraphicsAPI::setDepthComparisonFunction(Constant function) const
{
	glDepthFunc(mapConstant(function));
}

void OGLGraphicsAPI::setStencilComparisonFunction(Constant function, int compareValue, int mask) const
{
	glStencilFunc(mapConstant(function), compareValue, mask);
}

void OGLGraphicsAPI::setStencilOperation(Constant stencilFailOp, Constant depthFailOp, Constant pass) const
{
	glStencilOp(mapConstant(stencilFailOp), mapConstant(depthFailOp), mapConstant(pass));
}

void OGLGraphicsAPI::setStencilMask(int value) const
{
	glStencilMask(value);
}

void OGLGraphicsAPI::setPolygonFillingMode(Constant side, Constant mode) const
{
	glPolygonMode(mapConstant(side), mapConstant(mode));
}

void OGLGraphicsAPI::clearFramebuffer() const
{
	glClear(bufferClearingBitmask);
}

OGLGraphicsAPI* OGLGraphicsAPI::construct()
{
	if (!initialized) 
		return new OGLGraphicsAPI();

	return nullptr;
}

void OGLGraphicsAPI::render(const Mesh* mesh) const
{
	mesh->setForRendering();
	if (mesh->getIndexCount() == 0)
		draw(mesh->getDrawingMode(), mesh->getVertexCount());
	else
		drawIndexed(mesh->getDrawingMode(), mesh->getIndexCount());
}

void OGLGraphicsAPI::renderInstances(const Mesh* mesh, uint32_t drawCount) const
{
	mesh->setForRendering();
	if (mesh->getIndexCount() == 0)
		drawInstanced(mesh->getDrawingMode(), mesh->getVertexCount(), drawCount);
	else
		drawIndexedInstanced(mesh->getDrawingMode(), mesh->getIndexCount(), drawCount);
}

OGLGraphicsAPI::~OGLGraphicsAPI()
{
	initialized = false;
}

Shader* OGLGraphicsAPI::createShader() const
{
	return new OGLShader();
}

Mesh* OGLGraphicsAPI::createMesh() const
{
	return new OGLMesh();
}

ConstantBuffer* OGLGraphicsAPI::createConstantBuffer() const
{
	return new OGLConstantBuffer();
}

Texture* OGLGraphicsAPI::createTexture() const
{
	return new OGLTexture();
}

Framebuffer* OGLGraphicsAPI::createFramebuffer() const
{
	return new OGLFramebuffer();
}

uint32_t OGLGraphicsAPI::mapConstant(Constant constant) const
{
	switch (constant)
	{
		case IncrementWrap:			return GL_INCR_WRAP;
		case DecrementWrap:			return GL_DECR_WRAP;
		case Increment:				return GL_INCR;
		case Decrement:				return GL_DECR;
		case Keep:					return GL_KEEP;
		case Invert:				return GL_INVERT;
		case Replace:				return GL_REPLACE;
		case Zero:					return GL_ZERO;
		case Always:				return GL_ALWAYS;
		case Never:					return GL_NEVER;
		case Greater:				return GL_GREATER;
		case Equal:					return GL_EQUAL;
		case NotEqual:				return GL_NOTEQUAL;
		case LessOrEqual:			return GL_LEQUAL;
		case GreaterOrEqual:		return GL_GEQUAL;
		case GammaCorrection:		return GL_FRAMEBUFFER_SRGB;
		case Multisampling:			return GL_MULTISAMPLE;
		case Blending:				return GL_BLEND;
		case StencilTesting:		return GL_STENCIL_TEST;
		case DepthTesting:			return GL_DEPTH_TEST;
		case FaceCulling:			return GL_CULL_FACE;
		case Back:					return GL_BACK;
		case Front:					return GL_FRONT;
		case FrontAndBack:			return GL_FRONT_AND_BACK;
		case Lines:					return GL_LINE;
		case Points:				return GL_POINT;
		case Fill:					return GL_FILL;
		default:					return GL_NONE;
	}
}

void OGLGraphicsAPI::draw(const GLuint drawingMode, const GLuint verticesCount) const
{
	glDrawArrays(drawingMode, 0, verticesCount);
}

void OGLGraphicsAPI::drawIndexed(const GLuint drawingMode, const GLuint indicesCount) const
{
	glDrawElements(drawingMode, indicesCount, GL_UNSIGNED_INT, nullptr);
}

void OGLGraphicsAPI::drawInstanced(const GLuint drawingMode, const GLuint verticesCount, const GLuint instancesCount) const
{
	glDrawArraysInstanced(drawingMode, 0, verticesCount, instancesCount);
}

void OGLGraphicsAPI::drawIndexedInstanced(const GLuint drawingMode, const GLuint indicesCount, const GLuint instancesCount) const
{
	glDrawElementsInstanced(drawingMode, indicesCount, GL_UNSIGNED_INT, nullptr, instancesCount);
}