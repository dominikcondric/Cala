#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <string>
#include "Cala/Utility/Model.h"
#include "NativeAPI.h"

namespace Cala {
	class Mesh {
	public:
		Mesh(const Model& model, bool dynamic = false, bool _cullingEnabled = true);
		Mesh() = default;
		~Mesh();
		Mesh(const Mesh& other) = delete;
		Mesh(Mesh&& other) noexcept;
		Mesh& operator=(const Mesh& other) = delete;
		Mesh& operator=(Mesh&& other) noexcept;
		void free();
		void loadFromModel(const Model& model, bool dynamic = false, bool _cullingEnabled = true);
		void setIndexBufferData(const uint32_t* data, uint32_t arraySize, bool isDynamic = false);
		void setIndexBufferData(const std::vector<uint32_t>& data, bool isDynamic = false);
		void setVertexBufferData(const float* data, uint32_t arraySize, uint32_t _vertexCount, const std::vector<Model::VertexLayoutSpecification>& layouts, bool isDynamic);
		void setVertexBufferData(const std::vector<float>& data, uint32_t _vertexCount, const std::vector<Model::VertexLayoutSpecification>& layouts, bool isDynamic = false);
		void setDrawingMode(Model::DrawingMode mode);
		void updateIndexBufferData(const uint32_t* data, uint32_t arraySize, uint32_t arrayOffset);
		void updateIndexBufferData(const std::vector<uint32_t>& data, uint32_t arrayOffset);
		void updateVertexBufferData(const float* data, uint32_t arraySize, uint32_t arrayOffset);
		void updateVertexBufferData(const std::vector<float>& data, uint32_t arrayOffset);
		void setForRendering() const;
		uint32_t getVertexCount() const { return vertexCount; }
		uint32_t getIndexCount() const { return indexCount; }
		uint32_t getDrawingMode() const { return drawingMode; }

		bool cullingEnabled = false;

	private:
		uint32_t vertexCount{ 0 };
		uint32_t indexCount{ 0 };
	#ifdef CALA_API_OPENGL
		GLenum drawingMode;
		GLuint vbo = API_NULL;
		GLuint ebo = API_NULL;
		GLuint vao = API_NULL;
	#else
		#error "Api not supported yet!"
	#endif
	};
}

