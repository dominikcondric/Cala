#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <string>
#include "Cala/Utility/Model.h"

typedef uint32_t GLuint;

namespace Cala {
	class Mesh {
	public:
		Mesh(const Model& model, bool dynamic = false);
		Mesh() = default;
		~Mesh();
		Mesh(const Mesh& other) = delete;
		Mesh(Mesh&& other) noexcept;
		Mesh& operator=(const Mesh& other) = delete;
		Mesh& operator=(Mesh&& other) noexcept;
		void loadMesh(const Model& model, bool dynamic = false);
		void updateIndexBufferData(const uint32_t* data, uint32_t arraySize, uint32_t arrayOffset);
		void updateIndexBufferData(const std::vector<uint32_t>& data, uint32_t arrayOffset);
		void updateVertexBufferData(const float* data, uint32_t arraySize, uint32_t arrayOffset);
		void updateVertexBufferData(const std::vector<float>& data, uint32_t arrayOffset);
		void setForRendering() const;
		uint32_t getVertexCount() const { return vertexCount; }
		uint32_t getIndexCount() const { return indexCount; }
		uint32_t getDrawingMode() const { return drawingMode; }
		const std::string& getMeshFile() const { return meshFile; }

	private:
		void setIndexBufferData(const uint32_t* data, uint32_t arraySize, bool isDynamic = false);
		void setIndexBufferData(const std::vector<uint32_t>& data, bool isDynamic = false);
		void setVertexBufferData(const float* data, uint32_t arraySize, uint32_t _vertexCount, const std::vector<Model::VertexLayoutSpecification>& layouts, bool isDynamic);
		void setVertexBufferData(const std::vector<float>& data, uint32_t _vertexCount, const std::vector<Model::VertexLayoutSpecification>& layouts, bool isDynamic = false);
		void setDrawingMode(Model::DrawingMode mode);

	protected:
		uint32_t vertexCount{ 0 };
		uint32_t indexCount{ 0 };
		uint32_t drawingMode;
		std::string meshFile = "";
	#if CALA_API == CALA_API_OPENGL
		GLuint vbo = 0;
		GLuint ebo = 0;
		GLuint vao = 0;
	#else
		#error Api not supported yet!
	#endif
	};
}
