#include "Model.h"
#include <glm/gtc/constants.hpp>
#include <unordered_map>
#include <glm/gtx/string_cast.hpp>

namespace Cala {
	Model& Model::loadSphere(uint32_t stackCount, uint32_t sectorCount, float radius)
	{
		path = "Sphere";
		vertexCount = (sectorCount + 1) * (stackCount + 1);
		indexCount = ((stackCount - 2) * sectorCount * 6) + (2 * sectorCount * 3);

		const float len = 1.f / radius;
		std::vector<glm::vec3> vertices;
		vertices.reserve(vertexCount);
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
				vertices.push_back(vertex);
			}
		}

		indexData.reserve(indexCount);
		int k1, k2;
		for (uint32_t i = 0; i < stackCount; ++i)
		{
			k1 = i * (sectorCount + 1);
			k2 = k1 + sectorCount + 1;
			for (uint32_t j = 0; j < sectorCount; ++j, ++k1, ++k2)
			{
				if (i != 0)
				{
					indexData.push_back(k1);
					indexData.push_back(k2);
					indexData.push_back(k1 + 1);
				}

				if (i != (stackCount - 1))
				{
					indexData.push_back(k1 + 1);
					indexData.push_back(k2);
					indexData.push_back(k2 + 1);
				}
			}
		}

		vertexData.reserve(vertexCount * 8);
		uint32_t k = 0;
		for (uint32_t i = 0; i <= stackCount; ++i)
		{
			for (uint32_t j = 0; j <= sectorCount; ++j)
			{
				vertexData.push_back(vertices[k].x);
				vertexData.push_back(vertices[k].y);
				vertexData.push_back(vertices[k].z);

				vertexData.push_back(vertices[k].x * len);
				vertexData.push_back(vertices[k].y * len);
				vertexData.push_back(vertices[k].z * len);

				vertexData.push_back((float)j / sectorCount);
				vertexData.push_back((float)i / stackCount);
				++k;
			}
		}

		drawingMode = DrawingMode::Triangles;

		layoutSpecification.reserve(3);
		layoutSpecification.push_back({ 0, 3, 8 * sizeof(float), 0, 1 });
		layoutSpecification.push_back({ 1, 3, 8 * sizeof(float), sizeof(glm::vec3), 1 });
		layoutSpecification.push_back({ 2, 2, 8 * sizeof(float), 2 * sizeof(glm::vec3), 1 });

		return *this;
	}

	Model& Model::loadPlane(const glm::vec3& minBound, const glm::vec3& maxBound)
	{
		path = "Plane";
		vertexCount = 4;
		indexCount = 6;

		vertexData.reserve(8 * 4);
		const glm::vec3 normals[4] = {
			glm::vec3(0.f, 1.f, 0.f),
			glm::vec3(0.f, 1.f, 0.f),
			glm::vec3(0.f, 1.f, 0.f),
			glm::vec3(0.f, 1.f, 0.f)
		};

		const glm::vec2 texCoords[4] = {
			glm::vec2(0.f, 0.f),
			glm::vec2(1.f, 0.f),
			glm::vec2(1.f, 1.f),
			glm::vec2(0.f, 1.f)
		};

		const glm::vec3 vertices[4] = {
			glm::vec3(maxBound.x, minBound.y, minBound.z),
			minBound,
			glm::vec3(minBound.x, maxBound.y, maxBound.z),
			maxBound
		};

		indexData = { 0, 1, 2, 0, 2, 3 };

		for (int i = 0; i < 4; ++i)
		{
			vertexData.push_back(vertices[i].x);
			vertexData.push_back(vertices[i].y);
			vertexData.push_back(vertices[i].z);
			vertexData.push_back(normals[i].x);
			vertexData.push_back(normals[i].y);
			vertexData.push_back(normals[i].z);
			vertexData.push_back(texCoords[i].x);
			vertexData.push_back(texCoords[i].y);
		}

		drawingMode = DrawingMode::Triangles;
		layoutSpecification.reserve(3);
		layoutSpecification.push_back({ 0, 3, 8 * sizeof(float), 0, 1 });
		layoutSpecification.push_back({ 1, 3, 8 * sizeof(float), sizeof(glm::vec3), 1 });
		layoutSpecification.push_back({ 2, 2, 8 * sizeof(float), 2 * sizeof(glm::vec3), 1 });

		return *this;
	}

	Model& Model::loadRay(const glm::vec3& rayOrigin, const glm::vec3& rayDirection, const float t)
	{
		path = "Ray";
		vertexCount = 2;
		indexCount = 0;

		glm::vec3 rayEnd = rayOrigin + rayDirection * t;
		vertexData = {
			rayOrigin.x, rayOrigin.y, rayOrigin.z,
			rayEnd.x, rayEnd.y, rayEnd.z
		};

		drawingMode = DrawingMode::Lines;
		layoutSpecification.push_back({ 0, 3, 0, 0, 1 });

		return *this;
	}

	Model& Model::loadCustomModel(const std::vector<float>& _vertexData, uint32_t _vertexCount,
		const std::vector<uint32_t>& _indexData, const std::vector<VertexLayoutSpecification>& _layoutSpecification, DrawingMode _drawingMode)
	{
		path = "custom";
		vertexData = _vertexData;
		vertexCount = _vertexCount;
		indexData = _indexData;
		indexCount = (uint32_t)_indexData.size();
		layoutSpecification = _layoutSpecification;
		drawingMode = _drawingMode;
		return *this;
	}

	Model& Model::loadCube(const glm::vec3& minBound, const glm::vec3& maxBound)
	{
		path = "Cube";
		vertexCount = 24;
		indexCount = 36;
		const glm::vec3 vertices[] = {
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

		indexData = { 0, 1, 2, 0, 2, 3, 4, 5, 6, 4, 6, 7, 8, 11, 10, 8, 10, 9, 12, 15, 14, 12, 14, 13, 16, 17, 18, 16, 18, 19, 20, 23, 22, 20, 22, 21 };
		const glm::vec3 normals[24] = {
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

		const glm::vec2 texCoords[24] = {
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

		/*const glm::vec3 tangents[verticesCount] = {
			// front side
			glm::vec3(1.f, 0.f, 0.f),
			glm::vec3(1.f, 0.f, 0.f),
			glm::vec3(1.f, 0.f, 0.f),
			glm::vec3(1.f, 0.f, 0.f),
			// left side
			glm::vec3(0.f, 0.f, 1.f),
			glm::vec3(0.f, 0.f, 1.f),
			glm::vec3(0.f, 0.f, 1.f),
			glm::vec3(0.f, 0.f, 1.f),
			// back side
			glm::vec3(1.f, 0.f, 0.f),
			glm::vec3(1.f, 0.f, 0.f),
			glm::vec3(1.f, 0.f, 0.f),
			glm::vec3(1.f, 0.f, 0.f),
			// right side
			glm::vec3(0.f, 0.f, -1.f),
			glm::vec3(0.f, 0.f, -1.f),
			glm::vec3(0.f, 0.f, -1.f),
			glm::vec3(0.f, 0.f, -1.f),
			// top side
			glm::vec3(1.f, 0.f, 0.f),
			glm::vec3(1.f, 0.f, 0.f),
			glm::vec3(1.f, 0.f, 0.f),
			glm::vec3(1.f, 0.f, 0.f),
			// bottom side
			glm::vec3(1.f, 0.f, 0.f),
			glm::vec3(1.f, 0.f, 0.f),
			glm::vec3(1.f, 0.f, 0.f),
			glm::vec3(1.f, 0.f, 0.f)
		};

		const glm::vec3 bitangents[verticesCount] = {
			// front side
			glm::vec3(0.f, 1.f, 0.f),
			glm::vec3(0.f, 1.f, 0.f),
			glm::vec3(0.f, 1.f, 0.f),
			glm::vec3(0.f, 1.f, 0.f),
			// left side
			glm::vec3(0.f, 1.f, 0.f),
			glm::vec3(0.f, 1.f, 0.f),
			glm::vec3(0.f, 1.f, 0.f),
			glm::vec3(0.f, 1.f, 0.f),
			// back side
			glm::vec3(0.f, 1.f, 0.f),
			glm::vec3(0.f, 1.f, 0.f),
			glm::vec3(0.f, 1.f, 0.f),
			glm::vec3(0.f, 1.f, 0.f),
			// right side
			glm::vec3(0.f, 1.f, 0.f),
			glm::vec3(0.f, 1.f, 0.f),
			glm::vec3(0.f, 1.f, 0.f),
			glm::vec3(0.f, 1.f, 0.f),
			// top side
			glm::vec3(0.f, 0.f, -1.f),
			glm::vec3(0.f, 0.f, -1.f),
			glm::vec3(0.f, 0.f, -1.f),
			glm::vec3(0.f, 0.f, -1.f),
			// bottom side
			glm::vec3(0.f, 0.f, 1.f),
			glm::vec3(0.f, 0.f, 1.f),
			glm::vec3(0.f, 0.f, 1.f),
			glm::vec3(0.f, 0.f, 1.f)
		};*/

		vertexData.reserve(8 * vertexCount);
		for (uint32_t i = 0; i < 24; ++i)
		{
			vertexData.push_back(vertices[i].x);
			vertexData.push_back(vertices[i].y);
			vertexData.push_back(vertices[i].z);
			vertexData.push_back(normals[i].x);
			vertexData.push_back(normals[i].y);
			vertexData.push_back(normals[i].z);
			vertexData.push_back(texCoords[i].x);
			vertexData.push_back(texCoords[i].y);
			/*vertexData.push_back(tangents[i].x);
			vertexData.push_back(tangents[i].y);
			vertexData.push_back(tangents[i].z);
			vertexData.push_back(bitangents[i].x);
			vertexData.push_back(bitangents[i].y);
			vertexData.push_back(bitangents[i].z);*/
		}

		drawingMode = DrawingMode::Triangles;
		layoutSpecification.reserve(3);
		layoutSpecification.push_back({ 0, 3, 8 * sizeof(float), 0, 1 });
		layoutSpecification.push_back({ 1, 3, 8 * sizeof(float), sizeof(glm::vec3), 1 });
		layoutSpecification.push_back({ 2, 2, 8 * sizeof(float), 2 * sizeof(glm::vec3), 1 });

		return *this;
	}

	void Model::removeReduntantVertices()
	{
		/*
		* Algorithm for removing redundant vertices
		* Iterate over every vertex and add every vertex to new array only once
		* Create a map that maps every previous vertex index to new vertex index
		* Recreate index list using previous-to-new index mapping
		*/

		std::vector<float> newVertices;
		std::vector<uint32_t> newIndices(indexCount);
		std::vector<uint32_t> oldToNewIndexMapping(vertexCount);
		std::unordered_map<std::string, uint32_t> vertexMap;

		for (int i = 0; i < vertexCount; i++)
		{
			glm::vec3 vertex(vertexData[i * 8], vertexData[i * 8 + 1], vertexData[i * 8 + 2]);
			auto mapIterator = vertexMap.find(glm::to_string(vertex));
			if (mapIterator == vertexMap.end())
			{
				oldToNewIndexMapping[i] = vertexMap.size();							    // Map old vertex to new vertex index
				vertexMap.insert({ glm::to_string(vertex), vertexMap.size() });			// Add unique vertex
				for (int j = 0; j < 8; ++j)
					newVertices.push_back(vertexData[i * 8 + j]);						// Add vertex to new vertex array
			}
			else
			{
				oldToNewIndexMapping[i] = mapIterator->second;
			}
		}

		for (int i = 0; i < indexCount; ++i)
		{
			uint32_t oldIndex = indexData[i];
			newIndices[i] = oldToNewIndexMapping[oldIndex];
		}

		vertexData = newVertices;
		vertexCount = newVertices.size() / 8;
		indexData = newIndices;
	}
}