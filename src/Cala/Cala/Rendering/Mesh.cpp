#include "Mesh.h"
#include <cstring>
#include <glad/glad.h>

namespace Cala {
#if CALA_API == CALA_API_OPENGL
	Mesh::Mesh(const Model& model, bool dynamic)
	{
		loadMesh(model, dynamic);
	}

	Mesh::~Mesh()
	{
		glDeleteBuffers(1, &vbo);
		glDeleteBuffers(1, &ebo);
		glDeleteVertexArrays(1, &vao);
	}

	Mesh::Mesh(Mesh&& other) noexcept
	{
		*this = std::move(other);
	}

	Mesh& Mesh::operator=(Mesh&& other) noexcept
	{
		meshFile = std::move(other.meshFile);
		vao = other.vao;
		ebo = other.ebo;
		vbo = other.vbo;
		other.vao = GL_NONE;
		other.vbo = GL_NONE;
		other.ebo = GL_NONE;
		drawingMode = other.drawingMode;
		vertexCount = other.vertexCount;
		indexCount = other.indexCount;
		return *this;
	}

	void Mesh::loadMesh(const Model& model, bool dynamic)
	{
		meshFile = model.getModelPath();
		setVertexBufferData(model.getVertexData(), model.getVertexCount(), model.getLayoutSpecification(), dynamic);
		if (model.getIndexCount() != 0)
			setIndexBufferData(model.getIndexData(), dynamic);

		setDrawingMode(model.getDrawingMode());
	}

	void Mesh::setVertexBufferData(const float* data, uint32_t arraySize, uint32_t _vertexCount, const std::vector<Model::VertexLayoutSpecification>& layouts, bool isDynamic)
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

	void Mesh::setVertexBufferData(const std::vector<float>& data, uint32_t _vertexCount, const std::vector<Model::VertexLayoutSpecification>& layouts, bool isDynamic)
	{
		setVertexBufferData(data.data(), (uint32_t)data.size(), _vertexCount, layouts, isDynamic);
	}

	void Mesh::updateVertexBufferData(const float* data, uint32_t arraySize, uint32_t arrayOffset)
	{
		if (vao == GL_NONE)
			return;

		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferSubData(GL_ARRAY_BUFFER, arrayOffset * sizeof(float), arraySize * sizeof(float), data);
		glBindVertexArray(GL_NONE);
		glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);
	}

	void Mesh::updateVertexBufferData(const std::vector<float>& data, uint32_t arrayOffset)
	{
		updateVertexBufferData(data.data(), (uint32_t)data.size(), arrayOffset);
	}

	void Mesh::setIndexBufferData(const uint32_t* data, uint32_t arraySize, bool isDynamic)
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

	void Mesh::setIndexBufferData(const std::vector<uint32_t>& data, bool isDynamic)
	{
		setIndexBufferData(data.data(), (uint32_t)data.size(), isDynamic);
	}

	void Mesh::updateIndexBufferData(const uint32_t* data, uint32_t arraySize, uint32_t arrayOffset)
	{
		if (ebo == GL_NONE)
			return;

		glBindVertexArray(vao);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, arrayOffset * sizeof(GLuint), arraySize * sizeof(GLuint), data);
	}

	void Mesh::updateIndexBufferData(const std::vector<uint32_t>& data, uint32_t arrayOffset)
	{
		updateIndexBufferData(data.data(), (uint32_t)data.size(), arrayOffset);
	}

	void Mesh::setForRendering() const
	{
		glBindVertexArray(vao);
	}

	void Mesh::setDrawingMode(Model::DrawingMode mode)
	{
		switch (mode)
		{
			case Model::DrawingMode::Triangles:
				drawingMode = GL_TRIANGLES;
				break;
			case Model::DrawingMode::TriangleFan:
				drawingMode = GL_TRIANGLE_FAN;
				break;
			case Model::DrawingMode::TriangleStrip:
				drawingMode = GL_TRIANGLE_STRIP;
				break;
			case Model::DrawingMode::Lines:
				drawingMode = GL_LINES;
				break;
			case Model::DrawingMode::LineLoop:
				drawingMode = GL_LINE_LOOP;
				break;
			case Model::DrawingMode::LineStrip:
				drawingMode = GL_LINE_STRIP;
				break;
			default:
				break;
		}
	}
#else
	#error Api not supported yet!
#endif
}