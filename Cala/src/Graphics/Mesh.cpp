#include "Mesh.h"
#include <glm/gtc/constants.hpp>

void Mesh::loadSphere(uint32_t stackCount, uint32_t sectorCount, float radius)
{
	const int verticesCount = (sectorCount + 1) * (stackCount + 1);
	const int indicesCount = ((stackCount - 2) * sectorCount * 6) + (2 * sectorCount * 3);
	const float len = 1.f / radius;
	std::vector<glm::vec3> vertices;
	vertices.reserve(verticesCount);
	constexpr float pi = glm::pi<float>();
	float sectorStep = 2 * pi / sectorCount;
	float stackStep = pi / stackCount;
	float sectorAngle, stackAngle;
	float xy;
	glm::vec3 vertex;
	for (uint32_t i = 0; i <= stackCount; ++i)
	{
		stackAngle = pi / 2 - i * stackStep;
		xy = radius * cos(stackAngle);
		vertex.z = radius * sin(stackAngle);
		for (uint32_t j = 0; j <= sectorCount; ++j)
		{
			sectorAngle = j * sectorStep;
			vertex.x = xy * cos(sectorAngle);
			vertex.y = xy * sin(sectorAngle);
			vertices.push_back(vertex);
		}
	}

	std::vector<uint32_t> indices;
	indices.reserve(indicesCount);
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

	std::vector<float> data;
	data.reserve(verticesCount * 8);
	uint32_t k = 0;
	for (uint32_t i = 0; i <= stackCount; ++i)
	{
		for (uint32_t j = 0; j <= sectorCount; ++j)
		{
			data.push_back(vertices[k].x);
			data.push_back(vertices[k].y);
			data.push_back(vertices[k].z);

			data.push_back(vertices[k].x * len);
			data.push_back(vertices[k].y * len);
			data.push_back(vertices[k].z * len);

			data.push_back((float)j / sectorCount);
			data.push_back((float)i / stackCount);
			++k;
		}
	}

	setDrawingMode(DrawingMode::Triangles);

	std::vector<VertexLayoutSpecification> layouts;
	layouts.reserve(3);
	layouts.push_back({ 0, 3, 8 * sizeof(float), 0, 1 });
	layouts.push_back({ 1, 3, 8 * sizeof(float), sizeof(glm::vec3), 1 });
	layouts.push_back({ 2, 2, 8 * sizeof(float), 2 * sizeof(glm::vec3), 1 });
	setVertexBufferData(data, verticesCount, layouts, false);
	setIndexBufferData(indices, false);
}

void Mesh::loadPlane(const glm::vec3& minBound, const glm::vec3& maxBound)
{
	std::vector<float> data;
	data.reserve(8 * 4);
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

	uint32_t indices[] = { 0, 1, 2, 0, 2, 3 };

	for (int i = 0; i < 4; ++i)
	{
		data.push_back(vertices[i].x);
		data.push_back(vertices[i].y);
		data.push_back(vertices[i].z);
		data.push_back(normals[i].x);
		data.push_back(normals[i].y);
		data.push_back(normals[i].z);
		data.push_back(texCoords[i].x);
		data.push_back(texCoords[i].y);
	}
	
	setDrawingMode(DrawingMode::Triangles);
	std::vector<VertexLayoutSpecification> layouts;
	layouts.reserve(3);
	layouts.push_back({ 0, 3, 8 * sizeof(float), 0, 1 });
	layouts.push_back({ 1, 3, 8 * sizeof(float), sizeof(glm::vec3), 1 });
	layouts.push_back({ 2, 2, 8 * sizeof(float), 2 * sizeof(glm::vec3), 1 });
	setVertexBufferData(data, 4, layouts, false);
	setIndexBufferData(indices, 6, false);
}

void Mesh::loadRay(const glm::vec3& rayOrigin, const glm::vec3& rayDirection, const float t)
{
	const glm::vec3 vertices[2] = {
		rayOrigin,
		rayOrigin + rayDirection * t
	};

	float data[] = { vertices[0].x, vertices[0].y, vertices[0].z, vertices[1].x, vertices[1].y, vertices[1].z };

	setDrawingMode(DrawingMode::Lines);
	std::vector<VertexLayoutSpecification> layouts;
	layouts.push_back({ 0, 3, 0, 0, 1 });
	setVertexBufferData(data, 6, 2, layouts, false);
}

