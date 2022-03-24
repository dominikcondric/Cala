#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <string>

class Mesh {
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
	Mesh() = default;
	virtual ~Mesh() = default;
	void loadSphere(uint32_t stackCount = 30, uint32_t sectorCount = 50, float radius = 0.5f);
	void loadCube(const glm::vec3& minBound = glm::vec3(-0.5f), const glm::vec3& maxBound = glm::vec3(0.5f));
	void loadPlane(const glm::vec3& minBound = glm::vec3(-0.5f, 0.f, -0.5f), const glm::vec3& maxBound = glm::vec3(0.5f, 0.f, 0.5f));
	void loadRay(const glm::vec3& rayOrigin = glm::vec3(0.f), const glm::vec3& rayDirection = glm::vec3(0.f, 0.f, 1.f), const float t = 10.f);
	//void loadCloth(uint32_t xCount = 10, uint32_t yCount = 10);
	//void loadFromOBJ();
	virtual void setIndexBufferData(const uint32_t* data, uint32_t arraySize, bool isDynamic = false) = 0;
	virtual void setIndexBufferData(const std::vector<uint32_t>& data, bool isDynamic = false) = 0;
	virtual void updateIndexBufferData(const uint32_t* data, uint32_t arraySize, uint32_t arrayOffset) = 0;
	virtual void updateIndexBufferData(const std::vector<uint32_t>& data, uint32_t arrayOffset) = 0;
	virtual void setVertexBufferData(const float* data, uint32_t arraySize, uint32_t _vertexCount, const std::vector<VertexLayoutSpecification>& layouts, bool isDynamic) = 0;
	virtual void setVertexBufferData(const std::vector<float>& data, uint32_t _vertexCount, const std::vector<VertexLayoutSpecification>& layouts, bool isDynamic = false) = 0;
	virtual void updateVertexBufferData(const float* data, uint32_t arraySize, uint32_t arrayOffset) = 0;
	virtual void updateVertexBufferData(const std::vector<float>& data, uint32_t arrayOffset) = 0;
	virtual void setForRendering() const = 0;
	virtual void setDrawingMode(DrawingMode mode) = 0;
	uint32_t getVertexCount() const { return vertexCount; }
	uint32_t getIndexCount() const { return indexCount; }
	uint32_t getDrawingMode() const { return drawingMode; }
	const std::string& getMeshFile() const { return meshFile; }

protected:
	uint32_t vertexCount{ 0 };
	uint32_t indexCount{ 0 };
	uint32_t drawingMode;
	std::string meshFile;
};

