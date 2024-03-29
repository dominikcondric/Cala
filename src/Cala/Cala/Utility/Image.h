#pragma once
#include <string>
#include <glm/vec2.hpp>
#include <filesystem>

namespace Cala {
	class Image {
	public:
		Image() = default;
		Image(const std::filesystem::path& pathToImage);
		Image(const Image&) = delete;
		Image(Image&& other);
		Image& operator=(const Image&) = delete;
		Image& operator=(Image&& other);
		~Image();
		void load(const std::filesystem::path& pathToImage);
		void freeData();
		glm::ivec2 getDimensions() const { return glm::ivec2(width, height); }
		int getChannelCount() const { return channelCount; }
		const unsigned char* getData() const { return data; }
		const std::string& getPath() const { return path; }

	private:
		int height = 0;
		int width = 0;
		int channelCount = 0;
		unsigned char* data = nullptr;
		std::string path;
	};
}
