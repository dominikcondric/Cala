#include "Model.h"
#include <glm/gtc/constants.hpp>
#include <unordered_map>
#include <glm/gtx/string_cast.hpp>
#include "Logger.h"
#include <fstream>

namespace Cala {
    void Model::createGPUVertexData(uint32_t positionIndex, uint32_t normalIndex, uint32_t texCoordIndex, uint32_t tangentIndex)
    {
		int stride = sizeof(glm::vec3);
		gpuLayoutSpecification.push_back({ positionIndex, 3, 0, 0, 0 });

		if (!normals.empty() && textureCoordinates.empty())
		{
			stride = 2 * sizeof(glm::vec3);
			for (uint32_t i = 0; i < positions.size(); ++i)
			{
				gpuVertexData.push_back(positions[i].x);
				gpuVertexData.push_back(positions[i].y);
				gpuVertexData.push_back(positions[i].z);
				gpuVertexData.push_back(normals[i].x);
				gpuVertexData.push_back(normals[i].y);
				gpuVertexData.push_back(normals[i].z);
			}
			gpuLayoutSpecification.push_back({ normalIndex, 3, 0, sizeof(glm::vec3), 0 });
		}
		else if (normals.empty() && !textureCoordinates.empty())
		{
			auto tangents = calculateTangents();
			stride = 2 * sizeof(glm::vec3) + sizeof(glm::vec2);
			for (uint32_t i = 0; i < positions.size(); ++i)
			{
				gpuVertexData.push_back(positions[i].x);
				gpuVertexData.push_back(positions[i].y);
				gpuVertexData.push_back(positions[i].z);
				gpuVertexData.push_back(textureCoordinates[i].x);
				gpuVertexData.push_back(textureCoordinates[i].y);
				gpuVertexData.push_back(tangents[i].x);
				gpuVertexData.push_back(tangents[i].y);
				gpuVertexData.push_back(tangents[i].z);
			}
			gpuLayoutSpecification.push_back({ texCoordIndex, 2, 0, sizeof(glm::vec3), 0 });
			gpuLayoutSpecification.push_back({ tangentIndex, 3, 0, sizeof(glm::vec3) + sizeof(glm::vec2), 0 });
		}
		else if (!normals.empty() && !textureCoordinates.empty())
		{
			auto tangents = calculateTangents();
			stride = 3 * sizeof(glm::vec3) + sizeof(glm::vec2);
			for (uint32_t i = 0; i < positions.size(); ++i)
			{
				gpuVertexData.push_back(positions[i].x);
				gpuVertexData.push_back(positions[i].y);
				gpuVertexData.push_back(positions[i].z);
				gpuVertexData.push_back(normals[i].x);
				gpuVertexData.push_back(normals[i].y);
				gpuVertexData.push_back(normals[i].z);
				gpuVertexData.push_back(textureCoordinates[i].x);
				gpuVertexData.push_back(textureCoordinates[i].y);
				gpuVertexData.push_back(tangents[i].x);
				gpuVertexData.push_back(tangents[i].y);
				gpuVertexData.push_back(tangents[i].z);
			}
			gpuLayoutSpecification.push_back({ normalIndex, 3, 0, sizeof(glm::vec3), 0 });
			gpuLayoutSpecification.push_back({ texCoordIndex, 2, 0,  2 * sizeof(glm::vec3), 0 });
			gpuLayoutSpecification.push_back({ tangentIndex, 3, 0, 2 * sizeof(glm::vec3) + sizeof(glm::vec2), 0 });
		} 
		else
		{
			for (uint32_t i = 0; i < positions.size(); ++i)
			{
				gpuVertexData.push_back(positions[i].x);
				gpuVertexData.push_back(positions[i].y);
				gpuVertexData.push_back(positions[i].z);
			}
		}

		for (auto& layout : gpuLayoutSpecification)
			layout.strideInBytes = stride;
    }

