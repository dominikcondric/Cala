#include "ITexture.h"
#include <cstring>
#include "Framebuffer.h"

namespace Cala {
#ifdef CALA_API_OPENGL
    #include <glad/glad.h>
    ITexture::~ITexture()
    {
        if (writeOnly)
            glDeleteRenderbuffers(1, &textureHandle);
        else
            glDeleteTextures(1, &textureHandle);
    }

    ITexture::ITexture(ITexture&& other) noexcept
    {
        *this = std::move(other);
    }

    ITexture& ITexture::operator=(ITexture&& other) noexcept
    {
        textureHandle = other.textureHandle;
        width = other.width;
        height = other.height;
        nativeTextureType = other.nativeTextureType;
        internalTextureFormat = other.internalTextureFormat;
        textureFormat = other.textureFormat;
        dataType = other.dataType;
        other.textureHandle = GL_NONE;
        writeOnly = other.writeOnly;
        dimensionality = other.dimensionality;
        return *this;
    }

    void ITexture::setForSampling(uint32_t bindingIndex) const
    {
        if (writeOnly)
            return;

        glActiveTexture(GL_TEXTURE0 + bindingIndex);
        glBindTexture(nativeTextureType, textureHandle);
    }

    bool ITexture::isDepth() const
    {
        return textureFormat == GL_DEPTH_COMPONENT;
    }

    bool ITexture::isColor() const
    {
        return !isDepth() && !isDepthStencil();
    }

    bool ITexture::isDepthStencil() const
    {
        return textureFormat == GL_DEPTH_STENCIL;
    }

    void ITexture::initializeTextureData(const Specification &specification)
    {
        width = specification.width;
        height = specification.height;
        dimensionality = specification.dimensionality;
        writeOnly = specification.writeOnly;

        
        switch (specification.format)
        {
            case Format::RGB:
                internalTextureFormat = GL_RGB;
                textureFormat = GL_RGB;
                dataType = GL_UNSIGNED_BYTE;
                break;
            case Format::RGBA:
                internalTextureFormat = GL_RGBA;
                textureFormat = GL_RGB;
                dataType = GL_UNSIGNED_BYTE;
                break;
            case Format::GAMMA_RGBA:
                internalTextureFormat = GL_SRGB_ALPHA;
                textureFormat = GL_SRGB;
                dataType = GL_UNSIGNED_BYTE;
                break;
            case Format::FLOAT_RGBA:
                internalTextureFormat = GL_RGBA32F;
                textureFormat = GL_RGB;
                dataType = GL_FLOAT;
                break;
            case Format::DEPTH16:
                internalTextureFormat = GL_DEPTH_COMPONENT16;
                textureFormat = GL_DEPTH_COMPONENT;
                dataType = GL_FLOAT;
                break;
            case Format::DEPTH32:
                internalTextureFormat = GL_DEPTH_COMPONENT32;
                textureFormat = GL_DEPTH_COMPONENT;
                dataType = GL_FLOAT;
                break;
            case Format::DEPTH24_STENCIL8:
                internalTextureFormat = GL_DEPTH24_STENCIL8;
                textureFormat = GL_DEPTH_STENCIL;
                dataType = GL_UNSIGNED_INT_24_8;
                break;
        }
    }

    void ITexture::setTextureParameters(const Specification &specification) const
    {
        auto translateMethod = [](WrappingMethod method) -> GLuint
        {
            switch (method)
            {
                case Texture::WrappingMethod::ClampToEdge:
                    return GL_CLAMP_TO_EDGE;
                    break;
                case Texture::WrappingMethod::ClampToBorder:
                    return GL_CLAMP_TO_BORDER;
                    break;
                case Texture::WrappingMethod::Repeat:
                    return GL_REPEAT;
                    break;
                case Texture::WrappingMethod::MirroredRepeat:
                    return GL_MIRRORED_REPEAT;
                    break;
                default:
                    return GL_CLAMP_TO_EDGE;
            }
        };

        glTexParameteri(nativeTextureType, GL_TEXTURE_MAG_FILTER, specification.renderingStyle.magFilter == Filter::Linear ? GL_LINEAR : GL_NEAREST);
        glTexParameteri(nativeTextureType, GL_TEXTURE_MIN_FILTER, specification.renderingStyle.minFilter == Filter::Linear ? GL_LINEAR : GL_NEAREST);
        glTexParameterfv(nativeTextureType, GL_TEXTURE_BORDER_COLOR, specification.borderColor);
        glTexParameteri(nativeTextureType, GL_TEXTURE_WRAP_S, translateMethod(specification.renderingStyle.sDimensionWrap));

        if (dimensionality == Dimensionality::TwoDimensional) 
            glTexParameteri(nativeTextureType, GL_TEXTURE_WRAP_T, translateMethod(specification.renderingStyle.tDimensionWrap));
        
        if (dimensionality == Dimensionality::ThreeDimensional || dimensionality == Dimensionality::Cubemap) 
            glTexParameteri(nativeTextureType, GL_TEXTURE_WRAP_R, translateMethod(specification.renderingStyle.rDimensionWrap));

        if (!isColor())
        {
            glTexParameteri(nativeTextureType, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
            glTexParameteri(nativeTextureType, GL_TEXTURE_COMPARE_FUNC, GL_GEQUAL);
        }
    }
#else
    #error Api not supported yet!
#endif
}