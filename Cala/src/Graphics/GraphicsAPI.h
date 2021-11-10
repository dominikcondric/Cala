#pragma once
#include <memory>
#include "Shader.h"
#include "Mesh.h"
#include "Framebuffer.h"
#include "Texture.h"
#include "ConstantBuffer.h"

class GraphicsAPI {
public:
	enum class API {
		OpenGL, Direct3D, Vulkan
	};
	
	enum class DataType {
		Depth, Stencil, Color
	};

public:
	virtual ~GraphicsAPI() = default;
	virtual Shader* createShader() const = 0;
	virtual Mesh* createMesh() const = 0;
	virtual ConstantBuffer* createConstantBuffer() const = 0;
	virtual Texture* createTexture() const = 0;
	virtual Framebuffer* createFramebuffer() const = 0;
	virtual void render(const Mesh* mesh) const = 0;
	virtual void renderInstances(const Mesh* mesh, uint32_t drawCount) const = 0;
	virtual void setBufferClearingColor(const glm::vec4& color) const = 0;
	virtual void setBufferClearingBits(bool color, bool depth, bool stencil) = 0;
	virtual void setViewport(const glm::ivec4& viewport) = 0;
	virtual glm::ivec4 getCurrentViewport() const = 0;
	virtual void setRenderingPointSize(float size) const = 0;
	virtual void clearFramebuffer() const = 0;
	virtual void activateDefaultFramebuffer() const = 0;
	virtual void readFromBoundFramebuffer(const glm::uvec2& pixelCoordinates, const glm::uvec2& areaSize, DataType type, void* dataToWrite) const = 0; 

	enum Constant {
		// GraphicsAPI settings
		Blending, Multisampling, FaceCulling, 
		GammaCorrection, StencilTesting, DepthTesting,

		// Comparison functions
		Greater, Less, GreaterOrEqual,
		LessOrEqual, Equal, NotEqual,
		Always, Never,

		// Stencil operations
		Keep, Zero, Replace, Increment,
		IncrementWrap, Decrement, DecrementWrap,
		Invert,

		// Polygon fill modes
		Fill, Lines, Points,

		// Sides
		Front, Back, FrontAndBack,
	};

	virtual void enableSetting(Constant setting) const = 0;
	virtual void disableSetting(Constant setting) const = 0;
	virtual bool getSettingState(Constant setting) const = 0;
	virtual void setDepthComparisonFunction(Constant function) const = 0;
	virtual void setStencilComparisonFunction(Constant function, int compareValue, int mask) const = 0;
	virtual void setStencilOperation(Constant stencilFailOp, Constant depthFailOp, Constant pass) const = 0;
	virtual void setStencilMask(int value) const = 0;
	virtual void setPolygonFillingMode(Constant side, Constant mode) const = 0;

protected:
	GraphicsAPI() = default;
	GraphicsAPI(const GraphicsAPI& other) = delete;
	virtual void draw(const uint32_t drawingMode, const uint32_t vertexCount) const = 0;
	virtual void drawIndexed(const uint32_t drawingMode, const uint32_t indicesCount) const = 0;
	virtual void drawInstanced(const uint32_t drawingMode, const uint32_t verticesCount, const uint32_t instancesCount) const = 0;
	virtual void drawIndexedInstanced(const uint32_t drawingMode, const uint32_t indicesCount, const uint32_t instancesCount) const = 0;
	virtual uint32_t mapConstant(Constant constant) const = 0;
	uint32_t bufferClearingBitmask = 0;
	static bool initialized;
};
