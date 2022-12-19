#include "Image.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include "Core.h"

Image::Image(const std::filesystem::path& path)
{
	loadImage(path);
}

Image::~Image()
{
	if (data != nullptr)
		freeData();
}

void Image::loadImage(const std::filesystem::path& path)
{
	if (data != nullptr)
		freeData();

	if (!std::filesystem::exists(path))
	{
		LOG_ERROR("Path " << path.string() << " not valid!");
		return;
	}

	if (!path.extension().compare("jpg") || !path.extension().compare("png") || !path.extension().compare("jpeg"))
	{
		LOG_ERROR("Path " << path.string() << " has invalid file extension!");
		return;
	}

	this->path = path.string();
	data = stbi_load(path.string().c_str(), &width, &height, &channelCount, 0);

	if (data == nullptr)
		LOG_ERROR("Image not loaded properly.");
}

void Image::freeData()
{
	stbi_image_free(data);
	data = nullptr;
}
