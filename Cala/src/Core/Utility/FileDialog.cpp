#include "FileDialog.h"

FileDialog::FileDialog(const Window& parentWindow) : parentWindow(parentWindow.getNativeWindowPointer())
{	
}

#if defined(CALA_PLATFORM_WINDOWS)
#include <windows.h>
#include <commdlg.h>

void FileDialog::setFilters(const char* filtersString)
{
	filters = filtersString;
}

std::string FileDialog::openFile() const
{
	std::string openedFile;

	OPENFILENAME ofn;       // common dialog box structure
	char szFile[260];       // buffer for file name

	// Initialize OPENFILENAME
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = static_cast<HWND>(parentWindow);
	ofn.lpstrFile = szFile;
	// Set lpstrFile[0] to '\0' so that GetOpenFileName does not 
	// use the contents of szFile to initialize itself.
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = filters;
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = initialDirectory.c_str();
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	// Display the Open dialog box. 

	if (GetOpenFileName(&ofn) == TRUE)
	{
		openedFile.assign(ofn.lpstrFile);
	}

	return openedFile;
}

std::string FileDialog::saveFile() const
{
	std::string savedFile;

	OPENFILENAME ofn;       // common dialog box structure
	char szFile[260];       // buffer for file name

	// Initialize OPENFILENAME
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = static_cast<HWND>(parentWindow);
	ofn.lpstrFile = szFile;
	// Set lpstrFile[0] to '\0' so that GetOpenFileName does not 
	// use the contents of szFile to initialize itself.
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = filters;
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = "Scene";
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = initialDirectory.c_str();
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	// Display the Save dialog box. 

	if (GetSaveFileName(&ofn) == TRUE)
	{
		savedFile.assign(ofn.lpstrFile);
	}

	return savedFile;
}

#elif defined CALA_PLATFORM_LINUX | CALA_PLATFORM_APPLE
#error "Linux and MacOS file dialogs not supported yet!"
#endif