//void Mesh::loadCloth(uint32_t horizontalCount, uint32_t verticalCount) const
//{
//	if (horizontalCount % 2 != 0) ++horizontalCount;
//	if (verticalCount % 2 != 0) ++verticalCount;
//	const int indicesCount = (horizontalCount - 1) * (verticalCount - 1) * 6;
//	const int verticesCount = horizontalCount * verticalCount;
//
//	ClothComponent& cloth = scene.getComponent<ClothComponent>(entID);
//	cloth.points.reserve(verticesCount);
//	cloth.springs.reserve(8 * verticesCount - 5 * horizontalCount - 5 * verticalCount + 4);
//	cloth.translations.reserve(verticesCount);
//	std::vector<GLuint> indices;
//	indices.reserve(indicesCount);
//	std::vector<float> data;
//	data.reserve(verticalCount * 8);
//	float xDelta = 1.f / horizontalCount;
//	float yDelta = 1.f / verticalCount;
//
//	for (uint32_t i = 0; i < verticalCount; ++i)
//	{
//		for (uint32_t j = 0; j < horizontalCount; ++j)
//		{
//			// Position
//			data.push_back(-0.5f + j * xDelta);
//			data.push_back(0.5f - i * yDelta);
//			data.push_back(0.f);
//
//			// Normal
//			data.push_back(0.f);
//			data.push_back(0.f);
//			data.push_back(1.f);
//
//			// Texture coordinates
//			data.push_back(0.f + j * xDelta);
//			data.push_back(1.f - i * yDelta);
//
//			// Cloth particle
//			cloth.points.push_back(ClothComponent::Particle(glm::vec3(-0.5f + j * xDelta, 0.5f - i * yDelta, 0.f)));
//
//			cloth.translations.push_back(glm::vec3(0.f, 0.f, 0.f));
//			if (j > 0 && i > 0)
//			{
//				indices.push_back((i - 1) * horizontalCount + j - 1);
//				indices.push_back(i * horizontalCount + j - 1);
//				indices.push_back(i * horizontalCount + j);
//				indices.push_back(i * horizontalCount + j);
//				indices.push_back((i - 1) * horizontalCount + j);
//				indices.push_back((i - 1) * horizontalCount + j - 1);
//
//				// Cloth springs --> point connected to each adjacent point and second next vertically and horizontally
//				cloth.springs.push_back(ClothComponent::Spring((i - 1) * horizontalCount + j - 1, i * horizontalCount + j - 1, ClothComponent::Structural));
//				cloth.springs.push_back(ClothComponent::Spring((i - 1) * horizontalCount + j - 1, (i - 1) * horizontalCount + j, ClothComponent::Structural));
//				cloth.springs.push_back(ClothComponent::Spring((i - 1) * horizontalCount + j - 1, i * horizontalCount + j, ClothComponent::Shear));
//				cloth.springs.push_back(ClothComponent::Spring((i - 1) * horizontalCount + j, i * horizontalCount + j - 1, ClothComponent::Shear));
//				if (i == verticalCount - 1)
//					cloth.springs.push_back(ClothComponent::Spring(i * horizontalCount + j - 1, i * horizontalCount + j, ClothComponent::Structural));
//				if (j == horizontalCount - 1)
//					cloth.springs.push_back(ClothComponent::Spring((i - 1) * horizontalCount + j, i * horizontalCount + j, ClothComponent::Structural));
//
//				if (j > 1 && i > 1)
//				{
//					cloth.springs.push_back(ClothComponent::Spring((i - 2) * horizontalCount + j - 2, (i - 2) * horizontalCount + j, ClothComponent::Bending));
//					cloth.springs.push_back(ClothComponent::Spring((i - 2) * horizontalCount + j - 2, i * horizontalCount + j - 2, ClothComponent::Bending));
//					if (i == verticalCount - 1 || i == verticalCount - 2)
//						cloth.springs.push_back(ClothComponent::Spring(i * horizontalCount + j - 2, i * horizontalCount + j, ClothComponent::Bending));
//					if (j == horizontalCount - 1 || j == horizontalCount - 2)
//						cloth.springs.push_back(ClothComponent::Spring((i - 2) * horizontalCount + j, i * horizontalCount + j, ClothComponent::Bending));
//				}
//			}
//		}
//	}
//
//	vertexCount = verticesCount;
// 
//	drawingMode = GL_TRIANGLES;
//	layouts.reserve(3);
//	layouts.push_back({ 0, 3, 8 * sizeof(float), 0 });
//	layouts.push_back({ 1, 3, 8 * sizeof(float), sizeof(glm::vec3) });
//	layouts.push_back({ 2, 2, 8 * sizeof(float), 2 * sizeof(glm::vec3) });
//	dynamic = dynamicData;
//}

void Mesh::loadCube(const glm::vec3& minBound, const glm::vec3& maxBound)
{
	const int indicesCount = 36;
	const int verticesCount = 24;
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

	uint32_t indices[] = { 0, 1, 2, 0, 2, 3, 4, 5, 6, 4, 6, 7, 8, 11, 10, 8, 10, 9, 12, 15, 14, 12, 14, 13, 16, 17, 18, 16, 18, 19, 20, 23, 22, 20, 22, 21 };
	const glm::vec3 normals[verticesCount] = {
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

	const glm::vec2 texCoords[verticesCount] = {
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

	std::vector<float> data;
	data.reserve(8 * verticesCount);
	for (uint32_t i = 0; i < 24; ++i)
	{
		data.push_back(vertices[i].x);
		data.push_back(vertices[i].y);
		data.push_back(vertices[i].z);
		data.push_back(normals[i].x);
		data.push_back(normals[i].y);
		data.push_back(normals[i].z);
		data.push_back(texCoords[i].x);
		data.push_back(texCoords[i].y);
		/*data.push_back(tangents[i].x);
		data.push_back(tangents[i].y);
		data.push_back(tangents[i].z);
		data.push_back(bitangents[i].x);
		data.push_back(bitangents[i].y);
		data.push_back(bitangents[i].z);*/
	}

	setDrawingMode(DrawingMode::Triangles);
	std::vector<VertexLayoutSpecification> layouts;
	layouts.reserve(3);
	layouts.push_back({ 0, 3, 8 * sizeof(float), 0, 1 });
	layouts.push_back({ 1, 3, 8 * sizeof(float), sizeof(glm::vec3), 1 });
	layouts.push_back({ 2, 2, 8 * sizeof(float), 2 * sizeof(glm::vec3), 1 });
	setVertexBufferData(data, verticesCount, layouts, false);
	setIndexBufferData(indices, indicesCount, false);
}
