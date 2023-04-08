#include "Image.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include "Logger.h"

namespace Cala {
	Image::Image(const std::filesystem::path& pathToImage)
	{
		load(pathToImage);
	}

    Image::Image(Image &&other)
	{
		*this = std::move(other);
	}

    Image& Image::operator=(Image &&other)
    {
		width = other.width;
		height = other.height;
		channelCount = other.channelCount;
		data = other.data;
		other.data = nullptr;
		path = std::move(other.path);
		return *this;
    }

    Image::~Image()
    {
		if (data != nullptr)
			freeData();
	}

	void Image::load(const std::filesystem::path& pathToImage)
	{
		if (data != nullptr)
			freeData();

		if (!std::filesystem::exists(pathToImage))
		{
			Logger::getInstance().logErrorToConsole("Path " + pathToImage.string() + " not valid!");
			return;
		}

		if (!pathToImage.extension().compare("jpg") || !pathToImage.extension().compare("png") || !pathToImage.extension().compare("jpeg"))
		{
			Logger::getInstance().logErrorToConsole("Path " + pathToImage.string() + " has invalid file extension!");
			return;
		}

		path = pathToImage.string();
		data = stbi_load(path.c_str(), &width, &height, &channelCount, 0);

		if (data == nullptr)
			Logger::getInstance().logErrorToConsole("Image not loaded properly.");
	}

	void Image::freeData()
	{
		stbi_image_free(data);
		data = nullptr;
	}
}
