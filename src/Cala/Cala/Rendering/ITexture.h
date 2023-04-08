#pragma once
#include "NativeAPI.h"
#include <glm/vec2.hpp>

namespace Cala {
    class Framebuffer;

    class ITexture {
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

		enum class Format {
			RGB,
			RGBA,
			GAMMA_RGBA,
			FLOAT_RGBA,
			DEPTH16,
			DEPTH32,
			DEPTH24_STENCIL8
		};

		enum class Dimensionality {
			OneDimensional,
			TwoDimensional,
			ThreeDimensional,
			Cubemap
		};

		struct RenderingStyle {
			WrappingMethod sDimensionWrap = WrappingMethod::ClampToBorder;
			WrappingMethod tDimensionWrap = WrappingMethod::ClampToBorder;
			WrappingMethod rDimensionWrap = WrappingMethod::ClampToBorder;
			Filter magFilter = Filter::Linear;
			Filter minFilter = Filter::Linear;
		};

        struct Specification {
			Specification(uint32_t _width, uint32_t _height, Format _format, Dimensionality _dimensionality) :
			 width(_width), height(_height), format(_format), dimensionality(_dimensionality) {}
			~Specification() = default;
			RenderingStyle renderingStyle;
			uint32_t width;
			uint32_t height;
			Format format;
			Dimensionality dimensionality;
			bool writeOnly = false;
			float borderColor[4]{1.f, 1.f, 1.f, 1.f};
		};

        ITexture() = default;
        virtual ~ITexture() = 0;
        ITexture(const ITexture& other) = delete;
        ITexture(ITexture&& other) noexcept;
		ITexture& operator=(const ITexture& other) = delete;
		ITexture& operator=(ITexture&& other) noexcept;
        // virtual void attachToFramebuffer(Framebuffer& framebuffer) = 0;
		glm::ivec2 getDimensions() const { return glm::ivec2(width, height); }
		void setForSampling(uint32_t bindingIndex) const;
		bool isWriteOnly() const { return writeOnly; }
		bool isDepth() const;
		bool isColor() const;
		bool isDepthStencil() const;
		Dimensionality getDimensionality() const { return dimensionality; }

	protected:
		void initializeData(const Specification& specification);
		void setParameters(const Specification& specification) const;
        int width = 0;
        int height = 0;
		bool writeOnly = false;
		Dimensionality dimensionality;

	// API specific
	#ifdef CALA_API_OPENGL
	protected: 
        GLuint textureHandle = API_NULL;
        GLenum nativeType;
		GLenum internalFormat;
		GLenum format;
		GLenum dataType;
	public:
        GLuint getNativeHandle() const { return textureHandle; }
	#endif
    }; 
}