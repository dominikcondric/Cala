#pragma once
#include "Model.h"
#include <filesystem>
#include <regex>

namespace Cala {
    class ModelLoader {
    public:
        ModelLoader(bool _generateGPUVertexDataOnLoad) : generateGPUVertexDataOnLoad(_generateGPUVertexDataOnLoad) {}
        ModelLoader(const std::filesystem::path& modelPath);
        ~ModelLoader() = default;
        void loadFromObj(const std::filesystem::path& modelPath);
        std::vector<Model>& getModels() { return models; }

    private:
        std::vector<glm::vec3> positions;
        std::vector<glm::vec3> normals;
        std::vector<glm::vec2> textureCoordinates;
        std::vector<glm::vec3> alignedNormals;
        std::vector<glm::vec2> alignedTextureCoordinates;
        std::vector<uint32_t> indices;
        std::vector<std::pair<uint32_t, uint32_t>> normalTexPositionMapping;
        std::vector<Model> models;
        std::string modelName;
        bool modelCompleted = false;
        uint32_t positionsOffset = 0;
        uint32_t normalsOffset = 0;
        uint32_t texCoordsOffset = 0;
        bool generateGPUVertexDataOnLoad;
    };
}