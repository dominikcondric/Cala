#include "OGLTexture.h"
#include <cstring>

OGLTexture::~OGLTexture()
{
	glDeleteTextures(1, &textureID);
}

OGLTexture::OGLTexture(OGLTexture&& other) noexcept
{
	*this = std::move(other);
}

OGLTexture& OGLTexture::operator=(OGLTexture&& other) noexcept
{
	memcpy(this, &other, sizeof(OGLTexture));
	other.textureID = GL_NONE;
	return *this;
}

void OGLTexture::setForSampling(uint32_t bindingIndex) const
{
	glActiveTexture(GL_TEXTURE0 + bindingIndex);
	glBindTexture(type, textureID);
}

void OGLTexture::setForWriting(uint32_t bindingIndex) const
{
	glBindImageTexture(bindingIndex, textureID, 0, GL_FALSE, 0, GL_READ_WRITE, texInternalFormat);
}

uint32_t OGLTexture::translateConstant(WrappingMethod method) const
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
}

void OGLTexture::loadCubemap(const std::array<Image, 6>& images, const TextureSpecification& specification)
{
	Format format;
	if (images[0].getChannelCount() == 4)
		format = Format::RGBA;
	else
		format = Format::RGB;

	const glm::ivec2 imageDims = images[0].getDimensions();
	const void* texData[6];
	for (uint32_t i = 0; i < 6; ++i)
		texData[i] = images[i].getData();

	loadTexture(texData, imageDims.x, imageDims.y, format, Type::ThreeDimensional, specification);
}

void OGLTexture::load2DTexture(const Image& image, const TextureSpecification& specification)
{
	Format format;
	if (image.getChannelCount() == 4)
		format = Format::RGBA;
	else
		format = Format::RGB;

	const glm::ivec2 imageDims = image.getDimensions();
	loadTexture(image.getData(), imageDims.x, imageDims.y, format, Type::TwoDimensional, specification);
}

void OGLTexture::loadTexture(const void* data, uint32_t texWidth, uint32_t texHeight, Format format, Type texType, const TextureSpecification& specification)
{
	width = texWidth;
	height = texHeight;

	if (textureID == GL_NONE)
		glGenTextures(1, &textureID);

	GLenum texFormat;
	switch (format)
	{
		case Texture::Format::RGB:
			if (specification.gammaCorrection)
			{
				texInternalFormat = GL_SRGB;
				texFormat = GL_RGB;
			}
			else
			{
				texFormat = texInternalFormat = GL_RGB;
			}
			break;
		case Texture::Format::RGBA:
			if (specification.gammaCorrection)
			{
				texInternalFormat = GL_SRGB_ALPHA;
				texFormat = GL_RGBA;
			} 
			else
			{
				texFormat = texInternalFormat = GL_RGBA;
			}
			break;
		case Texture::Format::RGB16F:
			texInternalFormat = GL_RGB16F;
			texFormat = GL_RGB;
			break;
		case Texture::Format::RGBA16F:
			texInternalFormat = GL_RGBA16F;
			texFormat = GL_RGBA;
			break;
		case Texture::Format::RGB32F:
			texInternalFormat = GL_RGB32F;
			texFormat = GL_RGB;
			break;
		case Texture::Format::RGBA32F:
			texInternalFormat = GL_RGBA32F;
			texFormat = GL_RGBA;
			break;
	}

	switch (texType)
	{
		case Texture::Type::TwoDimensional:
		{
			type = GL_TEXTURE_2D;
			glBindTexture(type, textureID);
			glTexImage2D(GL_TEXTURE_2D, 0, texInternalFormat, width, height, 0, texFormat, GL_UNSIGNED_BYTE, data);
			glTexParameteri(type, GL_TEXTURE_WRAP_S, translateConstant(specification.sDimensionWrap));
			glTexParameteri(type, GL_TEXTURE_WRAP_T, translateConstant(specification.tDimensionWrap));
			glTexParameteri(type, GL_TEXTURE_MIN_FILTER, specification.magFilter == Filter::Linear ? GL_LINEAR : GL_NEAREST);
			glTexParameteri(type, GL_TEXTURE_MAG_FILTER, specification.magFilter == Filter::Linear ? GL_LINEAR : GL_NEAREST);
			break;
		}

		case Texture::Type::ThreeDimensional:
		{
			type = GL_TEXTURE_CUBE_MAP;
			glBindTexture(type, textureID);
			const void** texData = (const void**)data;
			for (int i = 0; i < 6; i++)
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, texInternalFormat, width, height, 0, texFormat, GL_UNSIGNED_BYTE, texData[i]);

			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, specification.magFilter == Filter::Linear ? GL_LINEAR : GL_NEAREST);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, specification.magFilter == Filter::Linear ? GL_LINEAR : GL_NEAREST);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, translateConstant(specification.sDimensionWrap));
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, translateConstant(specification.tDimensionWrap));
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, translateConstant(specification.rDimensionWrap));
			break;
		}
	}

	if (specification.generateMipmaps)
		glGenerateMipmap(type);

	glBindTexture(type, GL_NONE);
}