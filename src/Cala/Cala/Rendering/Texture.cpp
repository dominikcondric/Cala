#include "Texture.h"
#include <cstring>
#include "Framebuffer.h"
#include "GraphicsAPI.h"
#include "Cala/Utility/Logger.h"

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

        load(spec, texData);
    }

    void Texture::load2DTextureFromImage(const Image& image, const RenderingStyle& renderingStyle)
    {
        bool alpha = image.getChannelCount() == 4 ? true : false;
        Specification spec(image.getDimensions().x, image.getDimensions().y,  alpha ? Format::RGBA : Format::RGB, Dimensionality::TwoDimensional);
        spec.renderingStyle = renderingStyle;
        load(spec, (void*)image.getData());
    }

    void Texture::load(const Specification& specification, void* data)
    {
        if (isLoaded())
        {
            Logger::getInstance().logErrorToConsole("Texture already loaded!");
            return;
        }

        initializeData(specification);

        if (writeOnly)
        {
            glGenRenderbuffers(1, &textureHandle);
            nativeType = GL_RENDERBUFFER;
            glBindRenderbuffer(nativeType, textureHandle);
		    glRenderbufferStorage(nativeType, format, width, height);
            glBindRenderbuffer(nativeType, GL_NONE);
        }
        else 
        {
            glGenTextures(1, &textureHandle);
            switch (dimensionality)
            {
                case Dimensionality::OneDimensional:
                    // TODO
                    break;

                case Dimensionality::TwoDimensional:
                {
                    nativeType = GL_TEXTURE_2D;
                    glBindTexture(nativeType, textureHandle);
                    glTexImage2D(nativeType, 0, internalFormat, width, height, 0, format, dataType, data);
                    setParameters(specification);
                    break;
                }

                 case Dimensionality::ThreeDimensional:
                    // TODO
                    break;

                case Dimensionality::Cubemap:
                {
                    nativeType = GL_TEXTURE_CUBE_MAP;
                    glBindTexture(nativeType, textureHandle);
                    const void** texData = reinterpret_cast<const void**>(data);
                    for (int i = 0; i < 6; i++)
                        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internalFormat, width, height, 0, format, dataType, texData[i]);

                    setParameters(specification);
                    break;
                }
            }

            glBindTexture(nativeType, GL_NONE);
        }
    }

#else
#error Api not supported yet!
#endif
}