    Model &Model::loadSphere(uint32_t stackCount, uint32_t sectorCount, float radius)
    {
		name = "Sphere";

		constexpr float pi = glm::pi<float>();
		float sectorStep = 2 * pi / sectorCount;
		float stackStep = pi / stackCount;
		float sectorAngle, stackAngle;
		float xy;
		glm::vec3 vertex;
		for (uint32_t i = 0; i <= stackCount; ++i)
		{
			stackAngle = pi / 2 - i * stackStep;
			xy = radius * glm::cos(stackAngle);
			vertex.z = radius * glm::sin(stackAngle);
			for (uint32_t j = 0; j <= sectorCount; ++j)
			{
				sectorAngle = j * sectorStep;
				vertex.x = xy * glm::cos(sectorAngle);
				vertex.y = xy * glm::sin(sectorAngle);
				normals.push_back(glm::normalize(vertex));
				positions.push_back(vertex);
				textureCoordinates.push_back(glm::vec2((float)j / sectorCount, (float)i / sectorCount));
			}
		}

		int k1, k2;
		for (uint32_t i = 0; i < stackCount; ++i)
		{
			k1 = i * (sectorCount + 1);
			k2 = k1 + sectorCount + 1;
			for (uint32_t j = 0; j < sectorCount; ++j, ++k1, ++k2)
			{
				if (i != 0)
				{
					indices.push_back(k1);
					indices.push_back(k2);
					indices.push_back(k1 + 1);
				}

				if (i != (stackCount - 1))
				{
					indices.push_back(k1 + 1);
					indices.push_back(k2);
					indices.push_back(k2 + 1);
				}
			}
		}

		drawingMode = DrawingMode::Triangles;

		if (generateGPUVertexDataOnLoad)
			createGPUVertexData();

		return *this;
	}

	Model& Model::loadPlane(const glm::vec3& minBound, const glm::vec3& maxBound)
	{
		name = "Plane";

		normals = {
			glm::vec3(0.f, 1.f, 0.f),
			glm::vec3(0.f, 1.f, 0.f),
			glm::vec3(0.f, 1.f, 0.f),
			glm::vec3(0.f, 1.f, 0.f)
		};

		textureCoordinates = {
			glm::vec2(1.f, 0.f),
			glm::vec2(0.f, 0.f),
			glm::vec2(0.f, 1.f),
			glm::vec2(1.f, 1.f)
		};

		positions = {
			glm::vec3(maxBound.x, minBound.y, minBound.z),
			minBound,
			glm::vec3(minBound.x, maxBound.y, maxBound.z),
			maxBound
		};

		indices = { 0, 1, 2, 0, 2, 3 };
		if (generateGPUVertexDataOnLoad)

		drawingMode = DrawingMode::Triangles;
			createGPUVertexData();

		return *this;
	}

	Model& Model::loadRay(const glm::vec3& rayOrigin, const glm::vec3& rayDirection, const float t)
	{
		name = "Ray";

		glm::vec3 rayEnd = rayOrigin + rayDirection * t;
		positions = {
			rayOrigin,
			rayEnd
		};

		drawingMode = DrawingMode::Lines;
		if (generateGPUVertexDataOnLoad)
			createGPUVertexData();

		return *this;
	}

    Model& Model::loadCustomModel(const std::vector<glm::vec3>& _positions, const std::vector<glm::vec3>& _normals, const std::vector<glm::vec2>& _textureCoordinates,
		 const std::vector<uint32_t>& _indices, DrawingMode _drawingMode, const std::string& _name, const std::string& _path)
	{
		path = _path;
		name = _name;
		positions = _positions;
		normals = _normals;
		textureCoordinates = _textureCoordinates;
		indices = _indices;
		drawingMode = _drawingMode;
		if (generateGPUVertexDataOnLoad)
			createGPUVertexData();
		return *this;
	}

