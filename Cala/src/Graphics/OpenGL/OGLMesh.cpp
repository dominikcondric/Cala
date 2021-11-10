#include "OGLMesh.h"
#include <cstring>

OGLMesh::~OGLMesh()
{
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ebo);
	glDeleteVertexArrays(1, &vao);
}

OGLMesh::OGLMesh(OGLMesh&& other) noexcept
{
	std::memcpy(this, &other, sizeof(OGLMesh));
	other.vao = GL_NONE;
	other.vbo = GL_NONE;
	other.ebo = GL_NONE;
}

OGLMesh& OGLMesh::operator=(OGLMesh&& other) noexcept
{
	std::memcpy(this, &other, sizeof(OGLMesh));
	other.vao = GL_NONE;
	other.vbo = GL_NONE;
	other.ebo = GL_NONE;
	return *this;
}

void OGLMesh::setVertexBufferData(const float* data, uint32_t arraySize, uint32_t _vertexCount, const std::vector<VertexLayoutSpecification>& layouts, bool isDynamic)
{
	if (vbo != GL_NONE)
		glDeleteBuffers(1, &vbo);
	else
		glGenVertexArrays(1, &vao);

	this->vertexCount = _vertexCount;
	glGenBuffers(1, &vbo);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, arraySize * sizeof(float), data, isDynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);

	for (const auto& layout : layouts)
	{
		glVertexAttribPointer(layout.index, layout.componentCount, GL_FLOAT, GL_FALSE, layout.strideInBytes, (void*)layout.offsetInBytes);
		glEnableVertexAttribArray(layout.index);
	}

	glBindVertexArray(GL_NONE);
	glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);
}

void OGLMesh::setVertexBufferData(const std::vector<float>& data, uint32_t _vertexCount, const std::vector<VertexLayoutSpecification>& layouts, bool isDynamic)
{
	setVertexBufferData(data.data(), (uint32_t)data.size(), _vertexCount, layouts, isDynamic);
}

void OGLMesh::updateVertexBufferData(const float* data, uint32_t arraySize, uint32_t arrayOffset)
{
	if (vao == GL_NONE)
		return;

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferSubData(GL_ARRAY_BUFFER, arrayOffset * sizeof(float), arraySize * sizeof(float), data);
	glBindVertexArray(GL_NONE);
	glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);
}

void OGLMesh::updateVertexBufferData(const std::vector<float>& data, uint32_t arrayOffset)
{
	updateVertexBufferData(data.data(), (uint32_t)data.size(), arrayOffset);
}

void OGLMesh::setIndexBufferData(const uint32_t* data, uint32_t arraySize, bool isDynamic)
{
	if (ebo != GL_NONE)
		glDeleteBuffers(1, &ebo);

	indexCount = arraySize;
	glGenBuffers(1, &ebo);
	glBindVertexArray(vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, arraySize * sizeof(GLuint), data, isDynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
	glBindVertexArray(GL_NONE);
}

void OGLMesh::setIndexBufferData(const std::vector<uint32_t>& data, bool isDynamic)
{
	setIndexBufferData(data.data(), (uint32_t)data.size(), isDynamic);
}

void OGLMesh::updateIndexBufferData(const uint32_t* data, uint32_t arraySize, uint32_t arrayOffset)
{
	if (ebo == GL_NONE)
		return;

	glBindVertexArray(vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, arrayOffset * sizeof(GLuint), arraySize * sizeof(GLuint), data);
}

void OGLMesh::updateIndexBufferData(const std::vector<uint32_t>& data, uint32_t arrayOffset)
{
	updateIndexBufferData(data.data(), (uint32_t)data.size(), arrayOffset);
}

void OGLMesh::setForRendering() const
{
	glBindVertexArray(vao);
}

void OGLMesh::setDrawingMode(DrawingMode mode)
{
	switch (mode)
	{
	case Mesh::DrawingMode::Triangles:
		drawingMode = GL_TRIANGLES;
		break;
	case Mesh::DrawingMode::TriangleFan:
		drawingMode = GL_TRIANGLE_FAN;
		break;
	case Mesh::DrawingMode::TriangleStrip:
		drawingMode = GL_TRIANGLE_STRIP;
		break;
	case Mesh::DrawingMode::Lines:
		drawingMode = GL_LINES;
		break;
	case Mesh::DrawingMode::LineLoop:
		drawingMode = GL_LINE_LOOP;
		break;
	case Mesh::DrawingMode::LineStrip:
		drawingMode = GL_LINE_STRIP;
		break;
	default:
		break;
	}
}
