#pragma once
#include <string>
#include "Utility/Image.h"
#include <array>

typedef uint32_t GLuint;

class Texture {
public:
	enum class WrappingMethod {
		ClampToEdge,
		ClampToBorder,
		Repeat,
		MirroredRepeat
	};

	enum class Filter {
		Linear,
		Nearest
	};

	struct Specification {
		WrappingMethod sDimensionWrap = WrappingMethod::ClampToBorder;
		WrappingMethod tDimensionWrap = WrappingMethod::ClampToBorder;
		WrappingMethod rDimensionWrap = WrappingMethod::ClampToBorder;
		Filter magFilter = Filter::Linear;
		Filter minFilter = Filter::Linear;
		bool generateMipmaps = false;
		bool gammaCorrection = false;
	};

	enum class Format {
		RGB,
		RGBA,
		RGB16F,
		RGBA16F,
		RGB32F,
		RGBA32F
	};

	enum class Type {
		OneDimensional,
		TwoDimensional,
		ThreeDimensional
	};

public:
	Texture() = default;
	Texture(const Image& image, const Specification& specification);
	Texture(const std::array<Image, 6>& images, const Specification& specification);
	Texture(const Texture& other) = delete;
	Texture(Texture&& other) noexcept;
	Texture& operator=(const Texture& other) = delete;
	Texture& operator=(Texture&& other) noexcept;
	~Texture();
	const std::string& getImagePath() const { return imagePath; }
	glm::ivec2 getImageDimensions() const { return glm::ivec2(width, height); }
	void setForSampling(uint32_t bindingIndex) const;
	void setForWriting(uint32_t bindingIndex) const;
	void load2DTexture(const Image& image, const Specification& specification);
	const void* const getNativeTextureHandle() const;
	//void loadCubemap(const Image& image, const TextureSpecification& specification);
	void loadCubemap(const std::array<Image, 6>& images, const Specification& specification);

private:
	void loadTexture(const void* data, uint32_t texWidth, uint32_t texHeight, Format format, Type texType, const Specification& specification);
	uint32_t translateConstant(WrappingMethod method) const;
	int width = 0;
	int height = 0;
	std::string imagePath = "";
#if CALA_API == CALA_API_OPENGL
	GLuint textureID = 0;
	GLuint texInternalFormat;
	GLuint type;
#else
	#error Api not supported yet!
#endif
};

