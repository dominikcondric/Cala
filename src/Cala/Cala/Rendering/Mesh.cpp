#include "Mesh.h"
#include <cstring>
#include <glad/glad.h>
#include "NativeAPI.h"
#include "Cala/Utility/Logger.h"

namespace Cala {
#ifdef CALA_API_OPENGL
	Mesh::Mesh(const Model& model, bool dynamic, bool _cullingEnabled)
	{
		loadFromModel(model, dynamic, _cullingEnabled);
	}

	Mesh::~Mesh()
	{
		free();
	}

	Mesh::Mesh(Mesh&& other) noexcept
	{
		*this = std::move(other);
	}

	Mesh& Mesh::operator=(Mesh&& other) noexcept
	{
		vao = other.vao;
		ebo = other.ebo;
		vbo = other.vbo;
		other.vao = API_NULL;
		other.vbo = API_NULL;
		other.ebo = API_NULL;
		drawingMode = other.drawingMode;
		vertexCount = other.vertexCount;
		indexCount = other.indexCount;
		return *this;
	}

    bool Mesh::isLoaded() const
    {
		return vbo != API_NULL;
    }

    void Mesh::free()
    {
		glDeleteBuffers(1, &vbo);
		glDeleteBuffers(1, &ebo);
		glDeleteVertexArrays(1, &vao);

		vbo = API_NULL;
		ebo = API_NULL;
		vao = API_NULL;
    }

    void Mesh::loadFromModel(const Model& model, bool dynamic, bool _cullingEnabled)
	{
		setVertexBufferData(model.getGPUVertexData(), model.getPositions().size(), model.getLayoutSpecification(), dynamic);
		if (model.getIndices().size() != 0)
			setIndexBufferData(model.getIndices(), dynamic);

		setDrawingMode(model.getDrawingMode());
		cullingEnabled = _cullingEnabled;
	}

	void Mesh::setVertexBufferData(const float* data, uint32_t arraySize, uint32_t _vertexCount, const std::vector<Model::VertexLayoutSpecification>& layouts, bool isDynamic)
	{
		if (isLoaded())
		{
			Logger::getInstance().logErrorToConsole("Buffer already loaded!");
			return;
		}

		glGenVertexArrays(1, &vao);
		this->vertexCount = _vertexCount;
		glGenBuffers(1, &vbo);
		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, arraySize * sizeof(float), data, isDynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);

		for (const auto& layout : layouts)
		{
			glVertexAttribPointer(layout.index, layout.componentCount, GL_FLOAT, GL_FALSE, layout.strideInBytes, (void*)(uintptr_t)layout.offsetInBytes);
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
		if (!isLoaded())
		{
			Logger::getInstance().logErrorToConsole("Buffer not loaded yet!");
			return;
		}

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
		{
			Logger::getInstance().logErrorToConsole("Buffer already loaded!");
			return;
		}

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
		{
			Logger::getInstance().logErrorToConsole("Buffer not loaded yet!");
			return;
		}

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