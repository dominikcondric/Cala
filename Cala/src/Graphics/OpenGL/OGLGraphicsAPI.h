#pragma once
#include <glad/glad.h>
#include "../GraphicsAPI.h"

class OGLGraphicsAPI : public GraphicsAPI {
public:
	static OGLGraphicsAPI* construct();
	~OGLGraphicsAPI() override;
	void render(const Mesh* mesh) const override;
	void renderInstances(const Mesh* mesh, uint32_t drawCount) const override;
	void enableSetting(Constant setting) const override;
	void disableSetting(Constant setting) const override;
	bool getSettingState(Constant setting) const override;
	void setDepthComparisonFunction(Constant function) const override;
	void setStencilComparisonFunction(Constant function, int mask, int compareValue) const override;
	void setStencilOperation(Constant stencilFailOp, Constant depthFailOp, Constant pass) const override;
	void setStencilMask(int value) const override;
	void setPolygonFillingMode(Constant side, Constant mode) const override;
	void clearFramebuffer() const override;
	void setBufferClearingColor(const glm::vec4 & color) const override;
	void setBufferClearingBits(bool color, bool depth, bool stencil) override;
	void setViewport(const glm::ivec4& viewport) override;
	glm::ivec4 getCurrentViewport() const;
	void setRenderingPointSize(float size) const override;
	void activateDefaultFramebuffer() const;
	void readFromBoundFramebuffer(const glm::uvec2& pixelCoordinats, const glm::uvec2& areaSize, DataType type, void* dataToWrite) const override;

private:
	OGLGraphicsAPI() = default;
	uint32_t mapConstant(Constant constant) const override;
	void draw(const GLuint drawingMode, const GLuint verticesCount) const override;
	void drawIndexed(const GLuint drawingMode, const GLuint indicesCount) const override;
	void drawInstanced(const GLuint drawingMode, const GLuint verticesCount, const GLuint instancesCount) const override;
	void drawIndexedInstanced(const GLuint drawingMode, const GLuint indicesCount, const GLuint instancesCount) const override;

};

