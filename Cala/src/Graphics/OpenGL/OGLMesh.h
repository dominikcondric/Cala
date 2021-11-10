#pragma once
#include <glad/glad.h>
#include "../Mesh.h"

class OGLMesh : public Mesh {
public:
	OGLMesh() = default;
	~OGLMesh() override;
	OGLMesh(const OGLMesh& other) = delete;
	OGLMesh(OGLMesh&& other) noexcept;
	OGLMesh& operator=(OGLMesh&& other) noexcept;
	OGLMesh& operator=(const OGLMesh& other) = delete;
	void setVertexBufferData(const float* data, uint32_t arraySize, uint32_t vertexCount, const std::vector<VertexLayoutSpecification>& layouts, bool isDynamic) override;
	void setVertexBufferData(const std::vector<float>& data, uint32_t vertexCount, const std::vector<VertexLayoutSpecification>& layouts, bool isDynamic = false) override;
	void updateVertexBufferData(const float* data, uint32_t arraySize, uint32_t arrayOffset) override;
	void updateVertexBufferData(const std::vector<float>& data, uint32_t arrayOffset) override;
	void setIndexBufferData(const uint32_t* data, uint32_t arraySize, bool isDynamic = false) override;
	void setIndexBufferData(const std::vector<uint32_t>& data, bool isDynamic = false) override;
	void updateIndexBufferData(const uint32_t* data, uint32_t arraySize, uint32_t arrayOffset) override;
	void updateIndexBufferData(const std::vector<uint32_t>& data, uint32_t arrayOffset) override;
	void setForRendering() const override;
	void setDrawingMode(DrawingMode mode) override;

private:
	GLuint vao{ GL_NONE };
	GLuint vbo{ GL_NONE };
	GLuint ebo{ GL_NONE };
};

