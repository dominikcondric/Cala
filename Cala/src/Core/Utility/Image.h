#pragma once
#include <string>
#include <glm/vec2.hpp>
#include "Path.h"

class Image {
public:
	Image() = default;
	Image(const std::filesystem::path& path);
	~Image();
	void loadImage(const std::filesystem::path& path);
	void freeData();
	glm::ivec2 getDimensions() const { return glm::ivec2(width, height); }
	int getChannelCount() const { return channelCount; }
	const unsigned char* getData() const { return data; }

private:
	int height = 0;
	int width = 0;
	int channelCount = 0;
	unsigned char* data = nullptr;
};
