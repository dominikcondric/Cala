#include <iostream>
#include "GraphicsAPI.h"
#include <glm/gtx/string_cast.hpp>
#include "Shader.h"
#include "Mesh.h"
#include "Texture.h"
#include "ConstantBuffer.h"
#include "Framebuffer.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Cala/Utility/Logger.h"

namespace Cala {
	GraphicsAPI* GraphicsAPI::instance = nullptr;

#ifdef CALA_API_OPENGL
	GraphicsAPI* GraphicsAPI::construct()
	{
		if (!instance)
			instance = new GraphicsAPI;

		return instance;
	}

	GraphicsAPI::~GraphicsAPI()
	{
		instance = nullptr;
	}

    void GraphicsAPI::_checkForErrors(const std::string& file, int line)
    {
		GLenum errorCode = GL_NO_ERROR;
		while ((errorCode = glGetError()) != GL_NO_ERROR)
		{
			const char* error = nullptr;
			switch (errorCode)
			{
				case GL_INVALID_ENUM: 
					error = "INVALID ENUM";
					break;
				case GL_INVALID_VALUE:
					error = "INVALID VALUE";
					break;
				case GL_INVALID_OPERATION:
					error = "INVALID OPERATION";
					break;
				case GL_OUT_OF_MEMORY:
					error = "OUT OF MEMORY";
					break;
				case GL_INVALID_FRAMEBUFFER_OPERATION:
					error = "INVALID FRAMEBUFFER OPERATION";
					break;
			}

			std::string errorMessage(error);
			errorMessage += " in: " + file +  " (line ";
			errorMessage += std::to_string(line) + ")";
			Logger::getInstance().logErrorToConsole(errorMessage);
		}
    }

    void GraphicsAPI::setBufferClearingColor(const glm::vec4& color) const
	{
		glClearColor(color.x, color.y, color.z, color.w);
	}

	void GraphicsAPI::setBufferClearingBits(bool color, bool depth, bool stencil)
	{
		bufferClearingBitmask = 0;
		
		if (color)
			bufferClearingBitmask |= GL_COLOR_BUFFER_BIT;

		if (depth)
			bufferClearingBitmask |= GL_DEPTH_BUFFER_BIT;

		if (stencil)
			bufferClearingBitmask |= GL_STENCIL_BUFFER_BIT;
	}

	void GraphicsAPI::setViewport(const glm::ivec4& viewport)
	{
		glViewport(viewport.x, viewport.y, viewport.z, viewport.w);
	}

	glm::ivec4 GraphicsAPI::getCurrentViewport() const
	{
		glm::ivec4 viewport;
		glGetIntegerv(GL_VIEWPORT, &viewport.x);
		return viewport;
	}

	void GraphicsAPI::setRenderingPointSize(float size) const
	{
		glPointSize(size);
	}

    void GraphicsAPI::activateDefaultFramebuffer()
    {
		glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE);
		activeFramebuffer = nullptr;
    }

    void GraphicsAPI::activateFramebuffer(const Framebuffer& framebuffer)
    {
		GLuint framebufferID = framebuffer.getNativeFramebufferHandle();
		glBindFramebuffer(GL_FRAMEBUFFER, framebufferID);
		activeFramebuffer = &framebuffer;
    }

    void GraphicsAPI::enableSetting(Constant setting) const
	{
		glEnable(mapConstant(setting));
	}

	void GraphicsAPI::disableSetting(Constant setting) const
	{
		glDisable(mapConstant(setting));
	}

	bool GraphicsAPI::getSettingState(Constant setting) const
	{
		GLboolean state;
		glGetBooleanv(mapConstant(setting), &state);
		return (bool)state;
	}

	void GraphicsAPI::setDepthComparisonFunction(Constant function) const
	{
		glDepthFunc(mapConstant(function));
	}

	void GraphicsAPI::setStencilComparisonFunction(Constant function, int compareValue, int mask) const
	{
		glStencilFunc(mapConstant(function), compareValue, mask);
	}

	void GraphicsAPI::setStencilOperation(Constant stencilFailOp, Constant depthFailOp, Constant pass) const
	{
		glStencilOp(mapConstant(stencilFailOp), mapConstant(depthFailOp), mapConstant(pass));
	}

	void GraphicsAPI::setStencilMask(int value) const
	{
		glStencilMask(value);
	}

	void GraphicsAPI::setPolygonFillingMode(Constant side, Constant mode) const
	{
		glPolygonMode(mapConstant(side), mapConstant(mode));
	}

	void GraphicsAPI::setLineWidth(float lineWidth) const
	{
		glLineWidth(lineWidth);
	}

	void GraphicsAPI::clearFramebuffer() const
	{
		glClear(bufferClearingBitmask);
	}

	void GraphicsAPI::render(const Mesh& mesh) const
	{
		if (mesh.culled)
			enableSetting(FaceCulling);
		else 
			disableSetting(FaceCulling);

		mesh.setForRendering();
		if (mesh.getIndexCount() == 0)
			draw(mesh.getDrawingMode(), mesh.getVertexCount());
		else
			drawIndexed(mesh.getDrawingMode(), mesh.getIndexCount());
	}

	void GraphicsAPI::renderInstances(const Mesh& mesh, uint32_t drawCount) const
	{
		mesh.setForRendering();
		if (mesh.getIndexCount() == 0)
			drawInstanced(mesh.getDrawingMode(), mesh.getVertexCount(), drawCount);
		else
			drawIndexedInstanced(mesh.getDrawingMode(), mesh.getIndexCount(), drawCount);
	}

	uint32_t GraphicsAPI::mapConstant(Constant constant) const
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

	void GraphicsAPI::draw(const GLuint drawingMode, const GLuint verticesCount) const
	{
		glDrawArrays(drawingMode, 0, verticesCount);
	}

	void GraphicsAPI::drawIndexed(const GLuint drawingMode, const GLuint indicesCount) const
	{
		glDrawElements(drawingMode, indicesCount, GL_UNSIGNED_INT, nullptr);
	}

	void GraphicsAPI::drawInstanced(const GLuint drawingMode, const GLuint verticesCount, const GLuint instancesCount) const
	{
		glDrawArraysInstanced(drawingMode, 0, verticesCount, instancesCount);
	}

	void GraphicsAPI::drawIndexedInstanced(const GLuint drawingMode, const GLuint indicesCount, const GLuint instancesCount) const
	{
		glDrawElementsInstanced(drawingMode, indicesCount, GL_UNSIGNED_INT, nullptr, instancesCount);
	}
	#else 
		#error Api not supported yet!
	#endif
}