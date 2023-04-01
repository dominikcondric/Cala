#include "Texture.h"
#include <cstring>
#include "Framebuffer.h"
#include "GraphicsAPI.h"

namespace Cala {
#ifdef CALA_API_OPENGL
#include <glad/glad.h>
    Texture::Texture(const Image& image, const RenderingStyle& renderingStyle)
    {
        load2DTextureFromImage(image, renderingStyle);
    }

    Texture::Texture(const std::array<Image, 6>& images, const RenderingStyle& renderingStyle)
    {
        loadCubemapFromImages(images, renderingStyle);
    }

    Texture::Texture(Texture&& other) noexcept : ITexture(std::move(other))
    {
    }

    Texture& Texture::operator=(Texture&& other) noexcept
    {
        ITexture::operator=(std::move(other));
        return *this;
    }

    void Texture::loadCubemapFromImages(const std::array<Image, 6>& images, const RenderingStyle& renderingStyle)
    {
        bool alpha = images[0].getChannelCount() == 4 ? true : false;
        Specification spec(images[0].getDimensions().x, images[0].getDimensions().y, 
            alpha ? Format::RGBA : Format::RGB, Dimensionality::Cubemap);
        spec.renderingStyle = renderingStyle;

        const void* texData[6];
        for (uint32_t i = 0; i < 6; ++i)
            texData[i] = images[i].getData();

        generateTexture(spec, texData);
    }

    void Texture::load2DTextureFromImage(const Image& image, const RenderingStyle& renderingStyle)
    {
        bool alpha = image.getChannelCount() == 4 ? true : false;
        Specification spec(image.getDimensions().x, image.getDimensions().y,  alpha ? Format::RGBA : Format::RGB, Dimensionality::TwoDimensional);
        spec.renderingStyle = renderingStyle;
        generateTexture(spec, (void*)image.getData());
    }

    void Texture::generateTexture(const Specification& specification, void* data)
    {
        initializeTextureData(specification);

        if (writeOnly)
        {
            if (textureHandle == GL_NONE)
                glGenRenderbuffers(1, &textureHandle);
            
            nativeTextureType = GL_RENDERBUFFER;
            glBindRenderbuffer(nativeTextureType, textureHandle);
		    glRenderbufferStorage(nativeTextureType, internalTextureFormat, width, height);
            glBindRenderbuffer(nativeTextureType, GL_NONE);
        }
        else 
        {
            if (textureHandle == GL_NONE)
                glGenTextures(1, &textureHandle);

            switch (dimensionality)
            {
                case Dimensionality::OneDimensional:
                    // TODO
                    break;

                case Dimensionality::TwoDimensional:
                {
                    nativeTextureType = GL_TEXTURE_2D;
                    glBindTexture(nativeTextureType, textureHandle);
                    glTexImage2D(nativeTextureType, 0, internalTextureFormat, width, height, 0, textureFormat, dataType, data);
                    setTextureParameters(specification);
                    break;
                }

                 case Dimensionality::ThreeDimensional:
                    // TODO
                    break;

                case Dimensionality::Cubemap:
                {
                    nativeTextureType = GL_TEXTURE_CUBE_MAP;
                    glBindTexture(nativeTextureType, textureHandle);
                    const void** texData = reinterpret_cast<const void**>(data);
                    for (int i = 0; i < 6; i++)
                        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internalTextureFormat, width, height, 0, textureFormat, dataType, texData[i]);

                    setTextureParameters(specification);
                    break;
                }
            }

            glBindTexture(nativeTextureType, GL_NONE);
        }
    }

#else
#error Api not supported yet!
#endif
}