#include "TextureArray.h"
#include "GraphicsAPI.h"

namespace Cala {
#ifdef CALA_API_OPENGL
#include <glad/glad.h>
    TextureArray::TextureArray(const std::vector<Image> &images, const RenderingStyle& renderingStyle, uint32_t _mipmapCount)
    {
        loadTextureFromImages(images, renderingStyle, _mipmapCount);
    }

    void TextureArray::loadTextureFromImages(const std::vector<Image> &images, const RenderingStyle& renderingStyle, uint32_t _mipmapCount)
    {
        Specification spec(images[0].getDimensions().x, images[0].getDimensions().y, 
            (images[0].getChannelCount() == 4) ? Format::RGBA : Format::RGB, Dimensionality::TwoDimensional);
        spec.writeOnly = false;
        spec.renderingStyle = renderingStyle;
        generateTextureArray(spec, _mipmapCount, images.size());
    }

    void TextureArray::generateTextureArray(const Specification &specification, uint32_t _mipmapCount, uint32_t _layerCount)
    {
        initializeTextureData(specification);
        if (_mipmapCount == 0)
            _mipmapCount = 1;
        mipmapCount = _mipmapCount;
        layerCount = _layerCount;

        if (textureHandle == API_NULL)
            glGenTextures(1, &textureHandle);

        switch (dimensionality)
        {
            case Dimensionality::OneDimensional:
                // TODO
                break;

            case Dimensionality::TwoDimensional:
                nativeTextureType = GL_TEXTURE_2D_ARRAY;
                glBindTexture(nativeTextureType, textureHandle);
                glTexStorage3D(nativeTextureType, mipmapCount, internalTextureFormat, width, height, layerCount);
                setTextureParameters(specification);
                break;

            case Dimensionality::ThreeDimensional:
                // TODO
                break;

            case Dimensionality::Cubemap:
                // TODO
                break;
        }

        glBindTexture(nativeTextureType, GL_NONE);
    }
#endif
}