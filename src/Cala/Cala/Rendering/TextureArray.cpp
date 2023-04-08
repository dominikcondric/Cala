#include "TextureArray.h"
#include "GraphicsAPI.h"

namespace Cala {
#ifdef CALA_API_OPENGL
#include <glad/glad.h>
    TextureArray::TextureArray(const std::vector<Image> &images, const RenderingStyle& renderingStyle, uint32_t _mipmapCount)
    {
        loadFromImages(images, renderingStyle, _mipmapCount);
    }

    void TextureArray::loadFromImages(const std::vector<Image> &images, const RenderingStyle& renderingStyle, uint32_t _mipmapCount)
    {
        Specification spec(images[0].getDimensions().x, images[0].getDimensions().y, 
            (images[0].getChannelCount() == 4) ? Format::RGBA : Format::RGB, Dimensionality::TwoDimensional);
        spec.writeOnly = false;
        spec.renderingStyle = renderingStyle;
        load(spec, _mipmapCount, (uint32_t)images.size());
    }

    void TextureArray::load(const Specification &specification, uint32_t _mipmapCount, uint32_t _layerCount)
    {
        initializeData(specification);

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
                nativeType = GL_TEXTURE_2D_ARRAY;
                glBindTexture(nativeType, textureHandle);
                glTexStorage3D(nativeType, mipmapCount, internalFormat, width, height, layerCount);
                setParameters(specification);
                break;

            case Dimensionality::ThreeDimensional:
                // TODO
                break;

            case Dimensionality::Cubemap:
                // TODO
                break;
        }

        glBindTexture(nativeType, GL_NONE);
    }
#endif
}