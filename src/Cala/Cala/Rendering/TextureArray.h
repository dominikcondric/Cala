#pragma once
#include "ITexture.h"
#include <vector>
#include "Cala/Utility/Image.h"

namespace Cala {
    class TextureArray : public ITexture {
    public:
        TextureArray() = default;
        TextureArray(const std::vector<Image> &images, const RenderingStyle &renderingStyle, uint32_t _mipmapCount);
        ~TextureArray() override = default;
        void loadFromImages(const std::vector<Image> &images, const RenderingStyle &renderingStyle, uint32_t _mipmapCount);
        void load(const Specification &specification, uint32_t mipmapCount = 1, uint32_t layerCount = 2);
        uint32_t getLayerCount() const { return layerCount; }
        uint32_t getMipmapCount() const { return mipmapCount; }

    private:
        uint32_t mipmapCount = 0;
        uint32_t layerCount = 0;
    };
}