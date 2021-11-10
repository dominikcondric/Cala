#pragma once
#include <string>
#include "Core/Utility/Image.h"
#include <array>

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

	struct TextureSpecification {
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
	virtual ~Texture() = default;
	virtual void setForSampling(uint32_t bindingIndex) const = 0;
	virtual void setForWriting(uint32_t bindingIndex) const = 0;
	virtual void load2DTexture(const Image& image, const TextureSpecification& specification) = 0;
	virtual void loadTexture(const void* data, uint32_t texWidth, uint32_t texHeight, Format format, Type texType, const TextureSpecification& specification) = 0;
	virtual const void* const getNativeTextureHandle() const = 0;
	//virtual void loadCubemap(const Image& image, const TextureSpecification& specification) = 0;
	virtual void loadCubemap(const std::array<Image, 6>& images, const TextureSpecification& specification) = 0;

private:
	virtual uint32_t translateConstant(WrappingMethod method) const = 0;

protected:
	int width = 0;
	int height = 0;
};

