#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <string>

namespace Cala {
	class Model {
	public:
		struct VertexLayoutSpecification {
			uint32_t index;
			uint32_t componentCount;
			int strideInBytes;
			int offsetInBytes;
			int divisor;
		};

		enum class DrawingMode {
			Triangles,
			TriangleFan,
			TriangleStrip,
			Lines,
			LineLoop,
			LineStrip
		};

	public:
		Model() = default;
		~Model() = default;
		const std::vector<float>& getVertexData() const { return vertexData; }
		const std::vector<uint32_t>& getIndexData() const { return indexData; }
		const std::vector<VertexLayoutSpecification>& getLayoutSpecification() const { return layoutSpecification; }
		DrawingMode getDrawingMode() const { return drawingMode; }
		uint32_t getVertexCount() const { return vertexCount; }
		uint32_t getIndexCount() const { return indexCount; }
		const std::string& getModelPath() const { return path; }
		void removeReduntantVertices();

		Model& loadSphere(uint32_t stackCount = 30, uint32_t sectorCount = 50, float radius = 0.5f);
		Model& loadCube(const glm::vec3& minBound = glm::vec3(-0.5f), const glm::vec3& maxBound = glm::vec3(0.5f));
		Model& loadPlane(const glm::vec3& minBound = glm::vec3(-0.5f, 0.f, -0.5f), const glm::vec3& maxBound = glm::vec3(0.5f, 0.f, 0.5f));
		Model& loadRay(const glm::vec3& rayOrigin = glm::vec3(0.f), const glm::vec3& rayDirection = glm::vec3(0.f, 0.f, 1.f), const float t = 10.f);
		Model& loadCustomModel(const std::vector<float>& vertexData, uint32_t vertexCount, const std::vector<uint32_t>& indexData, const std::vector<VertexLayoutSpecification>& _layoutSpecification, DrawingMode _drawingMode);

	private:
		std::vector<float> vertexData;
		std::vector<uint32_t> indexData;
		std::vector<VertexLayoutSpecification> layoutSpecification;
		DrawingMode drawingMode;
		uint32_t indexCount = 0;
		uint32_t vertexCount = 0;
		std::string path;

		// Must be called after index data is defined
		std::vector<glm::vec3> calculateTangents(const std::vector<glm::vec3>& vertices, 
			const std::vector<glm::vec2>& textureCoordinates, const std::vector<uint32_t>& indices) const;
	};
}
