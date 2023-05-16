#include "ModelLoader.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include "Logger.h"

Cala::ModelLoader::ModelLoader(const std::filesystem::path &modelPath)
{
    std::filesystem::path pathExtension = modelPath.extension();
    if (pathExtension == ".obj")
        loadFromObj(modelPath);
}

void Cala::ModelLoader::loadFromObj(const std::filesystem::path &modelPath)
{
    if (!std::filesystem::exists(modelPath))
        return;
        
    if (modelPath.extension() != ".obj")
        return;

    if (!models.empty())
        models.clear();

    std::ifstream modelFile;

    try {
        modelFile.open(modelPath);
    }
    catch (std::ifstream::failure& e) {
        Logger::getInstance().logErrorToConsole(e.what());
    }

    std::string line;
    while (std::getline(modelFile, line))
    {
        std::istringstream lineStream(line);
        std::string label;
        lineStream >> label;
        if (label == "v")
        {
            if (modelCompleted)
            {
                modelCompleted = false;
                models.emplace_back(generateGPUVertexDataOnLoad).loadCustomModel(positions, alignedNormals, alignedTextureCoordinates, indices, Model::DrawingMode::Triangles, modelName);
                positionsOffset += positions.size();
                texCoordsOffset += textureCoordinates.size();
                normalsOffset += normals.size();
                positions.clear();
                indices.clear();
                normals.clear();
                textureCoordinates.clear();
                alignedNormals.clear();
                alignedTextureCoordinates.clear();
            }

            glm::vec3 position;
            lineStream >> position.x >> position.y >> position.z;
            positions.push_back(position);
        }
        else if (label == "o")
        {
            lineStream >> modelName;
        }
        else if (label == "vn")
        {
            glm::vec3 normal;
            lineStream >> normal.x >> normal.y >> normal.z;
            normals.push_back(normal);
        }
        else if (label == "vt")
        {
            glm::vec3 textureCoordinate;
            lineStream >> textureCoordinate.x >> textureCoordinate.y;
            textureCoordinates.push_back(textureCoordinate);
        }
        else if (label == "f")
        {
            modelCompleted = true;
            std::vector<std::string> indicesStrings;
            std::vector<uint32_t> polygonIndices;

            if (alignedNormals.empty() && !normals.empty())
                alignedNormals.resize(positions.size(), glm::vec3());

            if (alignedTextureCoordinates.empty() && !textureCoordinates.empty())
                alignedTextureCoordinates.resize(positions.size(), glm::vec2());

            while (lineStream >> indicesStrings.emplace_back()) {}

            if (indicesStrings.back().empty())
                indicesStrings.pop_back();

            for (const auto& indexString : indicesStrings)
            {
                std::istringstream indexStream(indexString);
                int positionIndex = -1, normalIndex = -1, textureCoordinateIndex = -1;
                indexStream >> positionIndex;
                indexStream.get();

                if (!textureCoordinates.empty())
                {
                    indexStream >> textureCoordinateIndex;
                    alignedTextureCoordinates[positionIndex-positionsOffset-1] = textureCoordinates[textureCoordinateIndex-1-texCoordsOffset];
                }

                if (!normals.empty())
                {
                    indexStream.get();              // Extract '/'
                    indexStream >> normalIndex;     
                    alignedNormals[positionIndex-1-positionsOffset] = normals[normalIndex-1-normalsOffset];
                }

                polygonIndices.push_back(positionIndex-1-positionsOffset);
            }

            indicesStrings.clear();

            for (uint32_t i = 0; i <= polygonIndices.size() - 2; i += 2)
            {
                indices.push_back(polygonIndices[i]);
                indices.push_back(polygonIndices[(i+1) % polygonIndices.size()]);
                indices.push_back(polygonIndices[(i+2) % polygonIndices.size()]);
            }
        }
    }

    modelCompleted = false;
    models.emplace_back(generateGPUVertexDataOnLoad).loadCustomModel(positions, alignedNormals, alignedTextureCoordinates, indices, Model::DrawingMode::Triangles, modelName);
    positionsOffset += positions.size();
    texCoordsOffset += textureCoordinates.size();
    normalsOffset += normals.size();
    positions.clear();
    indices.clear();
    normals.clear();
    textureCoordinates.clear();
    alignedNormals.clear();
    alignedTextureCoordinates.clear();
    modelFile.close();
}