#pragma once
#include "Core/Window.h"

class FileDialog {
public:
	FileDialog(const Window& parentWindow);
	~FileDialog() = default;
	void setInitialDirectory(const std::string& dir) { initialDirectory = dir; }

	/*
	* Filters must be in form of 'display\0name.extension\0' where both name and extension can be wildcards(*).
	* Multiple filters per display must be separated with ; (example: 'display\0*.png;*jpg\0').
	* String at the end must have two null terminators \0\0
	*/ 
	void setFilters(const char* filtersString);
	std::string openFile() const;
	std::string saveFile() const;

private:
	void* parentWindow = nullptr;
	std::string initialDirectory;
	const char* filters = "All\0";
};