	std::vector<glm::vec3> Model::calculateTangents() const
	{
		if (positions.size() == 0 || indices.size() == 0)
			Logger::getInstance().logErrorToConsole("Cannot calculate tangents, vertex data is empty");

		struct Triangle {
			// Positions
			const glm::vec3& p1;
			const glm::vec3& p2;
			const glm::vec3& p3;

			// Texture coordinates
			const glm::vec2& uv1;
			const glm::vec2& uv2;
			const glm::vec2& uv3;
		};

		std::vector<Triangle> triangles;
		std::vector<glm::vec3> tangents(positions.size(), glm::vec3(0.f));
		std::vector<uint32_t> trianglesPerVertex(positions.size(), 0);

		for (uint32_t i = 0; i < indices.size(); i += 3) {
			Triangle t{ positions[indices[i]], positions[indices[i + 1]], positions[indices[i + 2]],
						textureCoordinates[indices[i]], textureCoordinates[indices[i + 1]], textureCoordinates[indices[i + 2]] };

			glm::vec2 deltaUV1 = t.uv2 - t.uv1;
			glm::vec2 deltaUV2 = t.uv3 - t.uv1;
			glm::vec3 edge1 = t.p2 - t.p1;
			glm::vec3 edge2 = t.p3 - t.p1;
			const float f = 1.f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

			glm::vec3 tangent {
				f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x),
				f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y),
				f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z)
			};

			for (uint32_t j = 0; j < 3; ++j)
			{
				tangents[indices[i+j]] += tangent;
				trianglesPerVertex[indices[i+j]]++;
			}
		}

		for (uint32_t i = 0; i < tangents.size(); ++i)
		{
			tangents[i] /= trianglesPerVertex[i];
		}

		return tangents;
	}

	Model& Model::loadCube(const glm::vec3& minBound, const glm::vec3& maxBound)
	{
		name = "Cube";
		
		positions = {
			// front side
			maxBound,
			glm::vec3(minBound.x, maxBound.y, maxBound.z),
			glm::vec3(minBound.x,minBound.y, maxBound.z),
			glm::vec3(maxBound.x,minBound.y, maxBound.z),
			// left side
			glm::vec3(minBound.x, maxBound.y, maxBound.z),
			glm::vec3(minBound.x, maxBound.y,minBound.z),
			minBound,
			glm::vec3(minBound.x,minBound.y, maxBound.z),
			// back side
			glm::vec3(maxBound.x, maxBound.y,minBound.z),
			glm::vec3(minBound.x, maxBound.y,minBound.z),
			minBound,
			glm::vec3(maxBound.x,minBound.y,minBound.z),
			// right side
			maxBound,
			glm::vec3(maxBound.x, maxBound.y,minBound.z),
			glm::vec3(maxBound.x,minBound.y,minBound.z),
			glm::vec3(maxBound.x,minBound.y, maxBound.z),
			// top side
			glm::vec3(maxBound.x, maxBound.y,minBound.z),
			glm::vec3(minBound.x, maxBound.y,minBound.z),
			glm::vec3(minBound.x, maxBound.y, maxBound.z),
			maxBound,
			// bottom side
			glm::vec3(maxBound.x,minBound.y,minBound.z),
			minBound,
			glm::vec3(minBound.x,minBound.y, maxBound.z),
			glm::vec3(maxBound.x,minBound.y, maxBound.z)
		};

		indices = { 0, 1, 2, 0, 2, 3, 4, 5, 6, 4, 6, 7, 8, 11, 10, 8, 10, 9, 12, 15, 14, 12, 14, 13, 16, 17, 18, 16, 18, 19, 20, 23, 22, 20, 22, 21 };
		normals = {
			// front side
			glm::vec3(0.f, 0.f, 1.f),
			glm::vec3(0.f, 0.f, 1.f),
			glm::vec3(0.f, 0.f, 1.f),
			glm::vec3(0.f, 0.f, 1.f),
			// left side
			glm::vec3(-1.f, 0.f, 0.f),
			glm::vec3(-1.f, 0.f, 0.f),
			glm::vec3(-1.f, 0.f, 0.f),
			glm::vec3(-1.f, 0.f, 0.f),
			// back side
			glm::vec3(0.f, 0.f, -1.f),
			glm::vec3(0.f, 0.f, -1.f),
			glm::vec3(0.f, 0.f, -1.f),
			glm::vec3(0.f, 0.f, -1.f),
			// right side
			glm::vec3(1.f, 0.f, 0.f),
			glm::vec3(1.f, 0.f, 0.f),
			glm::vec3(1.f, 0.f, 0.f),
			glm::vec3(1.f, 0.f, 0.f),
			// top side
			glm::vec3(0.f, 1.f, 0.f),
			glm::vec3(0.f, 1.f, 0.f),
			glm::vec3(0.f, 1.f, 0.f),
			glm::vec3(0.f, 1.f, 0.f),
			// bottom side
			glm::vec3(0.f, -1.f, 0.f),
			glm::vec3(0.f, -1.f, 0.f),
			glm::vec3(0.f, -1.f, 0.f),
			glm::vec3(0.f, -1.f, 0.f),
		};

		textureCoordinates = {
			glm::vec2(0.0f, 0.0f),
			glm::vec2(1.0f, 0.0f),
			glm::vec2(1.0f, 1.0f),
			glm::vec2(0.0f, 1.0f),
			glm::vec2(0.0f, 0.0f),
			glm::vec2(1.0f, 0.0f),
			glm::vec2(1.0f, 1.0f),
			glm::vec2(0.0f, 1.0f),
			glm::vec2(0.0f, 0.0f),
			glm::vec2(1.0f, 0.0f),
			glm::vec2(1.0f, 1.0f),
			glm::vec2(0.0f, 1.0f),
			glm::vec2(0.0f, 0.0f),
			glm::vec2(1.0f, 0.0f),
			glm::vec2(1.0f, 1.0f),
			glm::vec2(0.0f, 1.0f),
			glm::vec2(0.0f, 0.0f),
			glm::vec2(1.0f, 0.0f),
			glm::vec2(1.0f, 1.0f),
			glm::vec2(0.0f, 1.0f),
			glm::vec2(0.0f, 0.0f),
			glm::vec2(1.0f, 0.0f),
			glm::vec2(1.0f, 1.0f),
			glm::vec2(0.0f, 1.0f),
		};

		drawingMode = DrawingMode::Triangles;
		if (generateGPUVertexDataOnLoad)
			createGPUVertexData();

		return *this;
	}

	void Model::removeReduntantPositions()
	{
		/*
		* Algorithm for removing redundant positions
		* Iterate over every vertex and add every vertex to new array only once
		* Create a map that maps every previous vertex index to new vertex index
		* Recreate index list using previous-to-new index mapping
		*/

		std::vector<glm::vec3> newPositions;
		std::vector<uint32_t> newIndices(indices);
		std::vector<uint32_t> oldToNewIndexMapping(positions.size());
		std::unordered_map<std::string, uint32_t> vertexMap;

		for (uint32_t i = 0; i < positions.size(); i++)
		{
			glm::vec3 vertex(positions[i]);
			auto mapIterator = vertexMap.find(glm::to_string(vertex));
			if (mapIterator == vertexMap.end())
			{
				oldToNewIndexMapping[i] = (uint32_t)vertexMap.size();							    // Map old vertex to new vertex index
				vertexMap.insert({ glm::to_string(vertex), (uint32_t)vertexMap.size() });			// Add unique vertex
				newPositions.push_back(positions[i]);												// Add vertex to new vertex array
			}
			else
			{
				oldToNewIndexMapping[i] = mapIterator->second;
			}
		}

		for (uint32_t i = 0; i < indices.size(); ++i)
		{
			uint32_t oldIndex = indices[i];
			newIndices[i] = oldToNewIndexMapping[oldIndex];
		}

		positions = newPositions;
		indices = newIndices;

		if (!gpuVertexData.empty())
			createGPUVertexData();
	}
}