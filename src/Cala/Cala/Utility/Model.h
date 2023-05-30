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
		Model(bool _generateGPUVertexDataOnLoad = true) : generateGPUVertexDataOnLoad(_generateGPUVertexDataOnLoad) {}
		~Model() = default;
		const std::vector<glm::vec3>& getPositions() const { return positions; }
		const std::vector<glm::vec3>& getNormals() const { return normals; }
		const std::vector<glm::vec2>& getTextureCoordinates() const { return textureCoordinates; }
		const std::vector<uint32_t>& getIndices() const { return indices; }
		const std::vector<VertexLayoutSpecification>& getLayoutSpecification() const { return gpuLayoutSpecification; }
		const std::vector<float>& getGPUVertexData() const { return gpuVertexData; }
		DrawingMode getDrawingMode() const { return drawingMode; }
		const std::string& getModelPath() const { return path; }
		const std::string& getModelName() const { return name; }
		void removeReduntantPositions();

		void createGPUVertexData(uint32_t positionIndex = 0, uint32_t normalsIndex = 1, uint32_t texCoordsIndex = 2, uint32_t tangentsIndex = 3);
		Model& loadSphere(uint32_t stackCount = 30, uint32_t sectorCount = 50, float radius = 0.5f);
		Model& loadCube(const glm::vec3& minBound = glm::vec3(-0.5f), const glm::vec3& maxBound = glm::vec3(0.5f));
		Model& loadPlane(const glm::vec3& minBound = glm::vec3(-0.5f, 0.f, -0.5f), const glm::vec3& maxBound = glm::vec3(0.5f, 0.f, 0.5f));
		Model& loadRay(const glm::vec3& rayOrigin = glm::vec3(0.f), const glm::vec3& rayDirection = glm::vec3(0.f, 0.f, 1.f), const float t = 10.f);
        Model &loadCustomModel(const std::vector<glm::vec3> &_positions, const std::vector<glm::vec3> &_normals, const std::vector<glm::vec2> &_textureCoordinates, const std::vector<uint32_t> &_indices, DrawingMode _drawingMode, const std::string &name);

    private:
		std::vector<glm::vec3> positions;
		std::vector<glm::vec3> normals;
		std::vector<glm::vec2> textureCoordinates;
		std::vector<uint32_t> indices;

		bool generateGPUVertexDataOnLoad;
		std::vector<float> gpuVertexData;
		std::vector<VertexLayoutSpecification> gpuLayoutSpecification;

		DrawingMode drawingMode;
		std::string path;
		std::string name;

		// Must be called after index data is defined
		std::vector<glm::vec3> calculateTangents() const;
	};
}
