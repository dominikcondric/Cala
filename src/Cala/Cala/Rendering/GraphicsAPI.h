#pragma once
#include <memory>
#include "Mesh.h"
#include "Framebuffer.h"

#define checkForErrors() GraphicsAPI::_checkForErrors(__FILE__, __LINE__)

namespace Cala {
	class GraphicsAPI {
	public:
		enum class DataType {
			Depth, Stencil, Color
		};

	public:
		static GraphicsAPI* construct();
		~GraphicsAPI();
		static void _checkForErrors(const std::string& file, int line);
		void render(const Mesh& mesh) const;
		void renderInstances(const Mesh& mesh, uint32_t drawCount) const;
		void setBufferClearingColor(const glm::vec4& color) const;
		void setBufferClearingBits(bool color, bool depth, bool stencil);
		void setViewport(const glm::ivec4& viewport);
		glm::ivec4 getCurrentViewport() const;
		void setRenderingPointSize(float size) const;
		void clearFramebuffer() const;
		void activateDefaultFramebuffer();
		void activateFramebuffer(const Framebuffer& framebuffer);
		const Framebuffer* getActiveFramebuffer() const { return activeFramebuffer; }

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

		void enableSetting(Constant setting) const;
		void disableSetting(Constant setting) const;
		bool getSettingState(Constant setting) const;
		void setDepthComparisonFunction(Constant function) const;
		void setStencilComparisonFunction(Constant function, int compareValue, int mask) const;
		void setStencilOperation(Constant stencilFailOp, Constant depthFailOp, Constant pass) const;
		void setStencilMask(int value) const;
		void setPolygonFillingMode(Constant side, Constant mode) const;
		void setLineWidth(float lineWidth) const;

	protected:
		GraphicsAPI() = default;
		GraphicsAPI(const GraphicsAPI& other) = delete;
		void draw(const uint32_t drawingMode, const uint32_t vertexCount) const;
		void drawIndexed(const uint32_t drawingMode, const uint32_t indicesCount) const;
		void drawInstanced(const uint32_t drawingMode, const uint32_t verticesCount, const uint32_t instancesCount) const;
		void drawIndexedInstanced(const uint32_t drawingMode, const uint32_t indicesCount, const uint32_t instancesCount) const;
		uint32_t mapConstant(Constant constant) const;
		uint32_t bufferClearingBitmask;
		static GraphicsAPI* instance;
		const Framebuffer* activeFramebuffer = nullptr;
	};
}
