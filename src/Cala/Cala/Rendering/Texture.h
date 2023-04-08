#pragma once
#include <string>
#include "ITexture.h"
#include "NativeAPI.h"
#include <array>
#include <vector>
#include "Cala/Utility/Image.h"

namespace Cala {
	class Texture : public ITexture {
	public:
		Texture() = default;
        Texture(const Image &image, const RenderingStyle& renderingStyle = RenderingStyle());
        Texture(const std::array<Image, 6> &images, const RenderingStyle& renderingStyle = RenderingStyle());
        Texture(const Texture &other) = delete;
        Texture(Texture&& other) noexcept;
		Texture& operator=(const Texture& other) = delete;
		Texture& operator=(Texture&& other) noexcept;
		~Texture() override = default;
		void load2DTextureFromImage(const Image& image, const RenderingStyle& renderingStyle = RenderingStyle());
        void loadCubemapFromImages(const std::array<Image, 6> &images, const RenderingStyle& renderingStyle = RenderingStyle());
        void load(const Specification &specification, void *data);
	};
}

