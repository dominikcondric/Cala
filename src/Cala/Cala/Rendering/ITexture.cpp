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
        nativeType = other.nativeType;
        internalFormat = other.internalFormat;
        format = other.format;
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
        glBindTexture(nativeType, textureHandle);
    }

    bool ITexture::isDepth() const
    {
        return format == GL_DEPTH_COMPONENT;
    }

    bool ITexture::isColor() const
    {
        return !isDepth() && !isDepthStencil();
    }

    bool ITexture::isDepthStencil() const
    {
        return format == GL_DEPTH_STENCIL;
    }

    void ITexture::initializeData(const Specification &specification)
    {
        width = specification.width;
        height = specification.height;
        dimensionality = specification.dimensionality;
        writeOnly = specification.writeOnly;
        
        switch (specification.format)
        {
            case Format::RGB:
                internalFormat = GL_RGB;
                format = GL_RGB;
                dataType = GL_UNSIGNED_BYTE;
                break;
            case Format::RGBA:
                internalFormat = GL_RGBA;
                format = GL_RGB;
                dataType = GL_UNSIGNED_BYTE;
                break;
            case Format::GAMMA_RGBA:
                internalFormat = GL_SRGB_ALPHA;
                format = GL_SRGB;
                dataType = GL_UNSIGNED_BYTE;
                break;
            case Format::FLOAT_RGBA:
                internalFormat = GL_RGBA32F;
                format = GL_RGB;
                dataType = GL_FLOAT;
                break;
            case Format::DEPTH16:
                internalFormat = GL_DEPTH_COMPONENT16;
                format = GL_DEPTH_COMPONENT;
                dataType = GL_FLOAT;
                break;
            case Format::DEPTH32:
                internalFormat = GL_DEPTH_COMPONENT32;
                format = GL_DEPTH_COMPONENT;
                dataType = GL_FLOAT;
                break;
            case Format::DEPTH24_STENCIL8:
                internalFormat = GL_DEPTH24_STENCIL8;
                format = GL_DEPTH_STENCIL;
                dataType = GL_UNSIGNED_INT_24_8;
                break;
        }
    }

    void ITexture::setParameters(const Specification &specification) const
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

        glTexParameteri(nativeType, GL_TEXTURE_MAG_FILTER, specification.renderingStyle.magFilter == Filter::Linear ? GL_LINEAR : GL_NEAREST);
        glTexParameteri(nativeType, GL_TEXTURE_MIN_FILTER, specification.renderingStyle.minFilter == Filter::Linear ? GL_LINEAR : GL_NEAREST);
        glTexParameterfv(nativeType, GL_TEXTURE_BORDER_COLOR, specification.borderColor);
        glTexParameteri(nativeType, GL_TEXTURE_WRAP_S, translateMethod(specification.renderingStyle.sDimensionWrap));

        if (dimensionality == Dimensionality::TwoDimensional) 
            glTexParameteri(nativeType, GL_TEXTURE_WRAP_T, translateMethod(specification.renderingStyle.tDimensionWrap));
        
        if (dimensionality == Dimensionality::ThreeDimensional || dimensionality == Dimensionality::Cubemap) 
            glTexParameteri(nativeType, GL_TEXTURE_WRAP_R, translateMethod(specification.renderingStyle.rDimensionWrap));

        if (!isColor())
        {
            glTexParameteri(nativeType, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
            glTexParameteri(nativeType, GL_TEXTURE_COMPARE_FUNC, GL_GEQUAL);
        }
    }
#else
    #error Api not supported yet!
#endif
}